#include <iostream>
#include <fstream>

/* Sundials */
#include <cvode/cvode.h>
#include <nvector/nvector_serial.h>
#include <sunlinsol/sunlinsol_dense.h>
#include <sunmatrix/sunmatrix_dense.h>

/* Eigen */
#include "Eigen/QR"

/* Local */
#include "robotic_arms.h"
#include "mat.h"
#include "propagator.h"

using namespace std;

/* Functions Called by the Solver */
int initiate_manipulator(SUNContext sunctx, N_Vector y, void* user_data);

int f_manipulator(sunrealtype t, N_Vector y, N_Vector ydot, void* user_data);

N_Vector end_effector_position(N_Vector y, SUNContext sunctx, void* user_data);

N_Vector end_effector_pose(N_Vector y, SUNContext sunctx, void* user_data);

SUNMatrix get_joint_matrix(int joint_number, N_Vector y, SUNContext sunctx, void* user_data);

SUNMatrix get_transformation_matrix(int final_joint_number, N_Vector y, SUNContext sunctx, void* user_data);

SUNMatrix mat_mul(SUNMatrix A, SUNMatrix B, SUNContext sunctx);

N_Vector solve_linear_system(SUNMatrix As, N_Vector bs, SUNContext sunctx);

void inv_dyn(N_Vector y, N_Vector tau, void* user_data);

N_Vector end_effector_linang_velocity(N_Vector y, SUNContext sunctx, void* user_data);

SUNMatrix compute_jacobian(N_Vector y, void* user_data);

void sub_mat(SUNMatrix mat, SUNMatrix sub_mat, vector<int> init_slice, vector<int> end_slice);

void sub_mat_to_vec(SUNMatrix mat, N_Vector sub_vec, int column, vector<int> slice);

N_Vector cross(SUNContext sunctx, N_Vector a, N_Vector b);

SUNMatrix transpose(SUNContext sunctx, SUNMatrix mat);

void rotm2axang(SUNMatrix R, double& angle, N_Vector axis);

SUNMatrix pseudo_inverse(SUNContext sunctx, SUNMatrix mat);


int initiate_manipulator(SUNContext sunctx, N_Vector y, void* user_data) {
    // Save joints
    // save_joints();

    // Save symbols and linear solver
    UserData data = (UserData)user_data;

    // Initialize Dv and Cv
    data->Dv = SUNDenseMatrix(NEQ_MANIP/2, NEQ_MANIP/2, sunctx);
    data->Cv = SUNDenseMatrix(NEQ_MANIP/2, NEQ_MANIP/2, sunctx);

    // Initialize y_manip
    for (size_t i = 0; i < NEQ_MANIP; i++)
    {
    Ith(y, INIT_SLICE_MANIP + i) = data->y0[INIT_SLICE_MANIP + i];
    }

    // Init torque
    for (size_t i = 0; i < NEQ_MANIP/2; i++)
    {
        Ith(data->additional, EE_TOR_INIT_SLICE + i) = 0.0;
    }

    return 0;
}

