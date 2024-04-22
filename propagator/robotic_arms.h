#ifndef ROBOTIC_ARMS_H
#define ROBOTIC_ARMS_H

#define NEQ_MANIP  12  /* number of equations  */

int initiate_manipulator(SUNContext sunctx, N_Vector y, void* user_data);

int f_manipulator(sunrealtype t, N_Vector y, N_Vector ydot, void* user_data);

#endif

