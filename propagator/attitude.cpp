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


int initiate_attitude(SUNContext sunctx, N_Vector y, void* user_data);

int f_attitude(sunrealtype t, N_Vector y, N_Vector ydot, void* user_data);

N_Vector get_magnetic_field(N_Vector y, SUNContext sunctx, UserData user_data);

N_Vector eddy_current_torque(SUNContext sunctx, N_Vector y, UserData user_data);


int initiate_attitude(SUNContext sunctx, N_Vector y, void* user_data) {

  // Get user data
  UserData data = (UserData)user_data;

  /* Initialize y_attitude */
  for (size_t i = 0; i < NEQ_ATTITUDE; i++)
  {
    Ith(y, INIT_SLICE_ATTITUDE + i) = data->y0[INIT_SLICE_ATTITUDE + i];
  }

  /* Generate Inertia matrix*/
  SUNMatrix I = SUNDenseMatrix(3, 3, sunctx);
  IJth(I, 0, 0) = data->debris_Ixx; 
  IJth(I, 1, 1) = data->debris_Iyy; 
  IJth(I, 2, 2) = data->debris_Izz;

  // Get magnetic tensor
  SUNMatrix M = SUNDenseMatrix(3, 3, sunctx);
  sunrealtype gamma  = 1 - ((2*data->debris_radius/data->debris_height) * tanh(data->debris_height/(2*data->debris_radius)));
  sunrealtype magn   = PI * data->debris_sigma * pow(data->debris_radius, 3) * data->debris_thick * data->debris_height;
  IJth(M, 0, 0) = gamma * magn;
  IJth(M, 1, 1) = gamma * magn;
  IJth(M, 2, 2) = 0.5   * magn;

  // Set user data
  data->M = M;

  return(0);
}

int f_attitude(sunrealtype t, N_Vector y, N_Vector ydot, void* user_data)
{
  SUNContext* sunctx;
  sunrealtype wx, wy, wz, q1, q2, q3, q4;
  UserData data;

  /* Retrieve user data */
  data  = (UserData)user_data;
  sunctx = data->sunctx;
  
  wx = Ith(y, INIT_SLICE_ATTITUDE + 0);
  wy = Ith(y, INIT_SLICE_ATTITUDE + 1);
  wz = Ith(y, INIT_SLICE_ATTITUDE + 2);
  q1 = Ith(y, INIT_SLICE_ATTITUDE + 3);
  q2 = Ith(y, INIT_SLICE_ATTITUDE + 4);
  q3 = Ith(y, INIT_SLICE_ATTITUDE + 5);
  q4 = Ith(y, INIT_SLICE_ATTITUDE + 6);

  // Compute eddy current torque
  N_Vector T_eddy = eddy_current_torque(*sunctx, y, data);

  Ith(ydot, INIT_SLICE_ATTITUDE + 0) = (Ith(T_eddy, 0) - (data->debris_Izz - data->debris_Iyy) * wz * wy) / data->debris_Ixx;
  Ith(ydot, INIT_SLICE_ATTITUDE + 1) = (Ith(T_eddy, 1) - (data->debris_Ixx - data->debris_Izz) * wx * wz) / data->debris_Iyy;
  Ith(ydot, INIT_SLICE_ATTITUDE + 2) = (Ith(T_eddy, 2) - (data->debris_Iyy - data->debris_Ixx) * wy * wx) / data->debris_Izz;
  Ith(ydot, INIT_SLICE_ATTITUDE + 3) = SUN_RCONST(0.5) * (wz * q2 - wy * q3 + wx * q4);
  Ith(ydot, INIT_SLICE_ATTITUDE + 4) = SUN_RCONST(0.5) * (-wz * q1 + wx * q3 + wy * q4);
  Ith(ydot, INIT_SLICE_ATTITUDE + 5) = SUN_RCONST(0.5) * (wy * q1 - wx * q2 + wz * q4);
  Ith(ydot, INIT_SLICE_ATTITUDE + 6) = SUN_RCONST(0.5) * (-wx * q1 - wy * q2 - wz * q3);

  return (0);
}

N_Vector get_magnetic_field(N_Vector y, SUNContext sunctx, UserData user_data) {
  /* Compute magnetic field */
  // Vectorial component
  N_Vector B = N_VNew_Serial(3, sunctx);

  // Extract end effector location and save it to additional values
  N_Vector location = end_effector_position(y, sunctx, user_data);
  for (size_t i = 0; i < 3; i++)
  {
    Ith(user_data->additional, EE_LOC_INIT_SLICE + i) = Ith(location, i);
  }
  
  // Extract end effector pose and save it to additional values
  N_Vector moment = end_effector_pose(y, sunctx, user_data);
  for (size_t i = 0; i < 3; i++)
  {
    Ith(user_data->additional, EE_POS_INIT_SLICE + i) = Ith(moment, i);
  }

  // Scalar components
  sunrealtype mag = (MU0 * user_data->mag_n_turns * user_data->mag_current * (PI*pow(user_data->mag_radius, 2))) / (4 * PI);
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
  N_Vector B = get_magnetic_field(y, sunctx, user_data);

  // Get magnetic tensor
  SUNMatrix M = user_data->M;

  // Extract end effector linear and angular velocity and save it to additional values
  N_Vector linang_vel = end_effector_linang_velocity(y, sunctx, user_data);
  for (size_t i = 0; i < 6; i++)
  {
    Ith(user_data->additional, EE_LAV_INIT_SLICE + i) = Ith(linang_vel, i);
  }

  // Extract relative angular velocity
  N_Vector wr = N_VNew_Serial(3, sunctx);  // ASSUMPTION: no chaser contribution to wr (done, check and remove comment)
  N_Vector tmp = N_VNew_Serial(3, sunctx);
  for (size_t i = 0; i < 3; i++) { Ith(wr, i) = Ith(y, INIT_SLICE_ATTITUDE + i) - Ith(linang_vel, i + 3); }

  // Compute torque
  // Compute cross product: cross(wr, B)
  wr = cross(sunctx, wr, B);
  
  // Perform the matrix-vector multiplication: M * temp1
  SUNMatMatvecSetup(M);
  SUNMatMatvec(M, wr, tmp);

  // Torque, perform the cross product: cross(M * cross(wr, B), B)
  N_Vector T = cross(sunctx, tmp, B);

  // Save EC torque to additional values
  for (size_t i = 0; i < 3; i++)
  {
    Ith(user_data->additional, EE_ECT_INIT_SLICE + i) = Ith(T, i);
  }

  // Free memory
  N_VDestroy(B);
  N_VDestroy(wr);
  N_VDestroy(tmp);

  return T;
}

