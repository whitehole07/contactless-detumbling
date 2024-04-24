#ifndef ROBOTIC_ARMS_H
#define ROBOTIC_ARMS_H

#define NEQ_MANIP  12  /* number of equations  */

/* Global Variables: Robot's Base Frame to Target's Body Frame */
#define ORIGIN_XDISTANCE_TO_DEBRIS  10.0  // [m]
#define ORIGIN_YDISTANCE_TO_DEBRIS  0.0   // [m]
#define ORIGIN_ZDISTANCE_TO_DEBRIS  5.0   // [m]

/* Denavit-Hartenberg Parameters */
#define SCALE       10
#define DH_A        {           0, SCALE*-0.6127, SCALE*-0.57155,             0,             0,             0}
#define DH_D        {SCALE*0.1807,             0,              0, SCALE*0.17415, SCALE*0.11985, SCALE*0.11655}
#define DH_ALPHA    {        PI/2,             0,              0,          PI/2,         -PI/2,             0}


int initiate_manipulator(SUNContext sunctx, N_Vector y, void* user_data);

int f_manipulator(sunrealtype t, N_Vector y, N_Vector ydot, void* user_data);

N_Vector end_effector_position(N_Vector y, SUNContext sunctx);

N_Vector end_effector_pose(N_Vector y, SUNContext sunctx);

SUNMatrix get_joint_matrix(int joint_number, N_Vector y, SUNContext sunctx);

SUNMatrix get_transformation_matrix(int final_joint_number, N_Vector y, SUNContext sunctx);

SUNMatrix mat_mul(SUNMatrix A, SUNMatrix B, SUNContext sunctx);

#endif

