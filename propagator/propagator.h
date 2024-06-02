#include <sunmatrix/sunmatrix_dense.h>
#include <nvector/nvector_serial.h>
#include <vector>

#include "robotic_arms.h"
#include "attitude.h"

#ifndef PROPAGATOR_H
#define PROPAGATOR_H

/* User-defined vector and matrix accessor macros: Ith, IJth */
#define Ith(v, i)      NV_Ith_S(v, i)         /* i-th vector component i=1..NEQ */
#define IJth(A, i, j)  SM_ELEMENT_D(A, i, j)  /* (i,j)-th matrix component i,j=1..NEQ */

// Integrator configuration
#define NEQ                   NEQ_ATTITUDE + NEQ_MANIP
#define INIT_SLICE_MANIP      0
#define INIT_SLICE_ATTITUDE   NEQ_MANIP

using namespace std;

/* User data */
/* Init additional values to be saved 
    3x - end effector position
    3x - end effector pose
    6x - joint torques
    6x - linear and angular velocity
*/
#define ADDITIONAL_SIZE   18
#define EE_LOC_INIT_SLICE 0
#define EE_POS_INIT_SLICE 3
#define EE_TOR_INIT_SLICE 6
#define EE_LAV_INIT_SLICE 12

typedef struct function_data
{ 
  SUNContext* sunctx;
  N_Vector additional;
  vector<double> y0;  // Initial conditions

  // Attitude (debris)
  double debris_Ixx, debris_Iyy, debris_Izz;
  double debris_radius, debris_height, debris_thick, debris_sigma;
  SUNMatrix M;

  // Magnet
  double mag_n_turns, mag_current, mag_radius;

  // Robotic arm
  double base_to_body_x, base_to_body_y, base_to_body_z;
  vector<double> dh_a, dh_d, dh_alpha;
  SUNMatrix Dv, Cv;
  vector<vector<double>> com;

  // Control
  N_Vector yD;
  vector<double> tau_max;
  bool control;

}* UserData;


class Environment {
public:

    SUNContext sunctx;
    
    // Constructor
    Environment(vector<double> y0, double debris_Ixx, double debris_Iyy, double debris_Izz, 
                double debris_radius, double debris_height, double debris_thick, double debris_sigma,
                 double mag_n_turns, double mag_current, double mag_radius, double base_to_body_x,
                  double base_to_body_y, double base_to_body_z, vector<double> dh_a,
                   vector<double> dh_d, vector<double> dh_alpha, vector<double> tau_max, vector<vector<double>> com);


    // Initializer
    void initialize();

    // Method to reset the environment
    void reset();

    // Compute control torque
    void set_control_torque(vector<double> yD);

    // Disabe torque
    void unset_control_torque();

    // Return current state
    tuple<double, vector<double>> current_state();

    // Method to execute one step in the environment
    tuple<double, vector<double>> step(double t_step);

private:
    // Define class variables
    UserData user_data;
    sunrealtype t, tout;
    N_Vector abstol;
    N_Vector y;
    SUNMatrix A;
    SUNLinearSolver LS;
    int iout;
    void* cvode_mem;
};

#endif