#include <iostream>
#include <cstdlib>
#include <omp.h>

using namespace std;


void matAddSerial(double *a, double *b, double *c, int n) {
    for ( int i = 0; i < n; i++) {

        for (int j = 0; j < n; j++) {
            c[i * n + j] = a[i * n + j] + b[i * n + j];

        }
    }

}


void matAddOuter( double *a, double *b, double *c, int n) {

    #pragma omp parallel for
    for ( int i = 0; i < n; i++) {

        for (int j = 0; j < n; j++) {

            c[i * n + j] = a[i * n + j] + b[i * n + j];
        }
    }


}


void matAddCollapse( double *a, double *b, double *c, int n) {

    #pragma omp parallel for collapse(2)
    for ( int i = 0; i < n; i++) {

        for (int j = 0; j < n; j++) {

            c[i * n + j] = a[i * n + j] + b[i * n + j];
        }
    }

}

int main() {
  
    int sizes[] = {8, 4096};
    int numSizes = 2;

    for ( int s = 0; s < numSizes; s++) {

        int n = sizes[s];
        cout << "\n----------------------------------------\n";
        cout << "Testing N = " << n << endl;

        long total = (long)n * n;
        double *a = new double[total];
        double *b = new double[total];
        double *cSerial = new double[total];
        double *cOuter = new double[total];
        double *cCollapse = new double[total];

        for ( long i = 0; i < total; i++) {

            a[i] = 1.0;
            b[i] = 2.0;

        }

   
        double t1 = omp_get_wtime();
        matAddSerial ( a, b, cSerial, n);
        double t2 = omp_get_wtime();
        double serialTime = t2 - t1;
        cout << "Serial Time:       " << serialTime << "s\n";


        t1 = omp_get_wtime();
        matAddOuter(a, b, cOuter, n);
        t2 = omp_get_wtime();
        double outerTime = t2 - t1;
        double outerSpeedup = serialTime / outerTime;
        cout << "Outer-Loop Time:   " << outerTime << "s | Speedup: " << outerSpeedup << "\n";

       

        t1 = omp_get_wtime();
        matAddCollapse(a, b, cCollapse, n);
        t2 = omp_get_wtime();
        double collapseTime = t2 - t1;
        double collapseSpeedup = serialTime / collapseTime;
        cout << "Collapse(2) Time:  " << collapseTime << "s | Speedup: " << collapseSpeedup << "\n";


 
        bool success = true;
        for (long i = 0; i < total; i++) {
            if (cSerial[i] != cOuter[i] || cSerial[i] != cCollapse[i]) {
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