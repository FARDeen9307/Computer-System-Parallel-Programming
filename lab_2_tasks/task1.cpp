#include <iostream>
#include <cstdlib>
#include <omp.h>

using namespace std;


long searchSer(long *a, long n, long target) {
    for (long i = 0; i < n; i++) {
        if (a[i] == target) {
            return i; 

        }

    }

    return -1; 

}



long searchPar(long *a, long n, long target) {
    long foundIndex = -1;

    #pragma omp parallel

    {
        long localIndex = -1;

        #pragma omp for
        for ( long i = 0; i < n; i++) {

           
            if ( foundIndex != -1){

             continue;
        }

            if ( a[i] == target) {
                localIndex = i;
                #pragma omp critical

                {
                    if (foundIndex == -1 || localIndex < foundIndex) {
                        foundIndex = localIndex;
                    }
                }

          
              }
        }
    }

    return foundIndex;
}


int main() {
    long sizes[] = {10000000, 100000000};
    int numSizes = 2;

    for ( int s = 0; s < numSizes; s++) {

        long n = sizes[s];
        long target = n / 2; 

        cout << "\n----------------------------------------\n";
        cout << "Testing N = " << n << " | Target = " << target << endl;


        long *a = new long[n];
        for (long i = 0; i < n; i++) {
            a[i] = i;
        }



        double t1 = omp_get_wtime();
        long r1 = searchSer(a, n, target);
        double t2 = omp_get_wtime();
        double serialTime = t2 - t1;
        cout << "Serial Index:   " << r1 << " | Time: " << serialTime << "s\n";

        
        t1 = omp_get_wtime();
        long r2 = searchPar(a, n, target);
        t2 = omp_get_wtime();
        double parallelTime = t2 - t1;
        cout << "Parallel Index: " << r2 << " | Time: " << parallelTime << "s\n";

       

        double speedup = 0.0;
        if (parallelTime > 0.0) {
            speedup = serialTime / parallelTime;
        }
        cout << "Speedup (Par):  " << speedup << endl;

        
        if ( r1 == r2) {

            cout << "Verification:   SUCCESS\n";
        } 
        
        else {
            cout << "Verification:   FAILED\n";
        }

        delete[] a;
        
    }

    return 0;
}




/*
g++ -fopenmp -O2 task2.cpp -o task2


$env:OMP_NUM_THREADS = "1"
./task2


$env:OMP_NUM_THREADS = "2"
./task2


$env:OMP_NUM_THREADS = "4"
./task2


$env:OMP_NUM_THREADS = "8"
./task2

*/