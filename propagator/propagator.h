#include <sunmatrix/sunmatrix_dense.h>
#include <nvector/nvector_serial.h>

#include <symengine/symbol.h>
#include <symengine/matrix.h>

#include "robotic_arms.h"
#include "attitude.h"

#ifndef PROPAGATOR_H
#define PROPAGATOR_H

/* User-defined vector and matrix accessor macros: Ith, IJth */
#define Ith(v, i)      NV_Ith_S(v, i)         /* i-th vector component i=1..NEQ */
#define IJth(A, i, j)  SM_ELEMENT_D(A, i, j)  /* (i,j)-th matrix component i,j=1..NEQ */

// Integrator configuration
#define NEQ                   NEQ_ATTITUDE
#define INIT_SLICE_ATTITUDE   0
#define INIT_SLICE_MANIP      0

using namespace std;
using namespace SymEngine;

/* User data */
typedef struct
{ 
  // Attitude
  SUNMatrix I;

  // Robotic arm
  SUNContext* sunctx;
  N_Vector tau;
}* UserData;

#endif