#include <iostream>
#include <cmath>

/* Sundials */
#include <cvode/cvode.h>
#include <nvector/nvector_serial.h>
#include <sunlinsol/sunlinsol_dense.h>
#include <sunmatrix/sunmatrix_dense.h>

/* Local */
#include "attitude.h"
#include "robotic_arms.h"
#include "propagator.h"

/* Constants */
#define PI    SUN_RCONST(3.14159265358979323846)
#define ZERO  SUN_RCONST(0.0)
#define MU0   4 * PI * 1e-7

/* Problem IV */
#define WX0   SUN_RCONST(0.1)
#define WY0   SUN_RCONST(0.2)
#define WZ0   SUN_RCONST(0.0)
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
#define MAG_CURRENT  50


int initiate_attitude(SUNContext sunctx, N_Vector y, void* user_data);

int f_attitude(sunrealtype t, N_Vector y, N_Vector ydot, void* user_data);

N_Vector get_magnetic_field(N_Vector y, SUNContext sunctx);

N_Vector eddy_current_torque(SUNContext sunctx, N_Vector y, UserData user_data);

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

  // Get magnetic tensor
  SUNMatrix M = SUNDenseMatrix(3, 3, sunctx);
  sunrealtype gamma  = 1 - ((2*DEBRIS_RADIUS/DEBRIS_HEIGHT) * tanh(DEBRIS_HEIGHT/(2*DEBRIS_RADIUS)));
  sunrealtype magn   = PI * DEBRIS_SIGMA * pow(DEBRIS_RADIUS, 3) * DEBRIS_THICK * DEBRIS_HEIGHT;
  IJth(M, 0, 0) = gamma * magn;
  IJth(M, 1, 1) = gamma * magn;
  IJth(M, 2, 2) = 0.5   * magn;

  // Set user data
  UserData data  = (UserData)user_data;
  data->I = I;
  data->M = M;

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
  
  wx = Ith(y, INIT_SLICE_ATTITUDE + 0);
  wy = Ith(y, INIT_SLICE_ATTITUDE + 1);
  wz = Ith(y, INIT_SLICE_ATTITUDE + 2);
  q1 = Ith(y, INIT_SLICE_ATTITUDE + 3);
  q2 = Ith(y, INIT_SLICE_ATTITUDE + 4);
  q3 = Ith(y, INIT_SLICE_ATTITUDE + 5);
  q4 = Ith(y, INIT_SLICE_ATTITUDE + 6);

  // Retrieve Inertia matrix columns
  sunrealtype** I_cols = SUNDenseMatrix_Cols(I);

  // Compute eddy current torque
  N_Vector T_eddy = eddy_current_torque(*sunctx, y, data);

  Ith(ydot, INIT_SLICE_ATTITUDE + 0) = (Ith(T_eddy, 0) - (I_cols[2][2] - I_cols[1][1]) * wz * wy) / I_cols[0][0];
  Ith(ydot, INIT_SLICE_ATTITUDE + 1) = (Ith(T_eddy, 1) - (I_cols[0][0] - I_cols[2][2]) * wx * wz) / I_cols[1][1];
  Ith(ydot, INIT_SLICE_ATTITUDE + 2) = (Ith(T_eddy, 2) - (I_cols[1][1] - I_cols[0][0]) * wy * wx) / I_cols[2][2];
  Ith(ydot, INIT_SLICE_ATTITUDE + 3) = SUN_RCONST(0.5) * (wz * q2 - wy * q3 + wx * q4);
  Ith(ydot, INIT_SLICE_ATTITUDE + 4) = SUN_RCONST(0.5) * (-wz * q1 + wx * q3 + wy * q4);
  Ith(ydot, INIT_SLICE_ATTITUDE + 5) = SUN_RCONST(0.5) * (wy * q1 - wx * q2 + wz * q4);
  Ith(ydot, INIT_SLICE_ATTITUDE + 6) = SUN_RCONST(0.5) * (-wx * q1 - wy * q2 - wz * q3);

  return (0);
}

N_Vector get_magnetic_field(N_Vector y, SUNContext sunctx) {
  /* Compute magnetic field */
  // Vectorial component
  N_Vector B = N_VNew_Serial(3, sunctx);

  // Extract end effector location
  N_Vector location = end_effector_position(y, sunctx);
  cout << "location norm: " << sqrt(N_VDotProd(location, location)) << endl;    

  // Extract end effector pose
  N_Vector moment = end_effector_pose(y, sunctx);
  cout << "pose norm: " << sqrt(N_VDotProd(moment, moment)) << endl;    

  // Scalar components
  sunrealtype mag = (MU0 * MAG_N_TURNS * MAG_CURRENT * (PI*pow(MAG_RADIUS, 2))) / (4 * PI);
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

  // Free the memory allocated for the N_Vector objects
  N_VDestroy(location);
  N_VDestroy(moment);

  return B;
}

N_Vector eddy_current_torque(SUNContext sunctx, N_Vector y, UserData user_data) {
  // Compute magnetic field
  N_Vector B = get_magnetic_field(y, sunctx);

  // Get magnetic tensor
  SUNMatrix M = user_data->M;

  // Extract relative angular velocity
  N_Vector wr = N_VNew_Serial(3, sunctx);  // ASSUMPTION: no chaser contribution to wr
  N_Vector tmp = N_VNew_Serial(3, sunctx);
  for (size_t i = 0; i < 3; i++) { Ith(wr, i) = Ith(y, INIT_SLICE_ATTITUDE + i); }

  // Compute torque
  // Compute cross product: cross(wr, B)
  wr = cross(sunctx, wr, B);
  
  // Perform the matrix-vector multiplication: M * temp1
  SUNMatMatvecSetup(M);
  SUNMatMatvec(M, wr, tmp);

  // Torque, perform the cross product: cross(M * cross(wr, B), B)
  N_Vector T = cross(sunctx, tmp, B);

  // Free memory
  N_VDestroy(B);
  N_VDestroy(wr);
  N_VDestroy(tmp);

  return T;
}

N_Vector cross (SUNContext sunctx, N_Vector a, N_Vector b) {
    N_Vector result = N_VNew_Serial(3, sunctx);

    Ith(result, 0) = Ith(a, 1) * Ith(b, 2) - Ith(a, 2) * Ith(b, 1);
    Ith(result, 1) = Ith(a, 2) * Ith(b, 0) - Ith(a, 0) * Ith(b, 2);
    Ith(result, 2) = Ith(a, 0) * Ith(b, 1) - Ith(a, 1) * Ith(b, 0);
    
    return result;
}