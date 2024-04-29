#include <iostream>
#include <vector>

#include <sunmatrix/sunmatrix_dense.h>
#include <nvector/nvector_serial.h>

#define Ith(v, i)      NV_Ith_S(v, i)         /* i-th vector component i=1..NEQ */
#define IJth(A, i, j)  SM_ELEMENT_D(A, i, j)  /* (i,j)-th matrix component i,j=1..NEQ */

using namespace std;

N_Vector solve_linear_system(SUNMatrix As, N_Vector bs, SUNContext sunctx) {
    // ONLY SQUARE MATRICES
    // Define
    int n = NV_LENGTH_S(bs);
    vector<vector<double>> A(n, vector<double>(n, 0.0));
    vector<double> b(n, 0.0);

    // Convert
    for (size_t i = 0; i < n; i++)
    {
        for (size_t j = 0; j < n; j++)
        {
            A[i][j] = IJth(As, i, j);
        }
        b[i] = Ith(bs, i);
    }

    n = A.size();
    vector<vector<double>> augmentedMatrix = A;
    for (size_t i = 0; i < A.size(); ++i) {
        augmentedMatrix[i].push_back(b[i]);
    }

    // Gaussian elimination
    for (int i = 0; i < n; ++i) {
        int max_row = i;
        for (int j = i + 1; j < n; ++j) {
            if (abs(augmentedMatrix[j][i]) > abs(augmentedMatrix[max_row][i])) {
                max_row = j;
            }
        }
        if (max_row != i) {
            swap(augmentedMatrix[i], augmentedMatrix[max_row]);
        }
        for (int j = i + 1; j < n; ++j) {
            double factor = augmentedMatrix[j][i] / augmentedMatrix[i][i];
            for (int k = i; k <= n; ++k) {
                augmentedMatrix[j][k] -= factor * augmentedMatrix[i][k];
            }
        }
    }

    // Back substitution
    vector<double> x(n);
    for (int i = n - 1; i >= 0; --i) {
        x[i] = augmentedMatrix[i][n];
        for (int j = i + 1; j < n; ++j) {
            x[i] -= augmentedMatrix[i][j] * x[j];
        }
        x[i] /= augmentedMatrix[i][i];
    }

    // Convert
    N_Vector xc = N_VNew_Serial(n, sunctx);
    for (size_t i = 0; i < n; i++)
    {
        Ith(xc, i) = x[i];
    }
    
    return xc;
}

int main() {
    // Example usage
    SUNContext sunctx;

    vector<vector<double>> Av = {{2, 1, -1, 2, 3, 4},
                                 {-3, -1, 2, 2, 3, 4},
                                 {-2, 1, 2, 2, 3, 4},
                                 {-5, 1, -2, 7, 6, 4},
                                 {-8, 1, 3, 2, -3, 9},
                                 {-2, 1, -2, 2, 8, 4}};
    vector<double> bv = {8, -11, -3, 2, 3, 4};


    SUNMatrix A = SUNDenseMatrix(6, 6, sunctx);
    for (size_t i = 0; i < 6; i++)
    {
        for (size_t j = 0; j < 6; j++)
        {
            IJth(A, i, j) = Av[i][j];
        }
    }

    N_Vector b = N_VNew_Serial(6, sunctx);
    for (size_t i = 0; i < 6; i++)
    {
        Ith(b, i) = bv[i];
    }
    
    N_Vector x = solve_linear_system(A, b, sunctx);

    cout << "Solution to the linear system Ax = b:" << endl;
    for (size_t i = 0; i < NV_LENGTH_S(x); ++i) {
        cout << "x[" << i << "] = " << Ith(x, i) << endl;
    }

    return 0;
}
