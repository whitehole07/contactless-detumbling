#include <sunmatrix/sunmatrix_dense.h>
#include <nvector/nvector_serial.h>
#include <vector>

#include "robotic_arms.h"
#include "attitude.h"
#include "orbit.h"

#ifndef PROPAGATOR_H
#define PROPAGATOR_H

/* User-defined vector and matrix accessor macros: Ith, IJth */
#define Ith(v, i)      NV_Ith_S(v, i)         /* i-th vector component i=1..NEQ */
#define IJth(A, i, j)  SM_ELEMENT_D(A, i, j)  /* (i,j)-th matrix component i,j=1..NEQ */

// Integrator configuration
#define NEQ                   NEQ_ATTITUDE + NEQ_MANIP + NEQ_ORBIT
#define INIT_SLICE_MANIP      0
#define INIT_SLICE_ATTITUDE   NEQ_MANIP
#define INIT_SLICE_ORBIT      NEQ_MANIP + NEQ_ATTITUDE

using namespace std;

/* User data */
/* Init additional values to be saved 
    3x - end effector position
    3x - end effector pose
    6x - joint torques
    6x - linear and angular velocity
    3x - eddy current torque
    3x - relative angular velocity in LVLH frame
    3x - magnetic field in the body frame
*/
#define ADDITIONAL_SIZE   27
#define EE_LOC_INIT_SLICE 0
#define EE_POS_INIT_SLICE 3
#define EE_TOR_INIT_SLICE 6
#define EE_LAV_INIT_SLICE 12
#define EE_ECT_INIT_SLICE 18
#define EE_WLV_INIT_SLICE 21
#define EE_MAG_INIT_SLICE 24

typedef struct function_data
{ 
  SUNContext* sunctx;
  N_Vector additional;
  vector<double> y0;  // Initial conditions

  // Attitude (debris)
  double debris_Ixx, debris_Iyy, debris_Izz;
  double Mxx, Myy, Mzz;

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
                double Mxx, double Myy, double Mzz,
                 double mag_n_turns, double mag_current, double mag_radius, double base_to_body_x,
                  double base_to_body_y, double base_to_body_z, vector<double> dh_a,
                   vector<double> dh_d, vector<double> dh_alpha, vector<double> tau_max, vector<vector<double>> com);


    // Initializer
    void initialize();

    // Method to reset the environment
    void reset();

    // Method to reset the environment with custom y0
    void reset_change_y0(vector<double> y0);

    // Compute control torque
    void set_control_torque(vector<double> yD);

    // Disabe torque
    void unset_control_torque();

    // Return current state
    tuple<double, vector<double>> current_state();

    // Method to execute one step in the environment
    tuple<int, double, vector<double>> step(double t_step);

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