#include <iostream>
#include <fstream>

/* JSON.hpp */
#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

using json = nlohmann::json;

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

/* RapidJSON */
#include "rapidjson/document.h"

/* Local */
#include "robotic_arms.h"
#include "propagator.h"

using namespace std;
using namespace SymEngine;
using namespace rapidjson;

#define Ith(v, i)      NV_Ith_S(v, i)         /* i-th vector component i=1..NEQ */
#define IJth(A, i, j)  SM_ELEMENT_D(A, i, j)  /* (i,j)-th matrix component i,j=1..NEQ */

SUNMatrix DenseMatrixToSUNMatrix(const DenseMatrix matrix, SUNContext sunctx);

/* Functions to handle sym matrix */
DenseMatrix subs_matrix(DenseMatrix& mat, vector<RCP<const Symbol>> symbols, vector<RCP<const Basic>> sub_vals);

DenseMatrix parse_symbolic_matrix(const string &filename);

SUNMatrix init(SUNContext sunctx){

    SUNMatrix Dv;
    vector<RCP<const Basic>> sub_vals(12);

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

    for (size_t i = 0; i < 12; i++) { sub_vals[i] = real_double(1.0); }

    DenseMatrix D = parse_symbolic_matrix("D.txt");
    D = subs_matrix(D, symbols, sub_vals);
    Dv = DenseMatrixToSUNMatrix(D, sunctx);

    return Dv;
}

int main() {
    SUNContext sunctx;

    N_Vector y_s;
    // SUNLinearSolver LS;
    SUNLinearSolver LS = SUNLinSolNewEmpty(sunctx);

    SUNContext_Create(SUN_COMM_NULL, &sunctx);

    SUNMatrix T = SUNDenseMatrix(6, 6, sunctx);
    for (size_t i = 0; i < 6; i++)
    {
        for (size_t j = 0; j < 6; j++)
        {
            IJth(T, i, j) = 1;
        }
    }

    y_s = N_VNew_Serial(6, sunctx);
    for (size_t i = 0; i < 6; i++) { Ith(y_s, i) = 0.0; }

    // Convert
    SUNMatrix D = init(sunctx);

    LS = SUNLinSol_Dense(y_s, T, sunctx);
    int retval = SUNLinSolSolve(LS, T, y_s, y_s, 0.0);

    // Output the solution
    cout << "Solution: x1 = " << Ith(y_s, 0) << ", x2 = " << Ith(y_s, 1) << endl;

    // Clean up
    SUNLinSolFree(LS);
    // SUNMatDestroy(Dv);
    SUNMatDestroy(T);
    N_VDestroy_Serial(y_s);

    return 0;
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

    std::cout << "Matrix:" << std::endl;
    // Iterate over each row
    for (sunindextype i = 0; i < nrows; ++i) {
        // Iterate over each column
        for (sunindextype j = 0; j < ncols; ++j) {
            // Retrieve the matrix element at row i, column j
            sunrealtype value;
            value = IJth(sunMatrix, i, j);
            // Print the matrix element
            std::cout << value << "\t";
        }
        // Move to the next row
        std::cout << std::endl;
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

// Function to parse a symbolic matrix from a .txt file
DenseMatrix parse_symbolic_matrix(const string &filename) {
    /*ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Unable to open file " << filename << endl;
        return DenseMatrix(0, 0); // Return an empty matrix on error
    }

    // Variables to store matrix dimensions and data
    vector<vector<RCP<const Basic>>> data;
    string line;

    // Read the matrix data from the file
    while (getline(file, line, ';')) {
        istringstream line_stream(line);
        vector<RCP<const Basic>> row;
        string token;
        while (getline(line_stream, token, ',')) {
            // Parse token into a symbolic expression and add it to the row
            RCP<const Basic> expr = parse(token);
            row.push_back(expr);
        }
        data.push_back(row);
    }

    // Get the dimensions of the matrix
    size_t rows = data.size();
    size_t cols = (rows > 0) ? data[0].size() : 0;

    // Construct DenseMatrix from parsed data
    DenseMatrix matrix(rows, cols);
    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < cols; ++j) {
            matrix.set(i, j, data[i][j]);
        }
    }

    return matrix;*/
    return DenseMatrix();
}