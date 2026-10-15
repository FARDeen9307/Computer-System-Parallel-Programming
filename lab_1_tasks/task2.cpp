#include <iostream>
#include <cstdlib>
#include <omp.h>

using namespace std;

long sumSer( long *a, long n) {

    long sum = 0;
    for ( long i = 0; i < n; i++)

        sum += a[i];
    return sum;

}


long sumBroke( long *a, long n) {
    long sum = 0;
    #pragma omp parallel for
    for ( long i = 0; i < n; i++)
        sum += a[i];
    return sum;

}

long sumRed( long *a, long n) {

    long sum = 0;
    #pragma omp parallel for reduction(+:sum)
    for (long i = 0; i < n; i++)
        sum += a[i];
    return sum;

}


int main() {

    long sizes[] = {10000000, 100000000};
    int numSizes = 2;

    for ( int s = 0; s < numSizes; s++) {

        long n = sizes[s];
        cout << "\n----------------------------------------\n";
        cout << "Testing N = " << n << endl;

        long *a = new long[n];
        for ( long i = 0; i < n; i++) {
            a[i] = 1;

        }

    
        double t1 = omp_get_wtime();
        long r1 = sumSer(a, n);
        double t2 = omp_get_wtime();
        double serialTime = t2 - t1;
        cout << "Serial Sum:     " << r1 << " | Time: " << serialTime << "s\n";

        
        t1 = omp_get_wtime();
        long r2 = sumBroke(a, n);
        t2 = omp_get_wtime();
        cout << "Naive Sum (BRO): " << r2 << " | Time: " << t2 - t1 << "s\n";

       

        t1 = omp_get_wtime();
        long r3 = sumRed(a, n);
        double t3 = omp_get_wtime();
        double redTime = t3 - t1;
        cout << "Reduction Sum:  " << r3 << " | Time: " << redTime << "s\n";

        
        double speedup = serialTime / redTime;
        cout << "Speedup (Red):  " << speedup << endl;

        
        if ( r1 == r3) {
            cout << "Verification:   SUCCESS\n";
        }
        
        else {

            cout << "Verification:   FAILED\n";
        }

        delete[] a;
    }

    

    return 0;
}


//g++ -fopenmp -O2 task2.cpp -o task2
/*
$env:OMP_NUM_THREADS = "1"
./task2
$env:OMP_NUM_THREADS = "2"
./task2
$env:OMP_NUM_THREADS = "4"
./task2
$env:OMP_NUM_THREADS = "8"
./task2

*/