void inv_dyn(N_Vector y, N_Vector tau, void* user_data) {

    SUNContext* sunctx;
    vector<double> tau_max;
    N_Vector yD;
    UserData data;
    SUNMatrix Dv, Cv;    
    
    // Retrieve user data
    data = (UserData)user_data;
    sunctx = data->sunctx;
    tau_max = data->tau_max;
    yD = data->yD;
    Dv = data->Dv;
    Cv = data->Cv;

    // Evaluate matrices
    N_Vector y_m = N_VNew_Serial(NEQ_MANIP, *sunctx);
    for (size_t i = 0; i < NEQ_MANIP; i++) { Ith(y_m, i) = Ith(y, INIT_SLICE_MANIP + i); }

    // Init vectors
    N_Vector u_bar = N_VNew_Serial(NEQ_MANIP/2, *sunctx);

    // Extract from general y and yD
    N_Vector yv = N_VNew_Serial(NEQ_MANIP/2, *sunctx);
    N_Vector dyv = N_VNew_Serial(NEQ_MANIP/2, *sunctx);
    N_Vector yDv = N_VNew_Serial(NEQ_MANIP/2, *sunctx);
    N_Vector dyDv = N_VNew_Serial(NEQ_MANIP/2, *sunctx);
    for (size_t i = 0; i < NEQ_MANIP/2; i++) { 
        Ith(yv, i) = Ith(y_m, i);
        Ith(dyv, i) = Ith(y_m, NEQ_MANIP/2 + i);
        Ith(yDv, i) = Ith(yD, i);
        Ith(dyDv, i) = Ith(yD, NEQ_MANIP/2 + i);
    }
    
    // Compute max difference
    double max_diff = 0.0;
    for (size_t j = 0; j < NEQ_MANIP/2; j++) {
        double diff = abs(Ith(yD, j) - Ith(yv, j));

        // Check for maximum
        max_diff = (max_diff < diff) ? diff : max_diff;
    }

    // Compute u_bar
    for (size_t i = 0; i < NEQ_MANIP/2; i++)
    {
        // Compute gains
        double KP = tau_max[i]/max_diff;
        double KD = sqrt(2)*KP;

        // Compute u_bar
        Ith(u_bar, i) = KD*(Ith(dyDv, i) - Ith(dyv, i)) + KP*(Ith(yDv, i) - Ith(yv, i));
        // cout << KD << "*(" << Ith(dyDv, i) << " - " << Ith(dyv, i) << ") + " << KP << "*(" << Ith(yDv, i) << " - " << Ith(yv, i) << ") = " << Ith(u_bar, i) << endl;
    }
    
    // Compute torque
    N_Vector tmp = N_VNew_Serial(NEQ_MANIP/2, *sunctx);
    SUNMatMatvec(Dv, u_bar, tau);
    SUNMatMatvec(Cv, dyv, tmp);
    N_VLinearSum(1, tau, 1, tmp, tau);

    // Clean up
    N_VDestroy(tmp);
    N_VDestroy(u_bar);
    N_VDestroy(yv);
    N_VDestroy(y_m);
    N_VDestroy(dyv);
    N_VDestroy(yDv);
    N_VDestroy(dyDv);

    return;
}

