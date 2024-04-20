#include <stdio.h>

/* Sundials */
#include <cvode/cvode.h>
#include <nvector/nvector_serial.h>
#include <sunlinsol/sunlinsol_dense.h>
#include <sunmatrix/sunmatrix_dense.h>

/* User-defined vector and matrix accessor macros: Ith, IJth */

/* These macros are defined in order to write code which exactly matches
   the mathematical problem description given above.

   Ith(v,i) references the ith component of the vector v, where i is in
   the range [1..NEQ] and NEQ is defined below. The Ith macro is defined
   using the N_VIth macro in nvector.h. N_VIth numbers the components of
   a vector starting from 0.

   IJth(A,i,j) references the (i,j)th element of the dense matrix A, where
   i and j are in the range [1..NEQ]. The IJth macro is defined using the
   SM_ELEMENT_D macro. SM_ELEMENT_D numbers rows and columns of
   a dense matrix starting from 0. */

#define Ith(v, i) NV_Ith_S(v, i - 1) /* i-th vector component i=1..NEQ */
#define IJth(A, i, j) SM_ELEMENT_D(A, i - 1, j - 1) /* (i,j)-th matrix component i,j=1..NEQ */

/* Problem Constants */

#define NEQ   7               /* number of equations  */
#define WX0   SUN_RCONST(0.2) /* initial y components */
#define WY0   SUN_RCONST(0.3)
#define WZ0   SUN_RCONST(0.1)
#define Q10   SUN_RCONST(0.0)
#define Q20   SUN_RCONST(0.0)
#define Q30   SUN_RCONST(0.0)
#define Q40   SUN_RCONST(1.0)

#define IXX   SUN_RCONST(3463.542)
#define IYY   SUN_RCONST(3463.542)
#define IZZ   SUN_RCONST(2968.75)

#define RTOL  SUN_RCONST(1.0e-4) /* scalar relative tolerance            */
#define ATOL1 SUN_RCONST(1.0e-8) /* vector absolute tolerance components */
#define ATOL2 SUN_RCONST(1.0e-14)
#define ATOL3 SUN_RCONST(1.0e-6)
#define T0    SUN_RCONST(0.0)  /* initial time           */
#define T1    SUN_RCONST(20.0)  /* first output time      */

#define ZERO SUN_RCONST(0.0)

/* Type : UserData (contains grid constants) */

typedef struct
{
  SUNMatrix I;
}* UserData;

/* Functions Called by the Solver */

static int f(sunrealtype t, N_Vector y, N_Vector ydot, void* user_data);

/* Private functions to output results */

static void PrintOutput(int i, sunrealtype t, N_Vector y);

/* Private function to check function return values */

static int check_retval(void* returnvalue, const char* funcname, int opt);

/*
 *-------------------------------
 * Main Program
 *-------------------------------
 */

