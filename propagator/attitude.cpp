#include <iostream>

/* Sundials */
#include <cvode/cvode.h>
#include <nvector/nvector_serial.h>
#include <sunlinsol/sunlinsol_dense.h>
#include <sunmatrix/sunmatrix_dense.h>

/* Local */
#include "attitude.h"
#include "propagator.h"

/* Problem Constants */
#define WX0   SUN_RCONST(0.2)  /* initial y components */
#define WY0   SUN_RCONST(0.3)
#define WZ0   SUN_RCONST(0.1)
#define Q10   SUN_RCONST(0.0)
#define Q20   SUN_RCONST(0.0)
#define Q30   SUN_RCONST(0.0)
#define Q40   SUN_RCONST(1.0)

#define IXX   SUN_RCONST(3463.542)
#define IYY   SUN_RCONST(3463.542)
#define IZZ   SUN_RCONST(2968.75)

#define ZERO  SUN_RCONST(0.0)


int initiate(SUNContext sunctx, N_Vector y, const int y_init_slice, void* user_data) {
  /* Variables */
  SUNMatrix I;

  /* Initialize y_attitude */
  Ith(y, y_init_slice + 0) = WX0;
  Ith(y, y_init_slice + 1) = WY0;
  Ith(y, y_init_slice + 2) = WZ0;
  Ith(y, y_init_slice + 3) = Q10;
  Ith(y, y_init_slice + 4) = Q20;
  Ith(y, y_init_slice + 5) = Q30;
  Ith(y, y_init_slice + 6) = Q40;

  /* Generate Inertia matrix*/
  I = SUNDenseMatrix(3, 3, sunctx);
  IJth(I, 0, 0) = IXX; 
  IJth(I, 1, 1) = IYY; 
  IJth(I, 2, 2) = IZZ;

  UserData data  = (UserData)user_data;
  data->I = I;

  return(0);
}

int f_attitude(sunrealtype t, N_Vector y, N_Vector ydot, void* user_data)
{
  sunrealtype wx, wy, wz, q1, q2, q3, q4;
  SUNMatrix I;
  UserData data;

  /* Retrieve user data */
  data  = (UserData)user_data;
  I = data->I;
  
  wx = Ith(y, 0);
  wy = Ith(y, 1);
  wz = Ith(y, 2);
  q1 = Ith(y, 3);
  q2 = Ith(y, 4);
  q3 = Ith(y, 5);
  q4 = Ith(y, 6);

  /* Retrieve Inertia matrix columns */
  sunrealtype** I_cols = SUNDenseMatrix_Cols(I);

  Ith(ydot, 0) = (ZERO - (I_cols[2][2] - I_cols[1][1]) * wz * wy) / I_cols[0][0];
  Ith(ydot, 1) = (ZERO - (I_cols[0][0] - I_cols[2][2]) * wx * wz) / I_cols[1][1];
  Ith(ydot, 2) = (ZERO - (I_cols[1][1] - I_cols[0][0]) * wy * wx) / I_cols[2][2];
  Ith(ydot, 3) = SUN_RCONST(0.5) * (wz * q2 - wy * q3 + wx * q4);
  Ith(ydot, 4) = SUN_RCONST(0.5) * (-wz * q1 + wx * q3 + wy * q4);
  Ith(ydot, 5) = SUN_RCONST(0.5) * (wy * q1 - wx * q2 + wz * q4);
  Ith(ydot, 6) = SUN_RCONST(0.5) * (-wx * q1 - wy * q2 - wz * q3);

  return (0);
}