N_Vector inv_kin(N_Vector q_init, SUNMatrix TD, double tol, int max_iter, void* user_data) {

    N_Vector q = q_init;
    SUNContext* sunctx;
    UserData data;

    // Retrieve user data
    data = (UserData)user_data;
    sunctx = data->sunctx;

    SUNMatrix TC = SUNDenseMatrix(4, 4, *sunctx);
    SUNMatrix J = SUNDenseMatrix(6, 6, *sunctx);
    SUNMatrix pinv_J = SUNDenseMatrix(6, 6, *sunctx);

    N_Vector dq = N_VNew_Serial(6, *sunctx);

    N_Vector trC = N_VNew_Serial(3, *sunctx);
    N_Vector trD = N_VNew_Serial(3, *sunctx);
    N_Vector tr_error = N_VNew_Serial(3, *sunctx);

    SUNMatrix RC = SUNDenseMatrix(3, 3, *sunctx);
    SUNMatrix RD = SUNDenseMatrix(3, 3, *sunctx);
    SUNMatrix R_error = SUNDenseMatrix(3, 3, *sunctx);

    double angle;
    N_Vector axis = N_VNew_Serial(3, *sunctx);

    N_Vector error = N_VNew_Serial(6, *sunctx);

    // Build y-like array
    N_Vector y = N_VNew_Serial(NEQ, *sunctx); for (size_t i = 0; i < NEQ; i++) { Ith(y, i) = 0.0; }    

    for (int iter = 0; iter < max_iter; iter++)
    {   
        // Build y-like array
        for (size_t i = INIT_SLICE_MANIP; i < INIT_SLICE_MANIP + NEQ_MANIP/2; i++)
        {
            Ith(y, i) = Ith(q, i - INIT_SLICE_MANIP);
        }   

        // Get current transformation matrix
        TC = get_transformation_matrix(NEQ_MANIP/2, y, *sunctx, user_data);

        // Compute error
        // Position error
        sub_mat_to_vec(TC, trC, 3, {0, 2});
        sub_mat_to_vec(TD, trD, 3, {0, 2});

        N_VLinearSum(1, trD, -1, trC, tr_error);

        // Orientation error
        sub_mat(TC, RC, {0, 2}, {0, 2});
        sub_mat(TD, RD, {0, 2}, {0, 2});

        R_error = mat_mul(transpose(*sunctx, RD), RC, *sunctx);

        rotm2axang(R_error, angle, axis);

        // Get error        
        Ith(error, 0) = Ith(tr_error, 0);
        Ith(error, 1) = Ith(tr_error, 1);
        Ith(error, 2) = Ith(tr_error, 2);
        Ith(error, 3) = angle * Ith(axis, 0);
        Ith(error, 4) = angle * Ith(axis, 1);
        Ith(error, 5) = angle * Ith(axis, 2);

        // Verify convergence
        if ( sqrt(N_VDotProd(error, error)) < tol )
        {
            break;
        }

        // Compute Jacobian
        J = compute_jacobian(y, user_data);

        // Compute pseudoinvers
        pinv_J = pseudo_inverse(*sunctx, J);

        // Find delta q
        SUNMatMatvec(pinv_J, error, dq);

        // Update q
        N_VLinearSum(1, q, 1, dq, q);
    }

    SUNMatDestroy(RC);
    SUNMatDestroy(RD);
    SUNMatDestroy(R_error);
    SUNMatDestroy(TC);
    SUNMatDestroy(J);
    SUNMatDestroy(pinv_J);
    N_VDestroy_Serial(dq);
    N_VDestroy_Serial(trC);
    N_VDestroy_Serial(trD);
    N_VDestroy_Serial(tr_error);
    N_VDestroy_Serial(axis);
    N_VDestroy_Serial(error);

    return q;
}

SUNMatrix compute_jacobian(N_Vector y, void* user_data) {

    SUNContext* sunctx;
    UserData data;
    vector<vector<double>> com_vec;

    // Retrieve user data
    data = (UserData)user_data;
    sunctx = data->sunctx;
    com_vec = data->com;

    SUNMatrix J = SUNDenseMatrix(6, 6, *sunctx);
    SUNMatrix T0j = SUNDenseMatrix(4, 4, *sunctx);
    SUNMatrix R = SUNDenseMatrix(3, 3, *sunctx);
    SUNMatrix com = SUNDenseMatrix(NEQ_MANIP/2, 3, *sunctx);

    N_Vector z_j = N_VNew_Serial(3, *sunctx);
    N_Vector diff = N_VNew_Serial(3, *sunctx);
    N_Vector tr = N_VNew_Serial(3, *sunctx);
    N_Vector tmp = N_VNew_Serial(3, *sunctx);
    N_Vector com_n = N_VNew_Serial(3, *sunctx);
    N_Vector pli = N_VNew_Serial(NEQ_MANIP/2, *sunctx);

    // Convert CoM to SUNMatrix
    for (size_t i = 0; i < NEQ_MANIP/2; i++) { for (size_t j = 0; j < 3; j++) { IJth(com, i, j) = com_vec[i][j]; } }

    // Get end effector transformation matrix
    SUNMatrix T0e = get_transformation_matrix(NEQ_MANIP/2, y, *sunctx, user_data);

    // Extract translation, rotation, and last com
    sub_mat(T0e, R, {0, 2}, {0, 2});
    sub_mat_to_vec(T0e, tr, 3, {0, 2});

    SUNMatrix com_transpose = transpose(*sunctx, com);
    sub_mat_to_vec(com_transpose, com_n, (NEQ_MANIP/2) - 1, {0, 2});

    // Extract relative CoM (tr + R*com_n)
    SUNMatMatvec(R, com_n, tmp);
    N_VLinearSum(1, tr, 1, tmp, pli); // CHECK TRANSF MATRICES 

    // Compute Jacobian
    for (size_t i = 0; i < 4; i++) {
        IJth(T0j, i, i) = 1.0;
    }

    for (int j = 0; j < NEQ_MANIP/2; j++)
    {
        // Update position jacobian
        sub_mat_to_vec(T0j, diff, 3, {0, 2});
        sub_mat_to_vec(T0j, z_j, 2, {0, 2});

        IJth(J, 0, j) = Ith(z_j, 1) * (Ith(pli, 2) - Ith(diff, 2)) - Ith(z_j, 2) * (Ith(pli, 1) - Ith(diff, 1));
        IJth(J, 1, j) = Ith(z_j, 2) * (Ith(pli, 0) - Ith(diff, 0)) - Ith(z_j, 0) * (Ith(pli, 2) - Ith(diff, 2));
        IJth(J, 2, j) = Ith(z_j, 0) * (Ith(pli, 1) - Ith(diff, 1)) - Ith(z_j, 1) * (Ith(pli, 0) - Ith(diff, 0));

        // Update orientation jacobian
        IJth(J, 3, j) = Ith(z_j, 0);
        IJth(J, 4, j) = Ith(z_j, 1);
        IJth(J, 5, j) = Ith(z_j, 2);

        // Get new transformation matrix
        SUNMatrix T = get_transformation_matrix(j+1, y, *sunctx, user_data);

        // Clean up
        SUNMatCopy(T, T0j);
        SUNMatDestroy(T);
    }

    SUNMatDestroy(com);
    SUNMatDestroy(T0e);
    SUNMatDestroy(R);
    SUNMatDestroy(T0j);
    SUNMatDestroy(com_transpose);
    N_VDestroy(pli);
    N_VDestroy(tmp);
    N_VDestroy(diff);
    N_VDestroy(z_j);
    N_VDestroy(tr);
    N_VDestroy(com_n);

    return J;
}

