#include <iostream>
#include <cstdlib>
#include <cmath>
#include <omp.h>

using namespace std;



void matmulSerial( double *a, double *b, double *c, int n) {
    for ( int i = 0; i < n; i++) {

        for (int j = 0; j < n; j++) {

            double sum = 0.0;
            for (int k = 0; k < n; k++) {

                sum += a[i * n + k] * b[k * n + j];
            }

            c[i * n + j] = sum;
        }

    }

}



void matmulOuter( double *a, double *b, double *c, int n) {
    #pragma omp parallel for
    for (int i = 0; i < n; i++) {

        for (int j = 0; j < n; j++) {

            double sum = 0.0;
            for (int k = 0; k < n; k++) {

                sum += a[i * n + k] * b[k * n + j];
            }

            c[i * n + j] = sum;
        }
    }


}


void matmulCollapse( double *a, double *b, double *c, int n) {

    #pragma omp parallel for collapse(2)
    for (int i = 0; i < n; i++) {

        for (int j = 0; j < n; j++) {

            double sum = 0.0;
            for (int k = 0; k < n; k++) {

                sum += a[i * n + k] * b[k * n + j];
            }


            c[i * n + j] = sum;
        }
    }

}


int main() {
    
    int sizes[] = {256, 512, 1024};
    int numSizes = 3;

    
    int numThreads = 1;
    #pragma omp parallel
    {

        #pragma omp single
        numThreads = omp_get_num_threads();

    }



    cout << "\n========================================\n";
    cout << "Running with OMP_NUM_THREADS = " << numThreads << endl;
    cout << "========================================\n";

    for ( int s = 0; s < numSizes; s++) {

        int n = sizes[s];
        cout << "\n----------------------------------------\n";
        cout << "Testing N = " << n << " x " << n << endl;

        long total = (long)n * n;
        double *a = new double[total];
        double *b = new double[total];
        double *cSerial = new double[total];
        double *cOuter = new double[total];
        double *cCollapse = new double[total];

       
        for ( long i = 0; i < total; i++) {

            a[i] = 1.0;
            b[i] = 2.0;
            cSerial[i] = 0.0;
            cOuter[i] = 0.0;
            cCollapse[i] = 0.0;


        }

        double t1 = omp_get_wtime();
        matmulSerial( a, b, cSerial, n);
        double t2 = omp_get_wtime();
        double serialTime = t2 - t1;
        cout << "Serial Time:       " << serialTime << "s\n";

       
        t1 = omp_get_wtime();
        matmulOuter(a, b, cOuter, n);
        t2 = omp_get_wtime();
        double outerTime = t2 - t1;
        double outerSpeedup = (outerTime > 0.0) ? (serialTime / outerTime) : 0.0;
        double outerEfficiency = outerSpeedup / numThreads;
        cout << "Outer-Loop Time:   " << outerTime << "s | Speedup: " << outerSpeedup << " | Efficiency: " << outerEfficiency << "\n";

      
        t1 = omp_get_wtime();
        matmulCollapse(a, b, cCollapse, n);
        t2 = omp_get_wtime();
        double collapseTime = t2 - t1;
        double collapseSpeedup = (collapseTime > 0.0) ? (serialTime / collapseTime) : 0.0;
        double collapseEfficiency = collapseSpeedup / numThreads;
        cout << "Collapse(2) Time:  " << collapseTime << "s | Speedup: " << collapseSpeedup << " | Efficiency: " << collapseEfficiency << "\n";

       
        bool success = true;
        double tolerance = 1e-5;
        for ( long i = 0; i < total; i++) {

            if (abs(cSerial[i] - cOuter[i]) > tolerance || abs(cSerial[i] - cCollapse[i]) > tolerance) {
                success = false;
                break;
            }

        }

        if ( success) {

            cout << "Verification:      SUCCESS\n";
        } 
        
        else {
            cout << "Verification:      FAILED\n";

        }

        delete[] a;
        delete[] b;
        delete[] cSerial;
        delete[] cOuter;
        delete[] cCollapse;


    }


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