int main(void)
{
  SUNContext sunctx;
  sunrealtype t, tout;
  N_Vector y;
  N_Vector abstol;
  SUNMatrix A, I;
  UserData user_data;
  SUNLinearSolver LS;
  void* cvode_mem;
  int retval, iout;
  FILE* FID;

  sunrealtype t_stop = T1;

  y         = NULL;
  A         = NULL;
  abstol    = NULL;
  I         = NULL;
  LS        = NULL;
  cvode_mem = NULL;

  /* Create the SUNDIALS context */
  retval = SUNContext_Create(SUN_COMM_NULL, &sunctx);
  if (check_retval(&retval, "SUNContext_Create", 1)) { return (1); }

  /* Initial conditions */
  y = N_VNew_Serial(NEQ, sunctx);
  if (check_retval((void*)y, "N_VNew_Serial", 0)) { return (1); }

  /* Initialize y */
  Ith(y, 1) = WX0;
  Ith(y, 2) = WY0;
  Ith(y, 3) = WZ0;
  Ith(y, 4) = Q10;
  Ith(y, 5) = Q20;
  Ith(y, 6) = Q30;
  Ith(y, 7) = Q40;

  /* Set the vector absolute tolerance */
  abstol = N_VNew_Serial(NEQ, sunctx);
  if (check_retval((void*)abstol, "N_VNew_Serial", 0)) { return (1); }

  Ith(abstol, 1) = ATOL1;
  Ith(abstol, 2) = ATOL2;
  Ith(abstol, 3) = ATOL3;
  Ith(abstol, 4) = ATOL1;
  Ith(abstol, 5) = ATOL1;
  Ith(abstol, 6) = ATOL1;
  Ith(abstol, 7) = ATOL1;

  /* Call CVodeCreate to create the solver memory and specify the
   * Backward Differentiation Formula */
  cvode_mem = CVodeCreate(CV_ADAMS, sunctx);
  if (check_retval((void*)cvode_mem, "CVodeCreate", 0)) { return (1); }

  /* Call CVodeInit to initialize the integrator memory and specify the
   * user's right hand side function in y'=f(t,y), the initial time T0, and
   * the initial dependent variable vector y. */
  retval = CVodeInit(cvode_mem, f, T0, y);
  if (check_retval(&retval, "CVodeInit", 1)) { return (1); }

  /* Call CVodeSVtolerances to specify the scalar relative tolerance
   * and vector absolute tolerances */
  retval = CVodeSVtolerances(cvode_mem, RTOL, abstol);
  if (check_retval(&retval, "CVodeSVtolerances", 1)) { return (1); }

  /* Generate Inertia matrix*/
  I = SUNDenseMatrix(3, 3, sunctx);
  if (check_retval((void*)I, "SUNDenseMatrix", 0)) { return (1); }
  IJth(I, 1, 1) = IXX; 
  IJth(I, 2, 2) = IYY; 
  IJth(I, 3, 3) = IZZ;

  /* Set the pointer to user-defined data */
  user_data = (UserData)malloc(sizeof *user_data); /* Allocate data memory */
  if (!user_data)
  {
    fprintf(stderr, "MEMORY_ERROR: malloc failed - returned NULL pointer\n");
    return 1;
  }
  user_data->I = I;

  retval = CVodeSetUserData(cvode_mem, user_data);
  if (check_retval(&retval, "CVodeSetUserData", 1)) { return (1); }

    // Attach linear solver
  A = SUNDenseMatrix(NEQ, NEQ, sunctx);
  LS = SUNLinSol_Dense(y, A, sunctx);

  retval = CVodeSetLinearSolver(cvode_mem, LS, A);
  if (check_retval(&retval, "CVodeSetLinearSolver", 1)) { return 1; }

  /* In loop, call CVode, print results, and test for error.
     Break out of loop when NOUT preset output times have been reached.  */
  printf(" \nAttitude propagation\n\n");

  /* Open file for printing statistics */
  FID = fopen("integration_stats.csv", "w");

  iout = 0;
  tout = T0+1;
  while (t < T1)
  {
    retval = CVode(cvode_mem, tout, y, &t, CV_NORMAL);
    PrintOutput(iout, t, y);

    if (check_retval(&retval, "CVode", 1)) { break; }
    if (retval == CV_SUCCESS)
    {
      iout++;
      tout += 1;
    }

    retval = CVodePrintAllStats(cvode_mem, FID, SUN_OUTPUTFORMAT_CSV);
  }
  fclose(FID);

  /* Print final statistics to the screen */
  printf("\nFinal Statistics:\n");
  retval = CVodePrintAllStats(cvode_mem, stdout, SUN_OUTPUTFORMAT_TABLE);

  /* Free memory */
  N_VDestroy(y);            /* Free y vector */
  N_VDestroy(abstol);       /* Free abstol vector */
  SUNLinSolFree(LS);
  CVodeFree(&cvode_mem);    /* Free CVODE memory */
  SUNMatDestroy(A);
  SUNMatDestroy(I);         /* Free the matrix memory */
  SUNContext_Free(&sunctx); /* Free the SUNDIALS context */
  free(user_data);

  return (retval);
}