/* Integration functions */
int f_manipulator(sunrealtype t, N_Vector y, N_Vector ydot, void* user_data)
{
    SUNContext* sunctx;
    UserData data;
    N_Vector tau, y_s, ddq, tmp, y_m, sol1, sol2;

    // Retrieve user data
    data = (UserData)user_data;
    sunctx = data->sunctx;

    // Init vectors
    y_s = N_VNew_Serial(NEQ_MANIP/2, *sunctx);
    tau = N_VNew_Serial(NEQ_MANIP/2, *sunctx);
    ddq = N_VNew_Serial(NEQ_MANIP/2, *sunctx);
    sol2 = N_VNew_Serial(NEQ_MANIP/2, *sunctx);

    // Extract from general y
    y_m = N_VNew_Serial(NEQ_MANIP, *sunctx);
    for (size_t i = 0; i < NEQ_MANIP; i++) { Ith(y_m, i) = Ith(y, INIT_SLICE_MANIP + i); }
    
    // Compute matrices
    D(data->Dv, *sunctx, y_m);
    C(data->Cv, *sunctx, y_m);

    // Compute Torque Control (if required)
    if (data->control) {
        // Get vectors
        N_Vector yv = N_VNew_Serial(NEQ_MANIP/2, *sunctx);
        for (size_t i = 0; i < NEQ_MANIP/2; i++) { 
            Ith(yv, i) = Ith(y, INIT_SLICE_MANIP + i);
        }

        // Compute max difference
        double max_diff = 0.0;
        for (size_t j = 0; j < NEQ_MANIP/2; j++) {
            double diff = abs(Ith(data->yD, j) - Ith(yv, j));

            // Check for maximum
            max_diff = (max_diff < diff) ? diff : max_diff;
        }

        // Check if torque is required
        if (max_diff > 1e-5)
        {
            // cout << "in " << max_diff << endl;
            inv_dyn(y, tau, data);
        } else {
            // cout << "out " << max_diff << endl;
            N_VConst(0.0, tau);
        }
        
        // Clean up
        N_VDestroy(yv);
             
    } else {
        // Set tau to zero
        N_VConst(0.0, tau);
    }

    // Add eddy current torque (TODO)

    // Save torque
    for (size_t i = 0; i < NEQ_MANIP/2; i++)
    {
        Ith(data->additional, EE_TOR_INIT_SLICE + i) = Ith(tau, i);
    }

    // Computations
    // EoM:
    //    dq  = y(n+1:end)
    //    ddq = -Dv\(Cv*y(n+1:end)) + Dv\tau;
    //

    // Extract slice of y
    for (size_t i = NEQ_MANIP/2; i < NEQ_MANIP; i++) { Ith(y_s, i - NEQ_MANIP/2) = Ith(y_m, i); }

    // Perform the operation Cv * y(n+1:end)
    SUNMatMatvec(data->Cv, y_s, ddq);
    
    // Perform the operation - [A=Dv] \ [b = (Cv * y(n+1:end))] -> [Ax = b]
    // LS = SUNLinSol_Dense(ddq, Dv, *sunctx);
    // SUNLinSolSetup(LS, Dv);
    // SUNLinSolSolve(LS, Dv, ddq, ddq, 0.0);
    sol1 = solve_linear_system(data->Dv, ddq, *sunctx);

    // Perform the operation [A=Dv] \ [b = tau] -> [Ax = b]
    // LS = SUNLinSol_Dense(tau, Dv, *sunctx);
    // SUNLinSolSetup(LS, Dv);
    // SUNLinSolSolve(LS, Dv, tmp, tau, 0.0);
    tmp = solve_linear_system(data->Dv, tau, *sunctx);

    // Perform summation
    N_VLinearSum(-1, sol1, 1, tmp, sol2);

    // Equations of motion
    // dq
    Ith(ydot, INIT_SLICE_MANIP + 0) = Ith(y, INIT_SLICE_MANIP + 6);
    Ith(ydot, INIT_SLICE_MANIP + 1) = Ith(y, INIT_SLICE_MANIP + 7);
    Ith(ydot, INIT_SLICE_MANIP + 2) = Ith(y, INIT_SLICE_MANIP + 8);
    Ith(ydot, INIT_SLICE_MANIP + 3) = Ith(y, INIT_SLICE_MANIP + 9);
    Ith(ydot, INIT_SLICE_MANIP + 4) = Ith(y, INIT_SLICE_MANIP + 10);
    Ith(ydot, INIT_SLICE_MANIP + 5) = Ith(y, INIT_SLICE_MANIP + 11);

    // ddq
    Ith(ydot, INIT_SLICE_MANIP + 6)  = Ith(sol2, INIT_SLICE_MANIP + 0);
    Ith(ydot, INIT_SLICE_MANIP + 7)  = Ith(sol2, INIT_SLICE_MANIP + 1);
    Ith(ydot, INIT_SLICE_MANIP + 8)  = Ith(sol2, INIT_SLICE_MANIP + 2);
    Ith(ydot, INIT_SLICE_MANIP + 9)  = Ith(sol2, INIT_SLICE_MANIP + 3);
    Ith(ydot, INIT_SLICE_MANIP + 10) = Ith(sol2, INIT_SLICE_MANIP + 4);
    Ith(ydot, INIT_SLICE_MANIP + 11) = Ith(sol2, INIT_SLICE_MANIP + 5);

    // Clean up
    N_VDestroy(tau);
    N_VDestroy(y_s);
    N_VDestroy(y_m);
    N_VDestroy(ddq);
    N_VDestroy(tmp);
    N_VDestroy(sol1);
    N_VDestroy(sol2);

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

N_Vector end_effector_linang_velocity(N_Vector y, SUNContext sunctx, void* user_data) {
    // Compute Jacobian
    SUNMatrix J = compute_jacobian(y, user_data);

    // Init vectors
    N_Vector vel = N_VNew_Serial(6, sunctx);
    N_Vector q_dot = N_VNew_Serial(6, sunctx);

    // Extract joint angular velocities
    for (size_t i = 0; i < 6; i++)
    {
        Ith(q_dot, i) = Ith(y, INIT_SLICE_MANIP + 6 + i);
    }

    // Compute velocity
    SUNMatMatvec(J, q_dot, vel);
    
    // Free up memory
    SUNMatDestroy(J);
    N_VDestroy(q_dot);

    return vel;
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
    double norm = sqrt(N_VDotProd(pos, pos));
    N_VScale(1.0 / norm, pos, pos);
    
    // Free up memory
    SUNMatDestroy(T);

    return pos;
}

SUNMatrix get_joint_matrix(int joint_number, N_Vector y, SUNContext sunctx, void* user_data) {
    // Load parameters
    UserData data = (UserData)user_data;
    vector<double> theta(6, 0.0); for (size_t i = 0; i < NEQ_MANIP/2; i++) { theta[i] = Ith(y, INIT_SLICE_MANIP + i); }
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
            SUNMatrix mat_mul_mat = mat_mul(T0i, Ti, sunctx);
            SUNMatCopy(mat_mul_mat, T0i);

            SUNMatDestroy(mat_mul_mat);

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
            double sum = 0.0;
            for (int k = 0; k < n; k++) {
                sum += IJth(A, i, k) * IJth(B, k, j);
            }
            IJth(C, i, j) = sum;
        }
    }

    return C;
}

