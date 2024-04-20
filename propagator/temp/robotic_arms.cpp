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
#define Ith(v, i) NV_Ith_S(v, i)             /* i-th vector component i=1..NEQ */
#define IJth(A, i, j) SM_ELEMENT_D(A, i, j)  /* (i,j)-th matrix component i,j=1..NEQ */

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
    SUNContext sunctx;
    vector<RCP<const Symbol>> symbols;
    DenseMatrix D;
    DenseMatrix C;
}* UserData;

using namespace std;
using namespace SymEngine;
using namespace rapidjson;

/* Functions Called by the Solver */
static int f(sunrealtype t, N_Vector y, N_Vector ydot, void* user_data);

SUNMatrix DenseMatrixToSUNMatrix(const DenseMatrix matrix, SUNContext sunctx);

/* Functions to handle sym matrix */
DenseMatrix subs_matrix(DenseMatrix& mat, vector<RCP<const Symbol>> symbols, vector<RCP<const Basic>> sub_vals);

DenseMatrix load_matrix_from_JSON(const string& filename);

int main() {
    // Define problem variables
    vector<RCP<const Symbol>> symbols = {
        symbol("t1"),
        symbol("t2"),
        symbol("t3"),
        symbol("t4"),
        symbol("t5"),
        symbol("t6"),
        symbol("dt1"),
        symbol("dt2"),
        symbol("dt3"),
        symbol("dt4"),
        symbol("dt5"),
        symbol("dt6")
    };

    vector<RCP<const Basic>> sub_vals = {
        integer(1), 
        integer(1), 
        integer(1), 
        integer(1), 
        integer(1), 
        integer(1),
        integer(1),
        integer(1),
        integer(1),
        integer(1), 
        integer(1), 
        integer(1)
    };

    // Load D and C matrix
    DenseMatrix D = load_matrix_from_JSON("/home/whitehole/PycharmProjects/master_thesis/robotics/equations_of_motion/D.json");
    DenseMatrix C = load_matrix_from_JSON("/home/whitehole/PycharmProjects/master_thesis/robotics/equations_of_motion/C.json");

    // Substitute values 
    DenseMatrix resultD = subs_matrix(D, symbols, sub_vals);
    DenseMatrix resultC = subs_matrix(C, symbols, sub_vals);

    // Print expressions
    cout << "\nD:\n" << resultD.__str__() << endl;
    cout << "\n\nC:\n" << resultC.__str__() << endl;

    return 0;
}

/* Integration functions */
static int f(sunrealtype t, N_Vector y, N_Vector ydot, void* user_data)
{
    SUNContext sunctx;
    SUNMatrix Dv, Cv;
    DenseMatrix D, C;
    vector<RCP<const Symbol>> symbols;
    vector<RCP<const Basic>> sub_vals(NV_LENGTH_S(y));
    UserData data;

    /* Retrieve user data */
    data  = (UserData)user_data;
    sunctx = data->sunctx;
    symbols = data->symbols;
    D = data->D;
    C = data->C;

    /* Evaluate matrices */
    // Sub vals
    for (size_t i = 0; i < NV_LENGTH_S(y); i++) { sub_vals[i] = real_double(Ith(y, i)); }

    // Evaluate
    D = subs_matrix(D, symbols, sub_vals);
    C = subs_matrix(C, symbols, sub_vals);

    // Convert
    Dv = DenseMatrixToSUNMatrix(D, sunctx);
    Cv = DenseMatrixToSUNMatrix(C, sunctx);

    /* Computations */
    // Extract slice of y
    N_Vector y_s = ; 
    // Calculate the expression -Dv\(Cv*y(n+1:end)) + Dv\tau
    N_Vector tmp = N_VClone(y_s); // Temporary vector for intermediate result
    // Perform the operation Cv * y(n+1:end)
    SUNMatMatvec(Cv, y, tmp);
    // Perform the operation Dv * (Cv * y(n+1:end))
    SUNMatMatvec(Dv, tmp, y);

    
    /* Equations of motion */
    // dy
    Ith(ydot, 0) = Ith(y, 6);
    Ith(ydot, 1) = Ith(y, 7);
    Ith(ydot, 2) = Ith(y, 8);
    Ith(ydot, 3) = Ith(y, 9);
    Ith(ydot, 4) = Ith(y, 10);
    Ith(ydot, 5) = Ith(y, 11);

    // ddy
    Ith(ydot, 6)  = 
    Ith(ydot, 7)  = 
    Ith(ydot, 8)  = 
    Ith(ydot, 9)  = 
    Ith(ydot, 10) = 
    Ith(ydot, 11) = 

    return (0);
}

// Function to convert DenseMatrix to SUNMatrix
SUNMatrix DenseMatrixToSUNMatrix(const DenseMatrix matrix, SUNContext sunctx) {
    // Get the number of rows and columns of the DenseMatrix
    size_t nrows = matrix.nrows();
    size_t ncols = matrix.ncols();

    // Create a new SUNMatrix with the same dimensions as the DenseMatrix
    SUNMatrix sunMatrix = SUNDenseMatrix(nrows, ncols, sunctx);

    // Iterate over each element of the DenseMatrix
    for (size_t i = 0; i < nrows; ++i) {
        for (size_t j = 0; j < ncols; ++j) {
            // Set the corresponding element in the SUNMatrix
            IJth(sunMatrix, i, j) = eval_double(*matrix.get(i, j));
        }
    }

    return sunMatrix;
}

/* Matrix operation functions */
DenseMatrix subs_matrix(DenseMatrix& mat, vector<RCP<const Symbol>> symbols, vector<RCP<const Basic>> sub_vals) {
    // Create mapping
    map_basic_basic values;
    for (size_t i = 0; i < symbols.size(); ++i) {
        values[symbols[i]] = sub_vals[i];
    }

    DenseMatrix result(mat.nrows(), mat.ncols());
    for (unsigned i = 0; i < mat.nrows(); ++i) {
        for (unsigned j = 0; j < mat.ncols(); ++j) {
            // Eval expression
            double eval = eval_double(*mat.get(i, j)->subs(values));

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
