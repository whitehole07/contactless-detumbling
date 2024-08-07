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

N_Vector gravity_gradient_torque(SUNContext sunctx, N_Vector y, UserData user_data);

SUNMatrix LVLH_to_body(N_Vector y, SUNContext sunctx);

N_Vector relative_angular_velocity_body(SUNContext sunctx, N_Vector y, UserData user_data);


int initiate_attitude(SUNContext sunctx, N_Vector y, void* user_data) {

  // Get user data
  UserData data = (UserData)user_data;

  /* Initialize y_attitude */
  for (size_t i = 0; i < NEQ_ATTITUDE; i++)
  {
    Ith(y, INIT_SLICE_ATTITUDE + i) = data->y0[INIT_SLICE_ATTITUDE + i];
  }

  // Retrieve relative angular velocity in body frame
  N_Vector w_r_LVLH = relative_angular_velocity_body(sunctx, y, data);

  // Initialize eddy current torque
  N_Vector Tec = eddy_current_torque(sunctx, y, data);
  
  // Clean up
  N_VDestroy(Tec);
  N_VDestroy(w_r_LVLH);

  return(0);
}

int f_attitude(sunrealtype t, N_Vector y, N_Vector ydot, void* user_data)
{
  SUNContext* sunctx;
  sunrealtype wx, wy, wz, q1, q2, q3, q4, wx_r, wy_r, wz_r;
  UserData data;

  /* Retrieve user data */
  data  = (UserData)user_data;
  sunctx = data->sunctx;
  
  wx = Ith(y, INIT_SLICE_ATTITUDE + 0);
  wy = Ith(y, INIT_SLICE_ATTITUDE + 1);
  wz = Ith(y, INIT_SLICE_ATTITUDE + 2);

  // Normalize quaternions
  N_Vector q = N_VNew_Serial(4, *sunctx);
  N_Vector q_norm = N_VNew_Serial(4, *sunctx);
  Ith(q, 0) = Ith(y, INIT_SLICE_ATTITUDE + 3);
  Ith(q, 1) = Ith(y, INIT_SLICE_ATTITUDE + 4);
  Ith(q, 2) = Ith(y, INIT_SLICE_ATTITUDE + 5);
  Ith(q, 3) = Ith(y, INIT_SLICE_ATTITUDE + 6);
  N_VScale(1/sqrt(N_VDotProd(q, q)), q, q_norm);

  q1 = Ith(q_norm, 0);
  q2 = Ith(q_norm, 1);
  q3 = Ith(q_norm, 2);
  q4 = Ith(q_norm, 3);

  // Retrieve relative angular velocity in body frame
  N_Vector w_r_LVLH = relative_angular_velocity_body(*sunctx, y, data);
  wx_r = Ith(w_r_LVLH, 0);
  wy_r = Ith(w_r_LVLH, 1);
  wz_r = Ith(w_r_LVLH, 2);

  // Compute eddy current torque
  N_Vector T_eddy = eddy_current_torque(*sunctx, y, data);

  // Compute gravity gradient torque
  N_Vector T_gg = gravity_gradient_torque(*sunctx, y, data);

  Ith(ydot, INIT_SLICE_ATTITUDE + 0) = ((Ith(T_eddy, 0)) - (data->debris_Izz - data->debris_Iyy) * wz * wy) / data->debris_Ixx; //  + Ith(T_gg, 0)
  Ith(ydot, INIT_SLICE_ATTITUDE + 1) = ((Ith(T_eddy, 1)) - (data->debris_Ixx - data->debris_Izz) * wx * wz) / data->debris_Iyy; //  + Ith(T_gg, 1)
  Ith(ydot, INIT_SLICE_ATTITUDE + 2) = ((Ith(T_eddy, 2)) - (data->debris_Iyy - data->debris_Ixx) * wy * wx) / data->debris_Izz; //  + Ith(T_gg, 2)

  // Quaternions EODs
  Ith(ydot, INIT_SLICE_ATTITUDE + 3) = SUN_RCONST(0.5) * (wz_r * q2 - wy_r * q3 + wx_r * q4);
  Ith(ydot, INIT_SLICE_ATTITUDE + 4) = SUN_RCONST(0.5) * (-wz_r * q1 + wx_r * q3 + wy_r * q4);
  Ith(ydot, INIT_SLICE_ATTITUDE + 5) = SUN_RCONST(0.5) * (wy_r * q1 - wx_r * q2 + wz_r * q4);
  Ith(ydot, INIT_SLICE_ATTITUDE + 6) = SUN_RCONST(0.5) * (-wx_r * q1 - wy_r * q2 - wz_r * q3);

  // Clean up
  N_VDestroy(T_eddy);
  N_VDestroy(T_gg);
  N_VDestroy(w_r_LVLH);

  return (0);
}

