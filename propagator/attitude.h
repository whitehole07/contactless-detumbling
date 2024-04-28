#include <nvector/nvector_serial.h>
#include <sunlinsol/sunlinsol_dense.h>

#ifndef ATTITUDE_H
#define ATTITUDE_H

#define NEQ_ATTITUDE  7  /* number of equations  */


int initiate_attitude(SUNContext sunctx, N_Vector y, void* user_data);

int f_attitude(sunrealtype t, N_Vector y, N_Vector ydot, void* user_data);

#endif

