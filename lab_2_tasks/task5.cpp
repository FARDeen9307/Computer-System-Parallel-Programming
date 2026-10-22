#include <iostream>
#include <cstdlib>
#include <omp.h>

using namespace std;

// 1. Serial 2D 
void conv2dSer( long *img, long *out, long rows, long cols, double *filter) {

    long outRows = rows - 2;
    long outCols = cols - 2;

    for ( long i = 0; i < outRows; i++) {

        for (long j = 0; j < outCols; j++) {

            double sum = 0.0;
            for (long fi = 0; fi < 3; fi++) {

                for (long fj = 0; fj < 3; fj++) {

                    sum += img[(i + fi) * cols + (j + fj)] * filter[fi * 3 + fj];
                }

            }

            out[i * outCols + j] = (long)sum;
        }
    }

}


void conv2dPar( long *img, long *out, long rows, long cols, double *filter) {

    long outRows = rows - 2;
    long outCols = cols - 2;

    #pragma omp parallel for
    for ( long i = 0; i < outRows; i++) {

        for (long j = 0; j < outCols; j++) {

            double sum = 0.0;
            for (long fi = 0; fi < 3; fi++) {

                for ( long fj = 0; fj < 3; fj++) {

                    sum += img[(i + fi) * cols + (j + fj)] * filter[fi * 3 + fj];
                }

            }
            out[i * outCols + j] = (long)sum;
        }
    }

}

int main() {
    long rows = 4000;
    long cols = 4000;
    long totalPixels = rows * cols;
    long outRows = rows - 2;
    long outCols = cols - 2;
    long totalOutPixels = outRows * outCols;


    cout << "\n----------------------------------------\n";
    cout << "Testing 2D Convolution: " << rows << " x " << cols << " Image" << endl;


    long *img = new long[totalPixels];
    long *outSer = new long[totalOutPixels];
    long *outPar = new long[totalOutPixels];

    // Initialize image
    for (long i = 0; i < totalPixels; i++) {
        img[i] = i % 256;
    }


    double filter[9];
    for (int i = 0; i < 9; i++) {

        filter[i] = 1.0 / 9.0;
    }

    
    double t1 = omp_get_wtime();
    conv2dSer( img, outSer, rows, cols, filter);
    double t2 = omp_get_wtime();
    double serialTime = t2 - t1;
    cout << "Serial Time:   " << serialTime << "s\n";

   
    t1 = omp_get_wtime();
    conv2dPar(img, outPar, rows, cols, filter);
    t2 = omp_get_wtime();
    double parallelTime = t2 - t1;
    cout << "Parallel Time: " << parallelTime << "s\n";

    
    double speedup = 0.0;
    if ( parallelTime > 0.0) {

        speedup = serialTime / parallelTime;
    }

    cout << "Speedup (Par):  " << speedup << endl;

 
    bool isMatch = true;
    for ( long i = 0; i < totalOutPixels; i++) {

        if ( outSer[i] != outPar[i]) {

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

    delete[] img;
    delete[] outSer;
    delete[] outPar;

    return 0;

}

/*
g++ -fopenmp -O2 task5.cpp -o task5


$env:OMP_NUM_THREADS = "1"
./task5


$env:OMP_NUM_THREADS = "2"
./task5


$env:OMP_NUM_THREADS = "4"
./task5


$env:OMP_NUM_THREADS = "8"
./task5
*/