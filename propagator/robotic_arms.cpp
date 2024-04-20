#include <iostream>
#include <fstream>

/* Sundials */
#include <cvode/cvode.h>
#include <nvector/nvector_serial.h>
#include <sunlinsol/sunlinsol_dense.h>
#include <sunmatrix/sunmatrix_dense.h>

/* SymEngine */
#include <symengine/symengine_config.h>
#include <symengine/parser.h>
#include <symengine/expression.h>
#include <symengine/symbol.h>
#include <symengine/symengine_casts.h>
#include <symengine/matrix.h>
#include <symengine/simplify.h>

/* RapidJSON */
#include "rapidjson/document.h"

/* User-defined vector and matrix accessor macros: Ith, IJth */
#define Ith(v, i) NV_Ith_S(v, i - 1) /* i-th vector component i=1..NEQ */
#define IJth(A, i, j) SM_ELEMENT_D(A, i - 1, j - 1) /* (i,j)-th matrix component i,j=1..NEQ */

#define T1   SUN_RCONST(0.0)
#define T2   SUN_RCONST(0.0)
#define T3   SUN_RCONST(0.0)
#define T4   SUN_RCONST(0.0)
#define T5   SUN_RCONST(0.0)
#define T6   SUN_RCONST(0.0)
#define DT1  SUN_RCONST(0.0)
#define DT2  SUN_RCONST(0.0)
#define DT3  SUN_RCONST(0.0)
#define DT4  SUN_RCONST(0.0)
#define DT5  SUN_RCONST(0.0)
#define DT6  SUN_RCONST(0.0)

#define RTOL  SUN_RCONST(1.0e-4)
#define ATOL  SUN_RCONST(1.0e-8)

#define INIT_TIMET0  SUN_RCONST(0.0)
#define END_TIME     SUN_RCONST(20.0)

/* Define UserData */
typedef struct
{
  /* Insert definition */;
}* UserData;

using namespace std;
using namespace SymEngine;
using namespace rapidjson;

/* Functions Called by the Solver */
static int f(sunrealtype t, N_Vector y, N_Vector ydot, void* user_data);

/* Functions to handle sym matrix */
DenseMatrix subs_matrix(DenseMatrix& mat, map_basic_basic subs_values);

DenseMatrix load_matrix_from_JSON(const string& filename);

int main() {
    // Create symbols
    auto t1 = symbol("t1");
    auto t2 = symbol("t2");
    auto t3 = symbol("t3");
    auto t4 = symbol("t4");
    auto t5 = symbol("t5");
    auto t6 = symbol("t6");

    auto dt1 = symbol("dt1");
    auto dt2 = symbol("dt2");
    auto dt3 = symbol("dt3");
    auto dt4 = symbol("dt4");
    auto dt5 = symbol("dt5");
    auto dt6 = symbol("dt6");

    // Define values for x and y
    map_basic_basic values = {
        {t1, integer(1)},
        {t2, integer(1)},
        {t3, integer(1)},
        {t4, integer(1)},
        {t5, integer(1)},
        {t6, integer(1)},
        {dt1, integer(1)},
        {dt2, integer(1)},
        {dt3, integer(1)},
        {dt4, integer(1)},
        {dt5, integer(1)},
        {dt6, integer(1)}
    };

    // Load D and C matrix
    DenseMatrix D = load_matrix_from_JSON("../robotics/analytical/D.json");
    DenseMatrix C = load_matrix_from_JSON("../robotics/analytical/C.json");

    // Substitute values 
    DenseMatrix resultD = subs_matrix(D, values);
    DenseMatrix resultC = subs_matrix(C, values);

    // Print expressions
    cout << "\nD:\n" << resultD.__str__() << endl;
    cout << "\n\nC:\n" << resultC.__str__() << endl;

    return 0;
}

/* Integration functions */
static int f(sunrealtype t, N_Vector y, N_Vector ydot, void* user_data)
{
    sunrealtype wx, wy, wz, q1, q2, q3, q4;
    sunrealtype dwx, dwy, dwz, dq1, dq2, dq3, dq4;
    UserData data;

    /* Replacement array */
    map_basic_basic values = {
        {t1, integer(1)},
        {t2, integer(1)},
        {t3, integer(1)},
        {t4, integer(1)},
        {t5, integer(1)},
        {t6, integer(1)},
        {dt1, integer(1)},
        {dt2, integer(1)},
        {dt3, integer(1)},
        {dt4, integer(1)},
        {dt5, integer(1)},
        {dt6, integer(1)}
    };

    /* Retrieve user data */
    data  = (UserData)user_data;

    /* Equations of motion */
    // dy
    Ith(ydot, 1) = Ith(y, 7);
    Ith(ydot, 2) = Ith(y, 8);
    Ith(ydot, 3) = Ith(y, 9);
    Ith(ydot, 4) = Ith(y, 10);
    Ith(ydot, 5) = Ith(y, 11);
    Ith(ydot, 6) = Ith(y, 12);

    // ddy
    Ith(ydot, 7)  = 
    Ith(ydot, 8)  = 
    Ith(ydot, 9)  = 
    Ith(ydot, 10) = 
    Ith(ydot, 11) = 
    Ith(ydot, 12) = 

    return (0);
}

/* Matrix operation functions */
DenseMatrix subs_matrix(DenseMatrix& mat, map_basic_basic subs_values) {
    DenseMatrix result(mat.nrows(), mat.ncols());
    for (unsigned i = 0; i < mat.nrows(); ++i) {
        for (unsigned j = 0; j < mat.ncols(); ++j) {
            // Eval expression
            double eval = eval_double(*mat.get(i, j)->subs(subs_values));

            // Set the result in the matrix
            result.set(i, j, real_double(eval));
        }
    }

    return result;
}

DenseMatrix load_matrix_from_JSON(const string& filename) {
    // Open the JSON file
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Unable to open file " << filename << endl;
        exit(1);
    }

    // Parse the JSON document
    Document doc;
    string jsonContent((istreambuf_iterator<char>(file)), (istreambuf_iterator<char>()));
    doc.Parse(jsonContent.c_str());

    // Check if parsing succeeded
    if (doc.HasParseError()) {
        cerr << "Error: Parse error in JSON file " << filename << " at position " << doc.GetErrorOffset() << endl;
        exit(1);
    }

    // Check if the JSON document is an array
    if (!doc.IsArray()) {
        cerr << "Error: JSON file " << filename << " does not contain an array" << endl;
        exit(1);
    }

    // Get matrix dimensions
    size_t rows = doc.Size();
    size_t cols = doc[0].Size();

    // Initialize DenseMatrix
    DenseMatrix matrix(rows, cols);

    for (size_t i = 0; i < rows; ++i) {
    const Value& row = doc[i];
        for (size_t j = 0; j < row.Size(); ++j) {
            const Value& entry = row[j];
            if (entry.HasMember("symbolic_matrix") && entry["symbolic_matrix"].IsString()) {
                auto parsed_expr = parse(entry["symbolic_matrix"].GetString());
                matrix.set(i, j, parsed_expr);
            } else {
                cout << "Error: Entry at row " << i << ", column " << j << " does not contain a valid symbolic_matrix string." << endl;
                // Handle this case appropriately
            }
        }
    }
    return matrix;
}
