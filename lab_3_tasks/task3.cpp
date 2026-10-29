#include <iostream>
#include <cstdlib>
#include <omp.h>

using namespace std;

int main() {
    long sizes[] = {100000, 1000000, 10000000, 100000000}; // N = 10^5, 10^6, 10^7, 10^8
    int numSizes = 4;

    for ( int s = 0; s < numSizes; s++) {
        long n = sizes[s];

        cout << "\n----------------------------------------\n";
        cout << "Testing N = " << n << " for Reductions vs Atomic\n";

        int *a = new int[n];
        for ( long i = 0; i < n; i++) {
            
            a[i] = i; // Mix of even and odd values
        }

        // 1. Serial Baseline
        double t1 = omp_get_wtime();
        long serEven = 0, serOdd = 0;
        for (long i = 0; i < n; i++) {
            if (a[i] % 2 == 0) serEven++;
            else serOdd++;
        }
        double t2 = omp_get_wtime();
        double serialTime = t2 - t1;
        cout << "Serial Time:         " << serialTime << "s (Even: " << serEven << ", Odd: " << serOdd << ")\n";

        // 2. Parallel Reduction
        t1 = omp_get_wtime();
        long parRedEven = 0, parRedOdd = 0;
        #pragma omp parallel for reduction(+:parRedEven, parRedOdd)
        for (long i = 0; i < n; i++) {
            if (a[i] % 2 == 0) parRedEven++;
            else parRedOdd++;
        }
        t2 = omp_get_wtime();
        double redTime = t2 - t1;
        cout << "Parallel (Reduction):" << redTime << "s | Speedup: " << (serialTime / redTime) << "\n";

        // 3. Parallel Atomic Updates
        t1 = omp_get_wtime();
        long parAtEven = 0, parAtOdd = 0;
        #pragma omp parallel for
        for (long i = 0; i < n; i++) {
            if (a[i] % 2 == 0) {
                #pragma omp atomic
                parAtEven++;
            } else {
                #pragma omp atomic
                parAtOdd++;
            }
        }
        t2 = omp_get_wtime();
        double atTime = t2 - t1;
        cout << "Parallel (Atomic):   " << atTime << "s | Speedup: " << (serialTime / atTime) << "\n";

        // Verification
        if (serEven == parRedEven && serOdd == parRedOdd && serEven == parAtEven && serOdd == parAtOdd) {
            cout << "Verification:        SUCCESS\n";
        } else {
            cout << "Verification:        FAILED\n";
        }

        delete[] a;
    }

    return 0;
}