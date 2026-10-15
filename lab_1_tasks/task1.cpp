#include <iostream>
#include <cstdlib>
#include <omp.h>

using namespace std;

void vecAddSerial(double *a, double *b, double *c, long n) {
    for (long i = 0; i < n; i++) {
        c[i] = a[i] + b[i];
    }
}

void vecAddParallel(double *a, double *b, double *c, long n) {
    #pragma omp parallel for
    for (long i = 0; i < n; i++) {
        c[i] = a[i] + b[i];
    }

}

int main() {

    long sizes[] = {100000, 1000000, 10000000, 100000000};
    int numSizes = 4;

    for ( int s = 0; s < numSizes; s++) {
        long n = sizes[s];
        cout << "\n----------------------------------------\n";
        cout << "Testing N = " << n << endl;

        double *a = new double[n];
        double *b = new double[n];
        double *cSerial = new double[n];
        double *cParallel = new double[n];

       
        for (long i = 0; i < n; i++) {
            a[i] = i * 1.0;
            b[i] = i * 2.0;

        }

        
        double t1 = omp_get_wtime();
        vecAddSerial(a, b, cSerial, n);
        double t2 = omp_get_wtime();
        double serialTime = t2 - t1;
        cout << "Serial Time:   " << serialTime << " seconds\n";

        

        t1 = omp_get_wtime();
        vecAddParallel(a, b, cParallel, n);
        t2 = omp_get_wtime();
        double parallelTime = t2 - t1;
        cout << "Parallel Time: " << parallelTime << " seconds\n";

        
        int ok = 1;
        for ( long i = 0; i < n; i++) {
            if (cSerial[i] != cParallel[i]) {
                ok = 0;
                break;
            }

        }

        if (ok) {
            cout << "Verification:  SUCCESS\n";

        }
        
        else {
            cout << "Verification:  FAILED\n";
        }

       
        double speedup = serialTime / parallelTime;
        cout << "Speedup:       " << speedup << endl;

        delete[] a;
        delete[] b;
        delete[] cSerial;
        delete[] cParallel;
    }


    return 0;
}



//g++ -fopenmp -O2 task1.cpp -o task1
/*$env:OMP_NUM_THREADS = "1"
./task1

$env:OMP_NUM_THREADS = "2"
./task1

$env:OMP_NUM_THREADS = "4"
./task1

$env:OMP_NUM_THREADS = "8"
./task1*/