N_Vector solve_linear_system(SUNMatrix As, N_Vector bs, SUNContext sunctx) {
    // ONLY SQUARE MATRICES
    // Define
    int n = NV_LENGTH_S(bs);
    vector<vector<double>> A(n, vector<double>(n, 0.0));
    vector<double> b(n, 0.0);

    // Convert
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            A[i][j] = IJth(As, i, j);
        }
        b[i] = Ith(bs, i);
    }

    n = A.size();
    vector<vector<double>> augmentedMatrix = A;
    for (size_t i = 0; i < A.size(); ++i) {
        augmentedMatrix[i].push_back(b[i]);
    }

    // Gaussian elimination
    for (int i = 0; i < n; ++i) {
        int max_row = i;
        for (int j = i + 1; j < n; ++j) {
            if (abs(augmentedMatrix[j][i]) > abs(augmentedMatrix[max_row][i])) {
                max_row = j;
            }
        }
        if (max_row != i) {
            swap(augmentedMatrix[i], augmentedMatrix[max_row]);
        }
        for (int j = i + 1; j < n; ++j) {
            double factor = augmentedMatrix[j][i] / augmentedMatrix[i][i];
            for (int k = i; k <= n; ++k) {
                augmentedMatrix[j][k] -= factor * augmentedMatrix[i][k];
            }
        }
    }

    // Back substitution
    vector<double> x(n);
    for (int i = n - 1; i >= 0; --i) {
        x[i] = augmentedMatrix[i][n];
        for (int j = i + 1; j < n; ++j) {
            x[i] -= augmentedMatrix[i][j] * x[j];
        }
        x[i] /= augmentedMatrix[i][i];
    }

    // Convert
    N_Vector xc = N_VNew_Serial(n, sunctx);
    for (int i = 0; i < n; i++)
    {
        Ith(xc, i) = x[i];
    }
    
    return xc;
}

