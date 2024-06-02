#include <nvector/nvector_serial.h>
#include <sunlinsol/sunlinsol_dense.h>

#ifndef ROBOTIC_ARMS_H
#define ROBOTIC_ARMS_H

#define NEQ_MANIP  12  /* number of equations  */

/* Constants */
#define PI    SUN_RCONST(3.14159265358979323846)
#define ZERO  SUN_RCONST(0.0)


int initiate_manipulator(SUNContext sunctx, N_Vector y, void* user_data);

int f_manipulator(sunrealtype t, N_Vector y, N_Vector ydot, void* user_data);

N_Vector end_effector_position(N_Vector y, SUNContext sunctx, void* user_data);

N_Vector end_effector_pose(N_Vector y, SUNContext sunctx, void* user_data);

N_Vector end_effector_linang_velocity(N_Vector y, SUNContext sunctx, void* user_data);

SUNMatrix get_joint_matrix(int joint_number, N_Vector y, SUNContext sunctx, void* user_data);

SUNMatrix get_transformation_matrix(int final_joint_number, N_Vector y, SUNContext sunctx, void* user_data);

SUNMatrix mat_mul(SUNMatrix A, SUNMatrix B, SUNContext sunctx);

N_Vector cross(SUNContext sunctx, N_Vector a, N_Vector b);

N_Vector inv_dyn(N_Vector y, void* user_data);

#endif

