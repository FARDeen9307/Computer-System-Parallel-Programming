#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <omp.h>

using namespace std;


void sortSer(long *a, long n) {
    sort( a, a + n);

}


void sortPar( long *a, long n) {
    #pragma omp parallel

    {
        for ( long phase = 0; phase < n; phase++) {
            if ( phase % 2 == 0) {
             
                #pragma omp for
                for ( long i = 0; i < n - 1; i += 2) {

                    if (a[i] > a[i + 1]) {
                        long temp = a[i];
                        a[i] = a[i + 1];
                        a[i + 1] = temp;

                    }
                }
            } 
            
            else {
                
                #pragma omp for
                for ( long i = 1; i < n - 1; i += 2) {
                    if (a[i] > a[i + 1]) {
                        long temp = a[i];
                        a[i] = a[i + 1];
                        a[i + 1] = temp;
                    }
                }
                
            }
          

        }
    }
}

int main() {

    
    long sizes[] = {50000, 100000};
    int numSizes = 2;


    for ( int s = 0; s < numSizes; s++) {

        long n = sizes[s];

        cout << "\n----------------------------------------\n";
        cout << "Testing N = " << n << endl;

        long *aSer = new long[n];
        long *aPar = new long[n];

        
        for ( long i = 0; i < n; i++) {

            long val = ( n - i);
            aSer[i] = val;
            aPar[i] = val;

        }

      
        double t1 = omp_get_wtime();
        sortSer( aSer, n);
        double t2 = omp_get_wtime();
        double serialTime = t2 - t1;
        cout << "Serial Time:   " << serialTime << "s\n";

       

        t1 = omp_get_wtime();
        sortPar(aPar, n);
        t2 = omp_get_wtime();
        double parallelTime = t2 - t1;
        cout << "Parallel Time: " << parallelTime << "s\n";

       
        double speedup = 0.0;
        if (parallelTime > 0.0) {
            speedup = serialTime / parallelTime;

        }
        cout << "Speedup (Par):  " << speedup << endl;

       
        bool isSorted = true;
        for ( long i = 0; i < n; i++) {

            if (aSer[i] != aPar[i]) {

                isSorted = false;
                break;

            }

        }

        if ( isSorted) {

            cout << "Verification:   SUCCESS\n";
        } 
        
        else {
            cout << "Verification:   FAILED\n";
        }

        delete[] aSer;
        delete[] aPar;

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