void sub_mat_to_vec(SUNMatrix mat, N_Vector sub_vec, int column, vector<int> slice) {

    for (int i = slice[0]; i <= slice[1]; i++)
    {   
        // Extract
        Ith(sub_vec, i - slice[0]) = IJth(mat, i, column);
    }

    return;
}

void sub_mat(SUNMatrix mat, SUNMatrix sub_mat, vector<int> i_slice, vector<int> j_slice) {
    for (int i = i_slice[0]; i <= i_slice[1]; i++)
    {
        for (int j = j_slice[0]; j <= j_slice[1]; j++)
        {   
            // Extract
            IJth(sub_mat, i - i_slice[0], j - j_slice[0]) = IJth(mat, i, j);
        }
    }

    return;
}

N_Vector cross(SUNContext sunctx, N_Vector a, N_Vector b) {
    N_Vector result = N_VNew_Serial(3, sunctx);

    Ith(result, 0) = Ith(a, 1) * Ith(b, 2) - Ith(a, 2) * Ith(b, 1);
    Ith(result, 1) = Ith(a, 2) * Ith(b, 0) - Ith(a, 0) * Ith(b, 2);
    Ith(result, 2) = Ith(a, 0) * Ith(b, 1) - Ith(a, 1) * Ith(b, 0);
    
    return result;
}

