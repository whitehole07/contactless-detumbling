#include <iostream>
#include <cmath>

/* Sundials */
#include <cvode/cvode.h>
#include <nvector/nvector_serial.h>
#include <sunlinsol/sunlinsol_dense.h>
#include <sunmatrix/sunmatrix_dense.h>

/* Local */
#include "attitude.h"
#include "propagator.h"

/* Constants */
#define PI    SUN_RCONST(3.14159265358979323846)
#define ZERO  SUN_RCONST(0.0)
#define MU0   4 * PI * 1e-7

/* Problem IV */
#define WX0   SUN_RCONST(0.2)
#define WY0   SUN_RCONST(0.3)
#define WZ0   SUN_RCONST(0.1)
#define Q10   SUN_RCONST(0.0)
#define Q20   SUN_RCONST(0.0)
#define Q30   SUN_RCONST(0.0)
#define Q40   SUN_RCONST(1.0)

/* Debris Properties */
#define DEBRIS_MASS     950        // [kg]
#define DEBRIS_HEIGHT   5          // [m]
#define DEBRIS_RADIUS   2.5        // [m]
#define DEBRIS_SIGMA    35000000
#define DEBRIS_THICK    0.1

#define DEBRIS_IXX   (0.083333333333333) * DEBRIS_MASS * (pow(DEBRIS_HEIGHT, 2) + 3 * pow(DEBRIS_RADIUS, 2))   // SUN_RCONST(3463.542)
#define DEBRIS_IYY   (0.083333333333333) * DEBRIS_MASS * (pow(DEBRIS_HEIGHT, 2) + 3 * pow(DEBRIS_RADIUS, 2))   // SUN_RCONST(3463.542)
#define DEBRIS_IZZ   (0.500000000000000) * DEBRIS_MASS * (                            pow(DEBRIS_RADIUS, 2))   // SUN_RCONST(2968.75)

/* Eddy Current Torque */
#define MAG_N_TURNS  500
#define MAG_RADIUS   2
#define MAG_CURRENT  5000
#define MAG_POSE     1 // TODO: check

// Magnetic Tensor
#define GAMMA   1 - (2*DEBRIS_RADIUS/DEBRIS_HEIGHT) * tanh(DEBRIS_HEIGHT/(2*DEBRIS_RADIUS))
#define MAGN    PI * DEBRIS_SIGMA * pow(DEBRIS_RADIUS, 3) * DEBRIS_THICK * DEBRIS_HEIGHT

#define MAGN_TENS11   MAGN * GAMMA
#define MAGN_TENS22   MAGN * GAMMA
#define MAGN_TENS33   MAGN * 0.5


int initiate_attitude(SUNContext sunctx, N_Vector y, void* user_data);

int f_attitude(sunrealtype t, N_Vector y, N_Vector ydot, void* user_data);

N_Vector get_magnetic_field(SUNContext sunctx);

N_Vector eddy_current_torque(SUNContext sunctx, N_Vector y);

N_Vector cross (SUNContext sunctx, N_Vector a, N_Vector b);


int initiate_attitude(SUNContext sunctx, N_Vector y, void* user_data) {
  /* Variables */
  SUNMatrix I;

  /* Initialize y_attitude */
  Ith(y, INIT_SLICE_ATTITUDE + 0) = WX0;
  Ith(y, INIT_SLICE_ATTITUDE + 1) = WY0;
  Ith(y, INIT_SLICE_ATTITUDE + 2) = WZ0;
  Ith(y, INIT_SLICE_ATTITUDE + 3) = Q10;
  Ith(y, INIT_SLICE_ATTITUDE + 4) = Q20;
  Ith(y, INIT_SLICE_ATTITUDE + 5) = Q30;
  Ith(y, INIT_SLICE_ATTITUDE + 6) = Q40;

  /* Generate Inertia matrix*/
  I = SUNDenseMatrix(3, 3, sunctx);
  IJth(I, 0, 0) = DEBRIS_IXX; 
  IJth(I, 1, 1) = DEBRIS_IYY; 
  IJth(I, 2, 2) = DEBRIS_IZZ;

  UserData data  = (UserData)user_data;
  data->I = I;

  return(0);
}

