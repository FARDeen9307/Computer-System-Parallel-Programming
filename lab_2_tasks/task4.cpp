#include <iostream>
#include <cstdlib>
#include <omp.h>

using namespace std;

void transposeSer( long *matrix, long *transposed, long rows, long cols) {
    for ( long i = 0; i < rows; i++) {

        for (long j = 0; j < cols; j++) {

            transposed[j * rows + i] = matrix[i * cols + j];
        }
    }

}


void transposePar( long *matrix, long *transposed, long rows, long cols) {

    #pragma omp parallel for
    for ( long i = 0; i < rows; i++) {

        for ( long j = 0; j < cols; j++) {

            transposed[j * rows + i] = matrix[i * cols + j];
        }
    }
}

int main() {

    long rows = 10000;
    long cols = 10000;
    long totalElements = rows * cols;

    cout << "\n----------------------------------------\n";
    cout << "Testing Matrix Transpose: " << rows << " x " << cols << endl;

    long *matrix = new long[totalElements];
    long *transSer = new long[totalElements];
    long *transPar = new long[totalElements];


    for ( long i = 0; i < totalElements; i++) {

        matrix[i] = i;

    }

   
    double t1 = omp_get_wtime();
    transposeSer( matrix, transSer, rows, cols);

    double t2 = omp_get_wtime();

    double serialTime = t2 - t1;

    cout << "Serial Time:   " << serialTime << "s\n";

   
    t1 = omp_get_wtime();
    transposePar(matrix, transPar, rows, cols);
    t2 = omp_get_wtime();
    double parallelTime = t2 - t1;
    cout << "Parallel Time: " << parallelTime << "s\n";


   
    double speedup = 0.0;
    if ( parallelTime > 0.0) {

        speedup = serialTime / parallelTime;
    }

    cout << "Speedup (Par):  " << speedup << endl;

    
    bool isMatch = true;
    for ( long i = 0; i < totalElements; i++) {

        if (transSer[i] != transPar[i]) {

            isMatch = false;
            break;

        }
    }

    if ( isMatch) {

        cout << "Verification:   SUCCESS\n";
    } 
    
    else {
        cout << "Verification:   FAILED\n";

    }

    delete[] matrix;
    delete[] transSer;
    delete[] transPar;

    return 0;

}


/*
g++ -fopenmp -O2 task4.cpp -o task4
$env:OMP_NUM_THREADS = "1"
./task4
$env:OMP_NUM_THREADS = "2"
./task4
$env:OMP_NUM_THREADS = "4"
./task4
$env:OMP_NUM_THREADS = "8"
./task4    
*/