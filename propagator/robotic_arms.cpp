#include <iostream>
#include <fstream>

/* Sundials */
#include <cvode/cvode.h>
#include <nvector/nvector_serial.h>
#include <sunlinsol/sunlinsol_dense.h>
#include <sunmatrix/sunmatrix_dense.h>

/* SymEngine */
#include <symengine/symengine_config.h>
#include <symengine/parser.h>
#include <symengine/expression.h>
#include <symengine/symbol.h>
#include <symengine/symengine_casts.h>
#include <symengine/matrix.h>

/* RapidJSON */
#include "rapidjson/document.h"

/* Local */
#include "robotic_arms.h"
#include "mat.h"
#include "propagator.h"

/* Problem Constants */
#define T1   SUN_RCONST(0.0)
#define T2   SUN_RCONST(-0.7)
#define T3   SUN_RCONST(0.0)
#define T4   SUN_RCONST(0.0)
#define T5   SUN_RCONST(0.0)
#define T6   SUN_RCONST(0.0)
#define DT1  SUN_RCONST(0.1)
#define DT2  SUN_RCONST(0.0)
#define DT3  SUN_RCONST(0.0)
#define DT4  SUN_RCONST(0.0)
#define DT5  SUN_RCONST(0.0)
#define DT6  SUN_RCONST(0.0)

using namespace std;
using namespace SymEngine;
using namespace rapidjson;

/* Functions Called by the Solver */
int initiate_manipulator(SUNContext sunctx, N_Vector y, void* user_data);

int f_manipulator(sunrealtype t, N_Vector y, N_Vector ydot, void* user_data);


int initiate_manipulator(SUNContext sunctx, N_Vector y, void* user_data) {
    // Save symbols and linear solver
    UserData data  = (UserData)user_data;

    /* Initialize y_manip */
    Ith(y, INIT_SLICE_MANIP + 0)  = T1;
    Ith(y, INIT_SLICE_MANIP + 1)  = T2;
    Ith(y, INIT_SLICE_MANIP + 2)  = T3;
    Ith(y, INIT_SLICE_MANIP + 3)  = T4;
    Ith(y, INIT_SLICE_MANIP + 4)  = T5;
    Ith(y, INIT_SLICE_MANIP + 5)  = T6;
    Ith(y, INIT_SLICE_MANIP + 6)  = DT1;
    Ith(y, INIT_SLICE_MANIP + 7)  = DT2;
    Ith(y, INIT_SLICE_MANIP + 8)  = DT3;
    Ith(y, INIT_SLICE_MANIP + 9)  = DT4;
    Ith(y, INIT_SLICE_MANIP + 10) = DT5;
    Ith(y, INIT_SLICE_MANIP + 11) = DT6;

    /* Initialize tau (temporary) */
    N_Vector tau = N_VNew_Serial(NEQ_MANIP/2, sunctx);

    Ith(tau, 0)  = 0.0;
    Ith(tau, 1)  = 0.0;
    Ith(tau, 2)  = 0.0;
    Ith(tau, 3)  = 0.0;
    Ith(tau, 4)  = 0.0;
    Ith(tau, 5)  = 0.0;
    data->tau = tau;

    return 0;
}