SUNMatrix transpose(SUNContext sunctx, SUNMatrix mat) {
    // Get the sizes of the matrices
    int m = SM_ROWS_D(mat);
    int n = SM_COLUMNS_D(mat);

    // Create a dense matrix to store the result of the multiplication
    SUNMatrix mat_t = SUNDenseMatrix(n, m, sunctx);

    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            IJth(mat_t, j, i) = IJth(mat, i, j);
        }
    }

    return mat_t;
}

void rotm2axang(SUNMatrix R, double& angle, N_Vector axis) {
    // Ensure R is a valid rotation matrix
    if (SM_ROWS_D(R) != 3 || SM_COLUMNS_D(R) != 3) {
        std::cerr << "Input must be a 3x3 matrix: " << SM_ROWS_D(R) << "x" << SM_COLUMNS_D(R) << std::endl;
        return;
    }

    // Calculate the trace of the rotation matrix
    double trace_R = 0.0;
    for (int i = 0; i < 3; ++i) {
        trace_R += IJth(R, i, i); // Sum of each column
    }

    if (trace_R > 2) {
        // Case when trace is greater than 2
        // This corresponds to the rotation angle being close to zero
        angle = 0.0;
        Ith(axis, 0) = 0.0;
        Ith(axis, 1) = 0.0;
        Ith(axis, 2) = 1.0; // Any axis can be chosen since angle is zero
    } else if (trace_R < -1) {
        // Case when trace is less than -1
        // This corresponds to the rotation angle being close to pi
        int max_i = 0;
        double max_val = IJth(R, 0, 0);
        for (int j = 1; j < 3; ++j) {
            if (IJth(R, j, j) > max_val) {
                max_val = IJth(R, j, j);
                max_i = j;
            }
        }
        vector<double> v(3, 0.0);
        for (int j = 0; j < 3; ++j) {
            v[j] = sqrt((max_val + 1) / 2) * IJth(R, j, max_i) / 2;
        }
        angle = PI;
        double norm_v = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
        Ith(axis, 0) = v[0] / norm_v;
        Ith(axis, 1) = v[1] / norm_v;
        Ith(axis, 2) = v[2] / norm_v;
    } else {
        // Case when the angle is between 0 and pi
        angle = acos((trace_R - 1) / 2);
        double s = 1 / (2 * sin(angle));
        Ith(axis, 0) = s * (IJth(R, 2, 1) - IJth(R, 1, 2));
        Ith(axis, 1) = s * (IJth(R, 0, 2) - IJth(R, 2, 0));
        Ith(axis, 2) = s * (IJth(R, 1, 0) - IJth(R, 0, 1));
    }
}

SUNMatrix pseudo_inverse(SUNContext sunctx, SUNMatrix mat) {
    // Convert SUNMatrix to MatrixXd
    Eigen::MatrixXd matrix(SM_ROWS_D(mat), SM_COLUMNS_D(mat));
    for (int i = 0; i < SM_ROWS_D(mat); i++) { for (int j = 0; j < SM_COLUMNS_D(mat); j++) { matrix(i, j) = IJth(mat, i, j); } }

    // Compute pseudo-inverse
    Eigen::MatrixXd pinv = matrix.completeOrthogonalDecomposition().pseudoInverse();

    // Convert back to SUNMatrix
    SUNMatrix pinv_sun = SUNDenseMatrix(SM_ROWS_D(mat), SM_COLUMNS_D(mat), sunctx);
    for (int i = 0; i < SM_ROWS_D(mat); i++) { for (int j = 0; j < SM_COLUMNS_D(mat); j++) { IJth(pinv_sun, i, j) = pinv(i, j); } }
    
    return pinv_sun;
}