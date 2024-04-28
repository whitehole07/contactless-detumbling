#include <stdio.h>
#include <iostream>

/* PyBind11 */
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

/* Sundials */
#include <cvode/cvode.h>
#include <nvector/nvector_serial.h>
#include <sunlinsol/sunlinsol_dense.h>
#include <sunmatrix/sunmatrix_dense.h>

/* Local */
#include "propagator.h"
#include "attitude.h"
#include "robotic_arms.h"

namespace py = pybind11;

// Solver settings
#define RTOL    SUN_RCONST(1.0e-4)      /* scalar relative tolerance            */
#define ATOL    SUN_RCONST(1.0e-8)      /* vector absolute tolerance components */

// Time settings
#define TI      SUN_RCONST(0.0)         /* Initial time */
#define TF      SUN_RCONST(2000)        /* Final time */
#define TSTEP   1                       /* Time step */

/* Debris Properties */
#define DEBRIS_MASS     950.0        // [kg]
#define DEBRIS_HEIGHT   5.0          // [m]
#define DEBRIS_RADIUS   2.5          // [m]
#define DEBRIS_SIGMA    35000000.0  
#define DEBRIS_THICK    0.1

#define DEBRIS_IXX   (0.083333333333333) * DEBRIS_MASS * (pow(DEBRIS_HEIGHT, 2) + 3 * pow(DEBRIS_RADIUS, 2))   // SUN_RCONST(3463.542)
#define DEBRIS_IYY   (0.083333333333333) * DEBRIS_MASS * (pow(DEBRIS_HEIGHT, 2) + 3 * pow(DEBRIS_RADIUS, 2))   // SUN_RCONST(3463.542)
#define DEBRIS_IZZ   (0.500000000000000) * DEBRIS_MASS * (                            pow(DEBRIS_RADIUS, 2))   // SUN_RCONST(2968.75)

/* Eddy Current Torque */
#define MAG_N_TURNS  500.0
#define MAG_RADIUS   1.0
#define MAG_CURRENT  50.0

/* Global Variables: Robot's Base Frame to Target's Body Frame */
#define ORIGIN_XDISTANCE_TO_DEBRIS  10.0  // [m]
#define ORIGIN_YDISTANCE_TO_DEBRIS  0.0   // [m]
#define ORIGIN_ZDISTANCE_TO_DEBRIS  5.0   // [m]

/* Denavit-Hartenberg Parameters */
#define SCALE       10
#define DH_A        {           0, SCALE*-0.6127, SCALE*-0.57155,             0,             0,             0}
#define DH_D        {SCALE*0.1807,             0,              0, SCALE*0.17415, SCALE*0.11985, SCALE*0.11655}
#define DH_ALPHA    {        PI/2,             0,              0,          PI/2,         -PI/2,             0}

/* Functions Called by the Solver */
static int f(sunrealtype t, N_Vector y, N_Vector ydot, void* user_data);

static void PrintOutput(int i, sunrealtype t, N_Vector y);

/* Private function to check function return values */
static int check_retval(void* returnvalue, const char* funcname, int opt);

/*
 *-------------------------------
 * Main Program
 *-------------------------------
 */

Environment::Environment(vector<double> y0, double debris_Ixx, double debris_Iyy, double debris_Izz, 
                double debris_radius, double debris_height, double debris_thick, double debris_sigma,
                 double mag_n_turns, double mag_current, double mag_radius, double base_to_body_x,
                  double base_to_body_y, double base_to_body_z, double scale, vector<double> dh_a,
                   vector<double> dh_d, vector<double> dh_alpha) {
    // Save problem values
    user_data = (UserData)malloc(sizeof *user_data); /* Allocate data memory */
    user_data->y0 = y0;     
    user_data->debris_Ixx = debris_Ixx;
    user_data->debris_Iyy = debris_Iyy;
    user_data->debris_Izz = debris_Izz;
    user_data->debris_radius = debris_radius;
    user_data->debris_height = debris_height;
    user_data->debris_thick = debris_thick;
    user_data->debris_sigma = debris_sigma;
    user_data->mag_n_turns = mag_n_turns;
    user_data->mag_current = mag_current;
    user_data->mag_radius = mag_radius;
    user_data->base_to_body_x = base_to_body_x;
    user_data->base_to_body_y = base_to_body_y;
    user_data->base_to_body_z = base_to_body_z;
    user_data->scale = scale;
    user_data->dh_a = dh_a;
    user_data->dh_d = dh_d;
    user_data->dh_alpha = dh_alpha;

    // Initialize environment 
    initialize();
}

