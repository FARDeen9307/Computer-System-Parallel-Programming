#include <iostream>
#include <omp.h>

using namespace std;

int main() {
    const int N = 3;
    int A[N][N] = {
        {1, 2, 3},
        {4, 5, 6},
        {7, 8, 9}
    };
    int B[N][N] = {
        {1, 0, 0},
        {0, 1, 0},
        {0, 0, 1}
    };
    int C[N][N] = {0};

    // Parallelize the outermost loop (row-wise distribution)
    #pragma omp parallel for
    for (int i = 0; i < N; ++i) {
        int tid = omp_get_thread_num();

        for (int j = 0; j < N; ++j) {
            int sum = 0;
            for (int k = 0; k < N; ++k) {
                sum += A[i][k] * B[k][j];
            }
            C[i][j] = sum;
        }

        #pragma omp critical
        {
            cout << "Row " << i << " of result calculated by Thread " << tid << endl;
        }
    }

    // Print resulting matrix
    cout << "\nResult of Matrix Multiplication:" << endl;
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            cout << C[i][j] << " ";
        }
        cout << endl;
    }

    return 0;
}