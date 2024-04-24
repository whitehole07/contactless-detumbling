#include <nvector/nvector_serial.h>
#include <sunlinsol/sunlinsol_dense.h>

#ifndef ATTITUDE_H
#define ATTITUDE_H

#define NEQ_ATTITUDE  7  /* number of equations  */

/* Debris Properties */
#define DEBRIS_MASS     950.0        // [kg]
#define DEBRIS_HEIGHT   5.0          // [m]
#define DEBRIS_RADIUS   2.5          // [m]
#define DEBRIS_SIGMA    35000000.0  
#define DEBRIS_THICK    0.1

#define DEBRIS_IXX   (0.083333333333333) * DEBRIS_MASS * (pow(DEBRIS_HEIGHT, 2) + 3 * pow(DEBRIS_RADIUS, 2))   // SUN_RCONST(3463.542)
#define DEBRIS_IYY   (0.083333333333333) * DEBRIS_MASS * (pow(DEBRIS_HEIGHT, 2) + 3 * pow(DEBRIS_RADIUS, 2))   // SUN_RCONST(3463.542)
#define DEBRIS_IZZ   (0.500000000000000) * DEBRIS_MASS * (                            pow(DEBRIS_RADIUS, 2))   // SUN_RCONST(2968.75)

/* Eddy Current Torque */
#define MAG_N_TURNS  500.0
#define MAG_RADIUS   1.0
#define MAG_CURRENT  50.0

int initiate_attitude(SUNContext sunctx, N_Vector y, void* user_data);

int f_attitude(sunrealtype t, N_Vector y, N_Vector ydot, void* user_data);

#endif

