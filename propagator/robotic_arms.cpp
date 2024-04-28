#include <iostream>
#include <fstream>

/* Sundials */
#include <cvode/cvode.h>
#include <nvector/nvector_serial.h>
#include <sunlinsol/sunlinsol_dense.h>
#include <sunmatrix/sunmatrix_dense.h>

/* Local */
#include "robotic_arms.h"
#include "mat.h"
#include "propagator.h"

using namespace std;
using namespace SymEngine;

/* Functions Called by the Solver */
int initiate_manipulator(SUNContext sunctx, N_Vector y, void* user_data);

int f_manipulator(sunrealtype t, N_Vector y, N_Vector ydot, void* user_data);

N_Vector end_effector_position(N_Vector y, SUNContext sunctx, void* user_data);

N_Vector end_effector_pose(N_Vector y, SUNContext sunctx, void* user_data);

SUNMatrix get_joint_matrix(int joint_number, N_Vector y, SUNContext sunctx, void* user_data);

SUNMatrix get_transformation_matrix(int final_joint_number, N_Vector y, SUNContext sunctx, void* user_data);

SUNMatrix mat_mul(SUNMatrix A, SUNMatrix B, SUNContext sunctx);


int initiate_manipulator(SUNContext sunctx, N_Vector y, void* user_data) {
    // Save joints
    // save_joints();

    // Save symbols and linear solver
    UserData data = (UserData)user_data;

    // Initialize y_manip
    for (size_t i = 0; i < NEQ_MANIP; i++)
    {
    Ith(y, INIT_SLICE_MANIP + i) = data->y0[INIT_SLICE_MANIP + i];
    }

    // Initialize tau (temporary) 
    N_Vector tau = N_VNew_Serial(NEQ_MANIP/2, sunctx);

    Ith(tau, 0)  = 0.0;
    Ith(tau, 1)  = 0.0;
    Ith(tau, 2)  = 0.0;
    Ith(tau, 3)  = 0.0;
    Ith(tau, 4)  = 0.0;
    Ith(tau, 5)  = 0.0;
    data->tau = tau;

    // Init end effector position and pose
    // Extract end effector location and save it to additional values
    N_Vector location = end_effector_position(y, sunctx, user_data);
    for (size_t i = 0; i < 3; i++)
    {
        Ith(data->additional, EE_LOC_INIT_SLICE + i) = Ith(location, i);
    }
    
    // Extract end effector pose and save it to additional values
    N_Vector moment = end_effector_pose(y, sunctx, user_data);
    for (size_t i = 0; i < 3; i++)
    {
        Ith(data->additional, EE_POS_INIT_SLICE + i) = Ith(moment, i);
    }

    return 0;
}