N_Vector gravity_gradient_torque(SUNContext sunctx, N_Vector y, UserData user_data) {
  // Compute norm of R
  N_Vector position = N_VNew_Serial(3, sunctx);
  for (size_t i = 0; i < 3; i++)
  {
    Ith(position, i) = Ith(y, INIT_SLICE_ORBIT + i);
  }
  double r = sqrt(N_VDotProd(position, position));

  // Rotate position to LVLH
  Ith(position, 0) = 0.0;
  Ith(position, 1) = r;
  Ith(position, 2) = 0.0;

  // Rotate to body frame
  N_Vector position_body = N_VNew_Serial(3, sunctx);
  SUNMatrix R_LVLHB = LVLH_to_body(y, sunctx);
  SUNMatMatvec(R_LVLHB, position, position_body);
  N_VScale(1/r, position_body, position_body);

  // Compute gravity gradient torque
  N_Vector gg_torque = N_VNew_Serial(3, sunctx);
  double mag = 3*(MU/(pow(r, 3)));
  Ith(gg_torque, 0) = mag * (user_data->debris_Izz - user_data->debris_Iyy) * Ith(position_body, 1) * Ith(position_body, 2);
  Ith(gg_torque, 1) = mag * (user_data->debris_Ixx - user_data->debris_Izz) * Ith(position_body, 0) * Ith(position_body, 2);
  Ith(gg_torque, 2) = mag * (user_data->debris_Iyy - user_data->debris_Ixx) * Ith(position_body, 0) * Ith(position_body, 1);

  // Clean up
  N_VDestroy(position);
  N_VDestroy(position_body);
  SUNMatDestroy(R_LVLHB);

  return gg_torque;
}

N_Vector get_magnetic_field(N_Vector y, SUNContext sunctx, UserData user_data) {
  /* Compute magnetic field */
  // Vectorial component
  N_Vector B = N_VNew_Serial(3, sunctx);
  N_Vector B_body = N_VNew_Serial(3, sunctx);

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
  double mag = (MU0 * user_data->mag_n_turns * user_data->mag_current * (PI*pow(user_data->mag_radius, 2))) / (4 * PI);
  double r = sqrt(N_VDotProd(location, location));

  // Compute the dot product: dot(moment,location)
  double dot_product = N_VDotProd(moment, location);

  // Compute 3 * location
  N_VScale(3.0, location, B);

  // Perform the outer dot product: 3 * location * dot(moment,location)
  N_VScale(dot_product, B, B);

  // Perform the element-wise operations
  N_VLinearSum(1.0 / pow(r, 5), B, -1.0 / pow(r, 3), moment, B);
  N_VScale(mag, B, B);

  // Bring B to body frame
  SUNMatrix R_LVLHB = LVLH_to_body(y, sunctx);
  SUNMatMatvec(R_LVLHB, B, B_body);

  // Free the memory allocated for the N_Vector objects
  N_VDestroy(location);
  N_VDestroy(B);
  N_VDestroy(moment);
  SUNMatDestroy(R_LVLHB);

  return B_body;
}

N_Vector eddy_current_torque(SUNContext sunctx, N_Vector y, UserData user_data) {
  // Compute magnetic field
  N_Vector B = get_magnetic_field(y, sunctx, user_data);

  for (size_t i = 0; i < 3; i++)
  {
    Ith(user_data->additional, EE_MAG_INIT_SLICE + i) = Ith(B, i);
  }

  // Get magnetic tensor
  SUNMatrix M = SUNDenseMatrix(3, 3, sunctx);
  IJth(M, 0, 0) = user_data->Mxx;
  IJth(M, 1, 1) = user_data->Myy;
  IJth(M, 2, 2) = user_data->Mzz;

  // Extract end effector linear and angular velocity and save it to additional values
  N_Vector linang_vel = end_effector_linang_velocity(y, sunctx, user_data);
  for (size_t i = 0; i < 6; i++)
  {
    Ith(user_data->additional, EE_LAV_INIT_SLICE + i) = Ith(linang_vel, i);
  }

  // Extract relative angular velocity
  N_Vector wr = N_VNew_Serial(3, sunctx);
  N_Vector wr_body = N_VNew_Serial(3, sunctx);
  for (size_t i = 0; i < 3; i++) { Ith(wr, i) = Ith(user_data->additional, EE_WLV_INIT_SLICE + i); } // TODO Evaluate: - Ith(linang_vel, i + 3); }
  SUNMatrix R_LVLHB = LVLH_to_body(y, sunctx);
  SUNMatMatvec(R_LVLHB, wr, wr_body);

  // Compute torque
  // Compute cross product: cross(wr, B)
  N_Vector cross_res = cross(sunctx, wr_body, B);
  
  // Perform the matrix-vector multiplication: M * temp1
  N_Vector tmp = N_VNew_Serial(3, sunctx);
  SUNMatMatvec(M, cross_res, tmp);

  // Torque, perform the cross product: cross(M * cross(wr, B), B)
  N_Vector T = cross(sunctx, tmp, B);

  // Save EC torque to additional values
  for (size_t i = 0; i < 3; i++)
  {
    Ith(user_data->additional, EE_ECT_INIT_SLICE + i) = Ith(T, i);
  }

  // Free memory
  SUNMatDestroy(R_LVLHB);
  SUNMatDestroy(M);
  N_VDestroy(B);
  N_VDestroy(linang_vel);
  N_VDestroy(wr);
  N_VDestroy(wr_body);
  N_VDestroy(tmp);
  N_VDestroy(cross_res);

  return T;
}

