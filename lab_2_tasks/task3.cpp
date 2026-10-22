#include <iostream>
#include <cstdlib>
#include <omp.h>

using namespace std;


void calcHistSer(long *a, long n, int *hist) {
    for ( int i = 0; i < 10; i++) {
        hist[i] = 0;

    }

    for ( long i = 0; i < n; i++) {

        int val = a[i];
        hist[val]++;
    }

}


void calcHistPar( long *a, long n, int *hist) {

    for (int i = 0; i < 10; i++) {
        hist[i] = 0;

    }

    #pragma omp parallel for
    for ( long i = 0; i < n; i++) {

        int val = a[i];
        #pragma omp atomic
        hist[val]++;

    }

}

int main() {
    long n = 100000000; 

    cout << "\n----------------------------------------\n";
    cout << "Testing N = " << n << " for Histogram Calculation (Values 0-9)" << endl;

    long *a = new long[n];
   
    for (long i = 0; i < n; i++) {
        a[i] = i % 10;

    }


    int histSer[10] = {0};
    int histPar[10] = {0};


    double t1 = omp_get_wtime();
    calcHistSer(a, n, histSer);
    double t2 = omp_get_wtime();
    double serialTime = t2 - t1;
    cout << "Serial Time:   " << serialTime << "s\n";

   
    t1 = omp_get_wtime();
    calcHistPar( a, n, histPar);
    t2 = omp_get_wtime();
    double parallelTime = t2 - t1;
    cout << "Parallel Time: " << parallelTime << "s\n";

    
    double speedup = 0.0;
    if ( parallelTime > 0.0) {

        speedup = serialTime / parallelTime;

    }

    cout << "Speedup (Par):  " << speedup << endl;

   
    bool isMatch = true;
    for ( int i = 0; i < 10; i++) {

        if (histSer[i] != histPar[i]) {

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

    delete[] a;
    return 0;

}


/*
g++ -fopenmp -O2 task3.cpp -o task3

$env:OMP_NUM_THREADS = "1"
./task3

$env:OMP_NUM_THREADS = "2"
./task3

$env:OMP_NUM_THREADS = "4"
./task3

$env:OMP_NUM_THREADS = "8"
./task3
*/