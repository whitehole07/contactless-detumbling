#include <stdio.h>
#include <iostream>

/* Sundials */
#include <cvode/cvode.h>
#include <nvector/nvector_serial.h>
#include <sunlinsol/sunlinsol_dense.h>
#include <sunmatrix/sunmatrix_dense.h>

/* Local */
#include "propagator.h"
#include "attitude.h"
#include "robotic_arms.h"

// Solver settings
#define RTOL    SUN_RCONST(1.0e-4)      /* scalar relative tolerance            */
#define ATOL    SUN_RCONST(1.0e-8)      /* vector absolute tolerance components */

// Time settings
#define T0      SUN_RCONST(0.0)         /* Initial time */
#define T1      SUN_RCONST(200)         /* Final time */
#define TSTEP   0.555555555555556       /* Time step */

/* Functions Called by the Solver */
static int f(sunrealtype t, N_Vector y, N_Vector ydot, void* user_data);

/* Private functions to output results */
static void save_to_CSV(FILE* csv_file, sunrealtype t, N_Vector y, bool init);

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
    SUNMatrix A;
    UserData user_data;
    SUNLinearSolver LS;
    void* cvode_mem;
    int retval, iout;

    sunrealtype t_stop = T1;

    /* Create the SUNDIALS context */
    retval = SUNContext_Create(SUN_COMM_NULL, &sunctx);
    if (check_retval(&retval, "SUNContext_Create", 1)) { return (1); }

    /* Set the pointer to user-defined data */
    user_data = (UserData)malloc(sizeof *user_data); /* Allocate data memory */
    user_data->sunctx = &sunctx;

    /* Initial conditions */
    y = N_VNew_Serial(NEQ, sunctx);
    if (check_retval((void*)y, "N_VNew_Serial", 0)) { return (1); }

    /* Initiate external propagators */
    initiate_manipulator(sunctx, y, user_data);
    initiate_attitude(sunctx, y, user_data);

    /* Set the vector absolute tolerance */
    abstol = N_VNew_Serial(NEQ, sunctx);
    if (check_retval((void*)abstol, "N_VNew_Serial", 0)) { return (1); }

    for (size_t i = 0; i < NEQ; i++) { Ith(abstol, i) = ATOL; }

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

      // Increase the maximum number of steps
    long int max_num_steps = 1000; // Set the desired maximum number of steps
    retval = CVodeSetMaxNumSteps(cvode_mem, max_num_steps);
    if (check_retval(&retval, "CVodeSetMaxNumSteps", 1)) { return (1); }

    retval = CVodeSetUserData(cvode_mem, user_data);
    if (check_retval(&retval, "CVodeSetUserData", 1)) { return (1); }

    // Attach linear solver
    A = SUNDenseMatrix(NEQ, NEQ, sunctx);
    LS = SUNLinSol_Dense(y, A, sunctx);

    retval = CVodeSetLinearSolver(cvode_mem, LS, A);
    if (check_retval(&retval, "CVodeSetLinearSolver", 1)) { return 1; }
    
    /* Loop */
    printf(" \nPropagation\n\n");

    // Open CSV file for writing and save first row
    FILE *csv_file = fopen("prop_result.csv", "w");
    save_to_CSV(csv_file, t, y, true);

    iout = 0;
    tout = T0 + TSTEP;
    while (t < T1) {
    retval = CVode(cvode_mem, tout, y, &t, CV_NORMAL);
    PrintOutput(iout, t, y);

    if (check_retval(&retval, "CVode", 1)) { break; }
    if (retval == CV_SUCCESS)
    {
        iout++;
        tout += TSTEP;
    }

    // Save row
    save_to_CSV(csv_file, t, y, false);

    }
    // Close CSV file
    fclose(csv_file);

    /* Print final statistics to the screen */
    printf("\nFinal Statistics:\n");
    retval = CVodePrintAllStats(cvode_mem, stdout, SUN_OUTPUTFORMAT_TABLE);

    /* Free memory */
    N_VDestroy(y);            /* Free y vector */
    N_VDestroy(abstol);       /* Free abstol vector */
    SUNLinSolFree(LS);
    CVodeFree(&cvode_mem);    /* Free CVODE memory */
    SUNMatDestroy(A);
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

static int f(sunrealtype t, N_Vector y, N_Vector ydot, void* user_data) {

  // Robotic Arm
  f_manipulator(t, y, ydot, user_data);

  // Attitude
  f_attitude(t, y, ydot, user_data);

  return (0);
}

/*
 *-------------------------------
 * Private helper functions
 *-------------------------------
 */

static void save_to_CSV(FILE* csv_file, sunrealtype t, N_Vector y, bool init) {

    if (init) {
      // Write header row
      fprintf(csv_file, "Time,");
      for (int i = 0; i < NEQ; i++) {
          fprintf(csv_file, "Component %d,", i + 1);
      }
      fprintf(csv_file, "\n");
    }

    fprintf(csv_file, "%f,", t); // Write time value
    for (int i = 0; i < NEQ; i++) {
            fprintf(csv_file, "%e,", Ith(y, i)); // Write each component
    }
    fprintf(csv_file, "\n"); // Write newline


  return;
}

static void PrintOutput(int i, sunrealtype t, N_Vector y)
{
  printf("At (%d) t = %0.4e    y =%14.6e  %14.6e  %14.6e  %14.6e  %14.6e  %14.6e  %14.6e\n", i, t, \
  Ith(y, 0), Ith(y, 1), Ith(y, 2), Ith(y, 3), Ith(y, 4), Ith(y, 5), Ith(y, 6));

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