void Environment::initialize() {
    /* Create the SUNDIALS context */
    SUNContext_Create(SUN_COMM_NULL, &sunctx);

    /* Set the pointer to user-defined data */
    user_data->sunctx = &sunctx;
    user_data->additional = N_VNew_Serial(ADDITIONAL_SIZE, sunctx);

    /* Initial conditions */
    y = N_VNew_Serial(NEQ, sunctx);

    /* Initiate external propagators */
    initiate_manipulator(sunctx, y, user_data);
    initiate_attitude(sunctx, y, user_data);

    /* Set the vector absolute tolerance */
    abstol = N_VNew_Serial(NEQ, sunctx);
    for (size_t i = 0; i < NEQ; i++) { Ith(abstol, i) = ATOL; }

    /* Call CVodeCreate to create the solver memory and specify the
    * Backward Differentiation Formula */
    cvode_mem = CVodeCreate(CV_ADAMS, sunctx);

    /* Call CVodeInit to initialize the integrator memory and specify the
    * user's right hand side function in y'=f(t,y), the initial time TI, and
    * the initial dependent variable vector y. */
    CVodeInit(cvode_mem, f, TI, y);

    /* Call CVodeSVtolerances to specify the scalar relative tolerance
    * and vector absolute tolerances */
    CVodeSVtolerances(cvode_mem, RTOL, abstol);

    // Increase the maximum number of steps
    long int max_num_steps = 1000; // Set the desired maximum number of steps
    CVodeSetMaxNumSteps(cvode_mem, max_num_steps);

    CVodeSetUserData(cvode_mem, user_data);

    // Attach linear solver
    A = SUNDenseMatrix(NEQ, NEQ, sunctx);
    LS = SUNLinSol_Dense(y, A, sunctx);

    CVodeSetLinearSolver(cvode_mem, LS, A);

    iout = 0;
    tout = 0;
}

// Method to reset the environment
void Environment::reset() {
    // Reset the environment to its initial state
    /* Free memory */
    N_VDestroy(y);            /* Free y vector */
    N_VDestroy(abstol);       /* Free abstol vector */
    SUNLinSolFree(LS);
    CVodeFree(&cvode_mem);    /* Free CVODE memory */
    SUNMatDestroy(A);
    SUNContext_Free(&sunctx); /* Free the SUNDIALS context */
    free(user_data);

    initialize();
}

tuple<double, vector<double>> Environment::current_state() {
    // Convert N_vector into vector
    vector<double> state_vector(NV_LENGTH_S(y) + NV_LENGTH_S(user_data->additional));
    for (sunindextype i = 0; i < NV_LENGTH_S(y); ++i) {
        state_vector[i] = Ith(y, i);
    }
    for (sunindextype i = 0; i < NV_LENGTH_S(user_data->additional); ++i) {
        state_vector[NV_LENGTH_S(y) + i] = Ith(user_data->additional, i);
    }

    // Time
    double return_time = iout == 0 ? 0 : t;
    return make_tuple(return_time, state_vector);
}

// Method to execute one step in the environment
tuple<double, vector<double>> Environment::step(double t_step, vector<double>action) {
    // Set next action
    for (size_t i = 0; i < NEQ_MANIP/2; i++)
    {
      Ith(user_data->tau, i) = action[i];
    }

    // Execute one step in the environment
    tout += t_step;
    int retval = CVode(cvode_mem, tout, y, &t, CV_NORMAL);
    PrintOutput(iout, t, y);

    if (check_retval(&retval, "CVode", 1)) {
      }
    if (retval == CV_SUCCESS)
    {
        iout++;
    }

    return current_state();
}


/*int main(void)
{
    printf(" \nPropagation\n\n");

    // Create a vector<double> y0 with NEQ elements initialized to 0.0
    vector<double> y0(NEQ, 0.0f);

    // Set the first 12 elements in the vector y0
    y0[0] = 0.0f;
    y0[1] = -0.7f;
    y0[2] = -0.3f;
    y0[3] = 0.0f;
    y0[4] = 0.0f;
    y0[5] = 0.0f;
    y0[6] = -0.02f;
    y0[7] = 0.0f;
    y0[8] = 0.0f;
    y0[9] = 0.0f;
    y0[10] = 0.0f;
    y0[11] = 0.0f;

    // Set the last 7 elements in the vector y0 with the new values
    y0[12] = 0.1f;
    y0[13] = 0.2f;
    y0[14] = 0.0f;
    y0[15] = 0.0f;
    y0[16] = 0.0f;
    y0[17] = 0.0f;
    y0[18] = 1.0f;

    Environment test(y0, DEBRIS_IXX, DEBRIS_IYY, DEBRIS_IZZ, DEBRIS_RADIUS, DEBRIS_HEIGHT, 
    DEBRIS_THICK, DEBRIS_SIGMA, MAG_N_TURNS, MAG_CURRENT, MAG_RADIUS,
     ORIGIN_XDISTANCE_TO_DEBRIS, ORIGIN_YDISTANCE_TO_DEBRIS, ORIGIN_ZDISTANCE_TO_DEBRIS, SCALE, DH_A, DH_D, DH_ALPHA);

    test.step(TSTEP, {0,0,0,0,0,0});
    test.step(TSTEP, {0,0,0,0,0,0});
    test.step(TSTEP, {0,0,0,0,0,0});
    test.reset();
    test.step(TSTEP, {0,0,0,0,0,0});

    return (0);
}*/


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


PYBIND11_MODULE(environment, m) {
    py::class_<Environment>(m, "Environment")
        .def(py::init<vector<double>, double, double, double, double, 
                      double, double, double, double, double, 
                      double, double, double, double, double, 
                      vector<double>, vector<double>, vector<double>>())
        .def("initialize", &Environment::initialize)
        .def("reset", &Environment::reset)
        .def("current_state", &Environment::current_state)
        .def("step", &Environment::step, py::arg("t_step"), py::arg("action"));
}
