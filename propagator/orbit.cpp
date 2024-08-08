#include <iostream>
#include <cmath>
#include <vector>

/* Sundials */
#include <cvode/cvode.h>
#include <nvector/nvector_serial.h>
#include <sunlinsol/sunlinsol_dense.h>
#include <sunmatrix/sunmatrix_dense.h>

/* Local */
#include "orbit.h"
#include "attitude.h"
#include "robotic_arms.h"
#include "propagator.h"

using namespace std;


SUNMatrix LVLH_to_inertia(N_Vector y, SUNContext sunctx);


int initiate_orbit(SUNContext sunctx, N_Vector y, void* user_data) {

  // Get user data
  UserData data = (UserData)user_data;

  /* Initialize y_orbit */
  for (size_t i = 0; i < NEQ_ORBIT; i++)
  {
    Ith(y, INIT_SLICE_ORBIT + i) = data->y0[INIT_SLICE_ORBIT + i];
  }

  return(0);
}

int f_orbit(sunrealtype t, N_Vector y, N_Vector ydot, void* user_data)
{
  SUNContext* sunctx;
  UserData data;

  /* Retrieve user data */
  data  = (UserData)user_data;
  sunctx = data->sunctx;

  // Extract position
  N_Vector rv = N_VNew_Serial(3, *sunctx);
  for (size_t i = 0; i < 3; i++)
    {
        Ith(rv, i) = Ith(y, INIT_SLICE_ORBIT + i);
    }

  // Compute position norm
  double r = sqrt(N_VDotProd(rv, rv));

  // Compute magnitude factor
  double mag = -(MU/pow(r, 3));

  Ith(ydot, INIT_SLICE_ORBIT + 0) = Ith(y, INIT_SLICE_ORBIT + 3);
  Ith(ydot, INIT_SLICE_ORBIT + 1) = Ith(y, INIT_SLICE_ORBIT + 4);
  Ith(ydot, INIT_SLICE_ORBIT + 2) = Ith(y, INIT_SLICE_ORBIT + 5);
  Ith(ydot, INIT_SLICE_ORBIT + 3) = mag * Ith(rv, 0);
  Ith(ydot, INIT_SLICE_ORBIT + 4) = mag * Ith(rv, 1);
  Ith(ydot, INIT_SLICE_ORBIT + 5) = mag * Ith(rv, 2);

  // Clean up
  N_VDestroy(rv);

  return(0);
}

SUNMatrix LVLH_to_inertia(N_Vector y, SUNContext sunctx) {

    // Extract position and velocity
    N_Vector position = N_VNew_Serial(3, sunctx);
    N_Vector velocity = N_VNew_Serial(3, sunctx);
    for (size_t i = 0; i < 3; i++)
    {
        Ith(position, i) = Ith(y, i + INIT_SLICE_ORBIT);
        Ith(velocity, i) = Ith(y, i + INIT_SLICE_ORBIT + 3);
    }

    // Normalize the position vector to get x_LVLH
    double pos_norm = sqrt(N_VDotProd(position, position));
    N_VScale(1.0 / pos_norm, position, position);

    // Compute the cross product of position and velocity to get z_LVLH (normal to orbital plane)
    N_Vector z_LVLH = cross(sunctx, position, velocity);
    double z_norm = sqrt(N_VDotProd(z_LVLH, z_LVLH));
    N_VScale(1.0 / z_norm, z_LVLH, z_LVLH);

    // Compute the cross product of z_LVLH and x_LVLH to get y_LVLH
    double vel_norm = sqrt(N_VDotProd(velocity, velocity));
    N_VScale(1.0 / vel_norm, velocity, velocity);

    // Create a 3x3 SUNMatrix
    SUNMatrix R_LVLH_to_inertial = SUNDenseMatrix(3, 3, sunctx);
    
    // Fill the SUNMatrix with the rotation matrix elements
    IJth(R_LVLH_to_inertial, 0, 0) = Ith(position, 0);
    IJth(R_LVLH_to_inertial, 0, 1) = Ith(velocity, 0);
    IJth(R_LVLH_to_inertial, 0, 2) = Ith(z_LVLH, 0);
    IJth(R_LVLH_to_inertial, 1, 0) = Ith(position, 1);
    IJth(R_LVLH_to_inertial, 1, 1) = Ith(velocity, 1);
    IJth(R_LVLH_to_inertial, 1, 2) = Ith(z_LVLH, 1);
    IJth(R_LVLH_to_inertial, 2, 0) = Ith(position, 2);
    IJth(R_LVLH_to_inertial, 2, 1) = Ith(velocity, 2);
    IJth(R_LVLH_to_inertial, 2, 2) = Ith(z_LVLH, 2);

    // Clean up
    N_VDestroy(position);
    N_VDestroy(velocity);
    N_VDestroy(z_LVLH);

    return R_LVLH_to_inertial;
}