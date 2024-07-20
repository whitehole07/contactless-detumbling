#include <nvector/nvector_serial.h>
#include <sunlinsol/sunlinsol_dense.h>

#ifndef ORBIT_H
#define ORBIT_H

#define NEQ_ORBIT  6  /* number of equations  */
#define MU 3.98600433e+5


int initiate_orbit(SUNContext sunctx, N_Vector y, void* user_data);

int f_orbit(sunrealtype t, N_Vector y, N_Vector ydot, void* user_data);

SUNMatrix LVLH_to_inertia(N_Vector y, SUNContext sunctx);

#endif

