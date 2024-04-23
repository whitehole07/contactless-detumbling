#ifndef ROBOTIC_ARMS_H
#define ROBOTIC_ARMS_H

#define NEQ_MANIP  12  /* number of equations  */


int initiate_manipulator(SUNContext sunctx, N_Vector y, void* user_data);

int f_manipulator(sunrealtype t, N_Vector y, N_Vector ydot, void* user_data);

N_Vector end_effector_position(N_Vector y, SUNContext sunctx);

N_Vector end_effector_pose(N_Vector y, SUNContext sunctx);

SUNMatrix get_joint_matrix(int joint_number, N_Vector y, SUNContext sunctx);

SUNMatrix get_transformation_matrix(int final_joint_number, N_Vector y, SUNContext sunctx);

SUNMatrix mat_mul(SUNMatrix A, SUNMatrix B, SUNContext sunctx);

#endif