/*
 *-------------------------------
 * Functions called by the solver
 *-------------------------------
 */

/*
 * f routine. Compute function f(t,y).
 */

static int f(sunrealtype t, N_Vector y, N_Vector ydot, void* user_data)
{
  sunrealtype wx, wy, wz, q1, q2, q3, q4;
  sunrealtype dwx, dwy, dwz, dq1, dq2, dq3, dq4;
  SUNMatrix I;
  UserData data;

  /* Retrieve user data */
  data  = (UserData)user_data;
  I = data->I;
  
  wx = Ith(y, 1);
  wy = Ith(y, 2);
  wz = Ith(y, 3);
  q1 = Ith(y, 4);
  q2 = Ith(y, 5);
  q3 = Ith(y, 6);
  q4 = Ith(y, 7);

  /* Retrieve Inertia matrix columns */
  sunrealtype** I_cols = SUNDenseMatrix_Cols(I);

  dwx = Ith(ydot, 1) = (ZERO - (I_cols[2][2] - I_cols[1][1]) * wz * wy) / I_cols[0][0];
  dwy = Ith(ydot, 3) = (ZERO - (I_cols[0][0] - I_cols[2][2]) * wx * wz) / I_cols[1][1];
  dwz = Ith(ydot, 3) = (ZERO - (I_cols[1][1] - I_cols[0][0]) * wy * wx) / I_cols[2][2];
  dq1 = Ith(ydot, 4) = SUN_RCONST(0.5) * (wz * q2 - wy * q3 + wx * q4);
  dq2 = Ith(ydot, 5) = SUN_RCONST(0.5) * (-wz * q1 + wx * q3 + wy * q4);
  dq3 = Ith(ydot, 6) = SUN_RCONST(0.5) * (wy * q1 - wx * q2 + wz * q4);
  dq4 = Ith(ydot, 7) = SUN_RCONST(0.5) * (-wx * q1 - wy * q2 - wz * q3);

  return (0);
}

/*
 *-------------------------------
 * Private helper functions
 *-------------------------------
 */

static void PrintOutput(int i, sunrealtype t, N_Vector y)
{
  printf("At (%d) t = %0.4e      y =%14.6e  %14.6e  %14.6e  %14.6e  %14.6e  %14.6e  %14.6e\n", i, t, \
  Ith(y, 1), Ith(y, 2), Ith(y, 3), Ith(y, 4), Ith(y, 5), Ith(y, 6), Ith(y, 7));

  return;
}

/*
 * Check function return value...
 *   opt == 0 means SUNDIALS function allocates memory so check if
 *            returned NULL pointer
 *   opt == 1 means SUNDIALS function returns an integer value so check if
 *            retval < 0
 *   opt == 2 means function allocates memory so check if returned
 *            NULL pointer
 */

static int check_retval(void* returnvalue, const char* funcname, int opt)
{
  int* retval;

  /* Check if SUNDIALS function returned NULL pointer - no memory allocated */
  if (opt == 0 && returnvalue == NULL)
  {
    fprintf(stderr, "\nSUNDIALS_ERROR: %s() failed - returned NULL pointer\n\n",
            funcname);
    return (1);
  }

  /* Check if retval < 0 */
  else if (opt == 1)
  {
    retval = (int*)returnvalue;
    if (*retval < 0)
    {
      fprintf(stderr, "\nSUNDIALS_ERROR: %s() failed with retval = %d\n\n",
              funcname, *retval);
      return (1);
    }
  }

  /* Check if function returned NULL pointer - no memory allocated */
  else if (opt == 2 && returnvalue == NULL)
  {
    fprintf(stderr, "\nMEMORY_ERROR: %s() failed - returned NULL pointer\n\n",
            funcname);
    return (1);
  }

  return (0);
}