// Function to convert quaternion to rotation matrix
SUNMatrix LVLH_to_body(N_Vector y, SUNContext sunctx) {
    // Retrieve current timestep quaternions from integrator
    double q1 = Ith(y, INIT_SLICE_ATTITUDE + 3 + 0);
    double q2 = Ith(y, INIT_SLICE_ATTITUDE + 3 + 1);
    double q3 = Ith(y, INIT_SLICE_ATTITUDE + 3 + 2);
    double q4 = Ith(y, INIT_SLICE_ATTITUDE + 3 + 3);

    // Initialize a 3x3 SUNMatrix
    SUNMatrix R = SUNDenseMatrix(3, 3, sunctx);

    // Fill the SUNMatrix with the rotation matrix elements
    IJth(R, 0, 0) = q1 * q1 - q2 * q2 - q3 * q3 + q4 * q4;
    IJth(R, 0, 1) = 2 * (q1 * q2 + q3 * q4);
    IJth(R, 0, 2) = 2 * (q1 * q3 - q2 * q4);
    IJth(R, 1, 0) = 2 * (q1 * q2 - q3 * q4);
    IJth(R, 1, 1) = -q1 * q1 + q2 * q2 - q3 * q3 + q4 * q4;
    IJth(R, 1, 2) = 2 * (q2 * q3 + q1 * q4);
    IJth(R, 2, 0) = 2 * (q1 * q3 + q2 * q4);
    IJth(R, 2, 1) = 2 * (q2 * q3 - q1 * q4);
    IJth(R, 2, 2) = -q1 * q1 - q2 * q2 + q3 * q3 + q4 * q4;

    return R; // THIS FUNCTION IS MOST PROBABLY WRONG!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
}

N_Vector relative_angular_velocity_body(SUNContext sunctx, N_Vector y, UserData user_data) {
  // Get rotation matrix: LVLH to Body
  SUNMatrix R_LVLHB = LVLH_to_body(y, sunctx);

  // Get rotation matrix: Body to LVLH
  SUNMatrix R_BLVLH = transpose(sunctx, R_LVLHB);

  // Retrieve orbital state
  N_Vector position = N_VNew_Serial(3, sunctx);
  N_Vector velocity = N_VNew_Serial(3, sunctx);
  for (size_t i = 0; i < 3; i++)
  {
    Ith(position, i) = Ith(y, INIT_SLICE_ORBIT + i);
    Ith(velocity, i) = Ith(y, INIT_SLICE_ORBIT + i + 3);
  }
  double r = sqrt(N_VDotProd(position, position));  // Magnitude of orbital position vector
  double v = sqrt(N_VDotProd(velocity, velocity));  // Magnitude of orbital velocity vector

  // Rotate to LVLH frame
  Ith(position, 0) = 0.0;
  Ith(position, 1) = r;
  Ith(position, 2) = 0.0;
  Ith(velocity, 0) = -v;
  Ith(velocity, 1) = 0.0;
  Ith(velocity, 2) = 0.0;

  // Compute orbital angular velocity and rotate to body frame
  N_Vector w_or = N_VNew_Serial(3, sunctx);
  N_Vector w_or_body = N_VNew_Serial(3, sunctx);
  N_Vector h = cross(sunctx, position, velocity);
  N_VScale(1/(r*r), h, w_or);
  SUNMatMatvec(R_LVLHB, w_or, w_or_body);

  // Compute relative angular velocity in the body frame and rotate back to LVLH
  N_Vector w_r = N_VNew_Serial(3, sunctx);
  N_Vector w_r_LVLH = N_VNew_Serial(3, sunctx);
  Ith(w_r, 0) = Ith(y, INIT_SLICE_ATTITUDE + 0) - Ith(w_or_body, 0);
  Ith(w_r, 1) = Ith(y, INIT_SLICE_ATTITUDE + 1) - Ith(w_or_body, 1);
  Ith(w_r, 2) = Ith(y, INIT_SLICE_ATTITUDE + 2) - Ith(w_or_body, 2);
  SUNMatMatvec(R_BLVLH, w_r, w_r_LVLH);

  // cout << "Rel BODY: " << Ith(w_r, 0) << ", " << Ith(w_r, 1) << ", " << Ith(w_r, 2) << endl;
  // cout << "Rel LVLH: " << Ith(w_r_LVLH, 0) << ", " << Ith(w_r_LVLH, 1) << ", " << Ith(w_r_LVLH, 2) << endl;

  // Save to user data
  for (size_t i = 0; i < 3; i++)
  {
    Ith(user_data->additional, EE_WLV_INIT_SLICE + i) = Ith(w_r_LVLH, i);
  }

  // Clean up
  SUNMatDestroy(R_LVLHB);
  SUNMatDestroy(R_BLVLH);
  N_VDestroy(position);
  N_VDestroy(velocity);
  N_VDestroy(w_or);
  N_VDestroy(w_or_body);
  N_VDestroy(h); 
  N_VDestroy(w_r);

  return w_r_LVLH;
}