/* Integration functions */
int f_manipulator(sunrealtype t, N_Vector y, N_Vector ydot, void* user_data)
{
    SUNContext* sunctx;
    SUNLinearSolver LS;
    N_Vector tau;
    UserData data;
    int retval;

    // Retrieve user data
    data = (UserData)user_data;
    sunctx = data->sunctx;
    tau = data->tau;

    // Extract from general y
    N_Vector y_m = N_VNew_Serial(NEQ_MANIP, *sunctx);
    for (size_t i = 0; i < NEQ_MANIP; i++) { Ith(y_m, i) = Ith(y, INIT_SLICE_MANIP + i); }
    

    // Init matrices
    SUNMatrix Dv = SUNDenseMatrix(NEQ_MANIP/2, NEQ_MANIP/2, *sunctx);
    SUNMatrix Cv = SUNDenseMatrix(NEQ_MANIP/2, NEQ_MANIP/2, *sunctx);
    
    /* Evaluate matrices */
    Dv = D(Dv, *sunctx, y_m);

    /* cout << "Matrix D: \n" << endl;
    // Iterate over each row
    for (sunindextype i = 0; i < 6; ++i) {
        // Iterate over each column
        for (sunindextype j = 0; j < 6; ++j) {
            // Retrieve the matrix element at row i, column j
            sunrealtype value;
            value = IJth(Dv, i, j);
            // Print the matrix element
            cout << value << "\t";
        }
        // Move to the next row
        cout << endl;
    }*/

    Cv = C(Cv, *sunctx, y_m);

    /*cout << "Matrix C:\n" << endl;
    // Iterate over each row
    for (sunindextype i = 0; i < 6; ++i) {
        // Iterate over each column
        for (sunindextype j = 0; j < 6; ++j) {
            // Retrieve the matrix element at row i, column j
            sunrealtype value;
            value = IJth(Cv, i, j);
            // Print the matrix element
            cout << value << "\t";
        }
        // Move to the next row
        cout << endl;
    }*/

    /* Computations
    EoM:
        dq  = y(n+1:end)
        ddq = -Dv\(Cv*y(n+1:end)) + Dv\tau;
    */

   // Init vectors
    N_Vector y_s = N_VNew_Serial(NEQ_MANIP/2, *sunctx);
    N_Vector ddq = N_VNew_Serial(NEQ_MANIP/2, *sunctx); 
    N_Vector tmp = N_VNew_Serial(NEQ_MANIP/2, *sunctx); 

    // Extract slice of y
    for (size_t i = NEQ_MANIP/2; i < NEQ_MANIP; i++) { Ith(y_s, i - NEQ_MANIP/2) = Ith(y_m, i); }

    // Perform the operation Cv * y(n+1:end)
    SUNMatMatvec(Cv, y_s, ddq);
    /*for (size_t i = 0; i < 6; i++)
    {
        cout << Ith(y_s, i) << " " << Ith(ddq, i)<< endl;
    }
    cout << "\n" << endl;*/
    
    // Perform the operation - [A=Dv] \ [b = (Cv * y(n+1:end))] -> [Ax = b]
    LS = SUNLinSol_Dense(ddq, Dv, *sunctx);
    retval = SUNLinSolSetup(LS, Dv);
    retval = SUNLinSolSolve(LS, Dv, ddq, ddq, 0.0);

    // Perform the operation [A=Dv] \ [b = tau] -> [Ax = b]
    LS = SUNLinSol_Dense(tau, Dv, *sunctx);
    retval = SUNLinSolSetup(LS, Dv);
    retval = SUNLinSolSolve(LS, Dv, tmp, tau, 0.0);

    /*for (size_t i = 0; i < 6; i++)
    {
        cout << Ith(ddq, i) << " " << Ith(tmp, i) << endl;
    }
    cout << "\n" << endl;*/

    // Perform summation
    N_VLinearSum(-1, ddq, 1, tmp, ddq);

    /* Equations of motion */
    // dq
    Ith(ydot, INIT_SLICE_MANIP + 0) = Ith(y, INIT_SLICE_MANIP + 6);
    Ith(ydot, INIT_SLICE_MANIP + 1) = Ith(y, INIT_SLICE_MANIP + 7);
    Ith(ydot, INIT_SLICE_MANIP + 2) = Ith(y, INIT_SLICE_MANIP + 8);
    Ith(ydot, INIT_SLICE_MANIP + 3) = Ith(y, INIT_SLICE_MANIP + 9);
    Ith(ydot, INIT_SLICE_MANIP + 4) = Ith(y, INIT_SLICE_MANIP + 10);
    Ith(ydot, INIT_SLICE_MANIP + 5) = Ith(y, INIT_SLICE_MANIP + 11);

    // ddq
    Ith(ydot, INIT_SLICE_MANIP + 6)  = Ith(ddq, INIT_SLICE_MANIP + 0);
    Ith(ydot, INIT_SLICE_MANIP + 7)  = Ith(ddq, INIT_SLICE_MANIP + 1);
    Ith(ydot, INIT_SLICE_MANIP + 8)  = Ith(ddq, INIT_SLICE_MANIP + 2);
    Ith(ydot, INIT_SLICE_MANIP + 9)  = Ith(ddq, INIT_SLICE_MANIP + 3);
    Ith(ydot, INIT_SLICE_MANIP + 10) = Ith(ddq, INIT_SLICE_MANIP + 4);
    Ith(ydot, INIT_SLICE_MANIP + 11) = Ith(ddq, INIT_SLICE_MANIP + 5);

    // Clean up
    SUNLinSolFree(LS);
    SUNMatDestroy(Dv);
    SUNMatDestroy(Cv);
    N_VDestroy_Serial(y_s);
    N_VDestroy_Serial(y_m);
    N_VDestroy_Serial(ddq);
    N_VDestroy_Serial(tmp);

    return (0);
}