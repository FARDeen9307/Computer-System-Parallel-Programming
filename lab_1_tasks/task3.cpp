#include <iostream>
#include <cstdlib>
#include <omp.h>

using namespace std;


void findMinMaxSerial( int *a, long n, int *maxVal, long *maxIdx, int *minVal, long *minIdx) {
    int mVal = a[0], miVal = a[0];
    long mIdx = 0, miIdx = 0;
    for (long i = 1; i < n; i++) {

        if (a[i] > mVal) {
            mVal = a[i];
            mIdx = i;

        }

        if (a[i] < miVal) {
            miVal = a[i];
            miIdx = i;
        }

    }
    *maxVal = mVal; *maxIdx = mIdx;
    *minVal = miVal; *minIdx = miIdx;
}


int findMaxRed( int *a, long n) {

    int maxVal = a[0];
    #pragma omp parallel for reduction(max:maxVal)
    for (long i = 0; i < n; i++) {
        if (a[i] > maxVal) maxVal = a[i];

    }

    return maxVal;
}


int findMinRed( int *a, long n) {

    int minVal = a[0];
    #pragma omp parallel for reduction(min:minVal)
    for ( long i = 0; i < n; i++) {

        if (a[i] < minVal) minVal = a[i];
    }

    return minVal;

}


void findMaxCrit( int *a, long n, int *maxVal, long *maxIdx) {

    int bestVal = a[0];
    long bestIdx = 0;
    #pragma omp parallel for
    for ( long i = 1; i < n; i++) {

        #pragma omp critical
        {

            if ( a[i] > bestVal) {

                bestVal = a[i];
                bestIdx = i;
            }
        }

    }
    *maxVal = bestVal;
    *maxIdx = bestIdx;

}


void findMinCrit( int *a, long n, int *minVal, long *minIdx) {

    int bestVal = a[0];
    long bestIdx = 0;
    #pragma omp parallel for
    for ( long i = 1; i < n; i++) {

        #pragma omp critical

        {
            if ( a[i] < bestVal) {

                bestVal = a[i];
                bestIdx = i;


            }
        }
    }

    *minVal = bestVal;
    *minIdx = bestIdx;
}

int main() {
    long sizes[] = {10000000, 100000000};
    int numSizes = 2;

    for ( int s = 0; s < numSizes; s++) {

        long n = sizes[s];
        cout << "\n----------------------------------------\n";
        cout << "Testing N = " << n << endl;


        int *a = new int[n];
        for (long i = 0; i < n; i++) {
            a[i] = i;
        }
        a[n / 4] = 555555;   
        a[n / 2] = 999999;   

        int serMax = 0, serMin = 0, redMax = 0, redMin = 0, critMax = 0, critMin = 0;
        long serMaxIdx = 0, serMinIdx = 0, critMaxIdx = 0, critMinIdx = 0;


        
        double t1 = omp_get_wtime();
        findMinMaxSerial(a, n, &serMax, &serMaxIdx, &serMin, &serMinIdx);
        double t2 = omp_get_wtime();
        double serialTime = t2 - t1;
        cout << "Serial Max: " << serMax << " (Idx " << serMaxIdx << ") | Min: " << serMin << " (Idx " << serMinIdx << ") | Time: " << serialTime << "s\n";

        
        t1 = omp_get_wtime();
        redMax = findMaxRed(a, n);
        redMin = findMinRed(a, n);
        t2 = omp_get_wtime();
        cout << "Reduction   Max: " << redMax << " | Min: " << redMin << " | Time: " << t2 - t1 << "s\n";

        
        t1 = omp_get_wtime();
        findMaxCrit(a, n, &critMax, &critMaxIdx);
        findMinCrit(a, n, &critMin, &critMinIdx);
        t2 = omp_get_wtime();
        double critTime = t2 - t1;
        cout << "Critical    Max: " << critMax << " (Idx " << critMaxIdx << ") | Min: " << critMin << " (Idx " << critMinIdx << ") | Time: " << critTime << "s\n";

   
        
        double speedup = serialTime / critTime;
        cout << "Speedup (Crit): " << speedup << endl;

     
        if ( serMax == redMax && serMin == redMin && serMax == critMax && serMin == critMin && serMaxIdx == critMaxIdx && serMinIdx == critMinIdx) {
            cout << "Verification: SUCCESS\n";

        } 
        
        else {

            cout << "Verification: FAILED\n";
        }

        delete[] a;
    }


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