int f_attitude(sunrealtype t, N_Vector y, N_Vector ydot, void* user_data)
{
  SUNContext* sunctx;
  sunrealtype wx, wy, wz, q1, q2, q3, q4;
  SUNMatrix I;
  UserData data;

  /* Retrieve user data */
  data  = (UserData)user_data;
  sunctx = data->sunctx;
  I = data->I;
  
  wx = Ith(y, 0);
  wy = Ith(y, 1);
  wz = Ith(y, 2);
  q1 = Ith(y, 3);
  q2 = Ith(y, 4);
  q3 = Ith(y, 5);
  q4 = Ith(y, 6);

  // Retrieve Inertia matrix columns
  sunrealtype** I_cols = SUNDenseMatrix_Cols(I);

  // Compute eddy current torque
  N_Vector T_eddy = eddy_current_torque(*sunctx, y);

  Ith(ydot, 0) = (Ith(T_eddy, 0) - (I_cols[2][2] - I_cols[1][1]) * wz * wy) / I_cols[0][0];
  Ith(ydot, 1) = (Ith(T_eddy, 1) - (I_cols[0][0] - I_cols[2][2]) * wx * wz) / I_cols[1][1];
  Ith(ydot, 2) = (Ith(T_eddy, 2) - (I_cols[1][1] - I_cols[0][0]) * wy * wx) / I_cols[2][2];
  Ith(ydot, 3) = SUN_RCONST(0.5) * (wz * q2 - wy * q3 + wx * q4);
  Ith(ydot, 4) = SUN_RCONST(0.5) * (-wz * q1 + wx * q3 + wy * q4);
  Ith(ydot, 5) = SUN_RCONST(0.5) * (wy * q1 - wx * q2 + wz * q4);
  Ith(ydot, 6) = SUN_RCONST(0.5) * (-wx * q1 - wy * q2 - wz * q3);

  return (0);
}

N_Vector get_magnetic_field(SUNContext sunctx) {
  /* Compute magnetic field */
  // Vectorial component
  N_Vector B = N_VNew_Serial(3, sunctx);

  N_Vector location = N_VNew_Serial(3, sunctx);
  Ith(location, 0) = 7; // TODO: recover from y (same for pose)
  Ith(location, 1) = 0;
  Ith(location, 2) = 0;

  N_Vector moment = N_VNew_Serial(3, sunctx);
  Ith(moment, 0) = 1;
  Ith(moment, 1) = 0;
  Ith(moment, 2) = 0;

  // Scalar components
  sunrealtype mag = (MU0 * MAG_N_TURNS * MAG_CURRENT * (pow(PI*MAG_RADIUS, 2))) / (4 * PI);
  sunrealtype r = sqrt(N_VDotProd(location, location));

  // Compute the dot product: dot(moment,location)
  sunrealtype dot_product = N_VDotProd(moment, location);

  // Compute 3 * location
  N_VScale(3.0, location, B);

  // Perform the outer dot product: 3 * location * dot(moment,location)
  N_VScale(dot_product, B, B);

  // Perform the element-wise operations
  N_VLinearSum(1.0 / pow(r, 5), B, -1.0 / pow(r, 3), moment, B);
  N_VScale(mag, B, B);

  // Output the transformed expression
  /* std::cout << "Transformed expression: ";
  for (int i = 0; i < 3; ++i) {
      std::cout << Ith(B, i) << " ";
  }
  std::cout << std::endl; */

  // Free the memory allocated for the N_Vector objects
  N_VDestroy(location);
  N_VDestroy(moment);

  return B;
}

N_Vector eddy_current_torque(SUNContext sunctx, N_Vector y) {
  // Compute magnetic field
  N_Vector B = get_magnetic_field(sunctx);

  // Get magnetic tensor
  SUNMatrix M = SUNDenseMatrix(3, 3, sunctx);
  IJth(M, 0, 0) = MAGN * GAMMA;
  IJth(M, 1, 1) = MAGN * GAMMA;
  IJth(M, 2, 2) = MAGN * 0.5;

  // Extract relative angular velocity
  N_Vector wr = N_VNew_Serial(3, sunctx);  // ASSUMPTION: no chaser contribution to wr
  for (size_t i = 0; i < 3; i++) { Ith(wr, i) = Ith(y, INIT_SLICE_ATTITUDE + i); }

  // Compute torque
  // Compute cross product: cross(wr, B)
  wr = cross(sunctx, wr, B);
  
  // Perform the matrix-vector multiplication: M * temp1
  SUNMatMatvec(M, wr, wr);

  // Torque, perform the cross product: cross(M * cross(wr, B), B)
  N_Vector T = cross(sunctx, wr, B);

  // Free memory
  N_VDestroy(B);
  N_VDestroy(wr);
  SUNMatDestroy(M);

  return T;
}

N_Vector cross (SUNContext sunctx, N_Vector a, N_Vector b) {
    N_Vector result = N_VNew_Serial(3, sunctx);

    Ith(result, 0) = Ith(a, 1) * Ith(b, 2) - Ith(a, 2) * Ith(b, 1);
    Ith(result, 1) = Ith(a, 2) * Ith(b, 0) - Ith(a, 0) * Ith(b, 2);
    Ith(result, 2) = Ith(a, 0) * Ith(b, 1) - Ith(a, 1) * Ith(b, 0);
    
    return result;
}