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
#define T2   SUN_RCONST(0.7)
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

#define RTOL  SUN_RCONST(1.0e-4)
#define ATOL  SUN_RCONST(1.0e-8)

#define INIT_TIMET0  SUN_RCONST(0.0)
#define END_TIME     SUN_RCONST(20.0)

using namespace std;
using namespace SymEngine;
using namespace rapidjson;

/* Functions Called by the Solver */
int initiate_manipulator(SUNContext sunctx, N_Vector y, const int y_init_slice, void* user_data);

int f_manipulator(sunrealtype t, N_Vector y, N_Vector ydot, void* user_data);


int initiate_manipulator(SUNContext sunctx, N_Vector y, const int y_init_slice, void* user_data) {
    // Save symbols and linear solver
    UserData data  = (UserData)user_data;

    /* Initialize y_manip */
    Ith(y, y_init_slice + 0)  = T1;
    Ith(y, y_init_slice + 1)  = T2;
    Ith(y, y_init_slice + 2)  = T3;
    Ith(y, y_init_slice + 3)  = T4;
    Ith(y, y_init_slice + 4)  = T5;
    Ith(y, y_init_slice + 5)  = T6;
    Ith(y, y_init_slice + 6)  = DT1;
    Ith(y, y_init_slice + 7)  = DT2;
    Ith(y, y_init_slice + 8)  = DT3;
    Ith(y, y_init_slice + 9)  = DT4;
    Ith(y, y_init_slice + 10) = DT5;
    Ith(y, y_init_slice + 11) = DT6;

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

    // Init matrices
    SUNMatrix Dv = SUNDenseMatrix(NEQ_MANIP/2, NEQ_MANIP/2, *sunctx);
    SUNMatrix Cv = SUNDenseMatrix(NEQ_MANIP/2, NEQ_MANIP/2, *sunctx);
    
    /* Evaluate matrices */
    Dv = D(Dv, *sunctx, y);

    /*cout << "Matrix D: \n" << endl;
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

    Cv = C(Cv, *sunctx, y);

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
    for (size_t i = NEQ_MANIP/2; i < NEQ_MANIP; i++) { Ith(y_s, i - NEQ_MANIP/2) = Ith(y, i); }

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

    /* for (size_t i = 0; i < 6; i++)
    {
        cout << Ith(ddq, i) << endl;
    }*/

    /* Equations of motion */
    // dq
    Ith(ydot, 0) = Ith(y, 6);
    Ith(ydot, 1) = Ith(y, 7);
    Ith(ydot, 2) = Ith(y, 8);
    Ith(ydot, 3) = Ith(y, 9);
    Ith(ydot, 4) = Ith(y, 10);
    Ith(ydot, 5) = Ith(y, 11);

    // ddq
    Ith(ydot, 6)  = Ith(ddq, 0);
    Ith(ydot, 7)  = Ith(ddq, 1);
    Ith(ydot, 8)  = Ith(ddq, 2);
    Ith(ydot, 9)  = Ith(ddq, 3);
    Ith(ydot, 10) = Ith(ddq, 4);
    Ith(ydot, 11) = Ith(ddq, 5);

    // Clean up
    SUNLinSolFree(LS);
    SUNMatDestroy(Dv);
    SUNMatDestroy(Cv);
    N_VDestroy_Serial(y_s);
    N_VDestroy_Serial(ddq);
    N_VDestroy_Serial(tmp);

    return (0);
}