/* Integration functions */
int f_manipulator(sunrealtype t, N_Vector y, N_Vector ydot, void* user_data)
{
    SUNContext* sunctx;
    SUNLinearSolver LS;
    N_Vector tau;
    UserData data;
    
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
    Dv = D(Dv, *sunctx, y_m, data->scale);
    Cv = C(Cv, *sunctx, y_m, data->scale);

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
    
    // Perform the operation - [A=Dv] \ [b = (Cv * y(n+1:end))] -> [Ax = b]
    LS = SUNLinSol_Dense(ddq, Dv, *sunctx);
    SUNLinSolSetup(LS, Dv);
    SUNLinSolSolve(LS, Dv, ddq, ddq, 0.0);

    // Perform the operation [A=Dv] \ [b = tau] -> [Ax = b]
    LS = SUNLinSol_Dense(tau, Dv, *sunctx);
    SUNLinSolSetup(LS, Dv);
    SUNLinSolSolve(LS, Dv, tmp, tau, 0.0);

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

N_Vector end_effector_position(N_Vector y, SUNContext sunctx, void* user_data) {
    // Get end effector transformation matrix
    SUNMatrix T = get_transformation_matrix(NEQ_MANIP/2, y, sunctx, user_data);

    // Extract position
    N_Vector loc = N_VNew_Serial(3, sunctx);

    // Set position considering offsets
    UserData data = (UserData)user_data;
    Ith(loc, 0) = IJth(T, 0, 3) + data->base_to_body_x;
    Ith(loc, 1) = IJth(T, 1, 3) + data->base_to_body_y;
    Ith(loc, 2) = IJth(T, 2, 3) + data->base_to_body_z;
    
    // Free up memory
    SUNMatDestroy(T);

    return loc;
}

N_Vector end_effector_pose(N_Vector y, SUNContext sunctx, void* user_data) {
    // Get end effector transformation matrix
    SUNMatrix T = get_transformation_matrix(NEQ_MANIP/2, y, sunctx, user_data);

    // Extract pose
    N_Vector pos = N_VNew_Serial(3, sunctx);
    for (size_t i = 0; i < 3; i++)
    {
        Ith(pos, i) = IJth(T, i, 2);
    }

    // Normalize pose
    sunrealtype norm = sqrt(N_VDotProd(pos, pos));
    N_VScale(1.0 / norm, pos, pos);
    
    // Free up memory
    SUNMatDestroy(T);

    return pos;
}

SUNMatrix get_joint_matrix(int joint_number, N_Vector y, SUNContext sunctx, void* user_data) {
    // Load parameters
    UserData data = (UserData)user_data;
    vector<double> theta(6, 0.0); for (size_t i = 0; i < 6; i++) { theta[i] = Ith(y, INIT_SLICE_MANIP + i); }
    vector<double> a     = data->dh_a;
    vector<double> d     = data->dh_d;
    vector<double> alpha = data->dh_alpha;

    // Init matrix
    SUNMatrix Ti = SUNDenseMatrix(4, 4, sunctx);
    
    // Set matrix
    // Row 0
    IJth(Ti, 0, 0) = cos(theta[joint_number]);
    IJth(Ti, 0, 1) = -sin(theta[joint_number]) * cos(alpha[joint_number]);
    IJth(Ti, 0, 2) = sin(theta[joint_number]) * sin(alpha[joint_number]);
    IJth(Ti, 0, 3) = a[joint_number] * cos(theta[joint_number]);
    
    // Row 1
    IJth(Ti, 1, 0) = sin(theta[joint_number]);
    IJth(Ti, 1, 1) = cos(theta[joint_number]) * cos(alpha[joint_number]);
    IJth(Ti, 1, 2) = -cos(theta[joint_number]) * sin(alpha[joint_number]);
    IJth(Ti, 1, 3) = a[joint_number] * sin(theta[joint_number]);

    // Row 2
    IJth(Ti, 2, 0) = 0;
    IJth(Ti, 2, 1) = sin(alpha[joint_number]);
    IJth(Ti, 2, 2) = cos(alpha[joint_number]);
    IJth(Ti, 2, 3) = d[joint_number];

    // Row 3
    IJth(Ti, 3, 0) = 0;
    IJth(Ti, 3, 1) = 0;
    IJth(Ti, 3, 2) = 0;
    IJth(Ti, 3, 3) = 1;

    return Ti;
}


SUNMatrix get_transformation_matrix(int final_joint_number, N_Vector y, SUNContext sunctx, void* user_data) {
    // Loop to get i-th transformation matrix
    SUNMatrix T0i = SUNDenseMatrix(4, 4, sunctx);

    for (int i = 0; i < final_joint_number; i++)
    {
        // Get joint matrix
        SUNMatrix Ti = get_joint_matrix(i, y, sunctx, user_data);

        // Update transformation matrix
        if (i > 0)
        {
            // Compute transformation matrix
            T0i = mat_mul(T0i, Ti, sunctx);

        } else
        {   
            // First matrix
            SUNMatCopy(Ti, T0i);
        }
        
        // Free up memory
        SUNMatDestroy(Ti);
    }

    return T0i;
}

SUNMatrix mat_mul(SUNMatrix A, SUNMatrix B, SUNContext sunctx) {
    // Get the sizes of the matrices
    int m = SM_ROWS_D(A);
    int n = SM_COLUMNS_D(A);
    int p = SM_COLUMNS_D(B);

    // Create a dense matrix to store the result of the multiplication
    SUNMatrix C = SUNDenseMatrix(m, p, sunctx);

    // Perform matrix-matrix multiplication: C = A * B
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < p; j++) {
            sunrealtype sum = 0.0;
            for (int k = 0; k < n; k++) {
                sum += IJth(A, i, k) * IJth(B, k, j);
            }
            IJth(C, i, j) = sum;
        }
    }

    return C;
}
