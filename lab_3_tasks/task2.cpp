#include <iostream>
#include <cstdlib>
#include <omp.h>

using namespace std;

int main() {
    
    int sizes[] = {100000, 1000000, 10000000}; // N = 10^5, 10^6, 10^7
    int numSizes = 3;

    for (int s = 0; s < numSizes; s++) {
        int n = sizes[s];

        cout << "\n----------------------------------------\n";
        cout << "Testing N = " << n << " with Uneven Workload Scheduling\n";

        double *aSer = new double[n];
        double *aStat = new double[n];
        double *aDyn = new double[n];
        double *aGuid = new double[n];

        for (int i = 0; i < n; i++) {
            double val = i * 0.001;
            aSer[i] = val;
            aStat[i] = val;
            aDyn[i] = val;
            aGuid[i] = val;
        }

        // 1. Serial Baseline
        double t1 = omp_get_wtime();
        for (int i = 0; i < n; i++) {
            double x = aSer[i];
            int work = (i % 1000) * 100;
            for (int j = 0; j < work; j++) {
                x = x * 1.0000001 + 0.0000001;
            }
            aSer[i] = x;
        }
        double t2 = omp_get_wtime();
        double serialTime = t2 - t1;
        cout << "Serial Time:         " << serialTime << "s\n";

        // 2. Parallel Schedule: Static
        t1 = omp_get_wtime();
        #pragma omp parallel for schedule(static)
        for (int i = 0; i < n; i++) {
            double x = aStat[i];
            int work = (i % 1000) * 100;
            for (int j = 0; j < work; j++) {
                x = x * 1.0000001 + 0.0000001;
            }
            aStat[i] = x;
        }
        t2 = omp_get_wtime();
        double statTime = t2 - t1;
        cout << "Parallel (Static):   " << statTime << "s | Speedup: " << (serialTime / statTime) << "\n";

        // 3. Parallel Schedule: Dynamic (chunk size 100)
        t1 = omp_get_wtime();
        #pragma omp parallel for schedule(dynamic, 100)
        for (int i = 0; i < n; i++) {
            double x = aDyn[i];
            int work = (i % 1000) * 100;
            for (int j = 0; j < work; j++) {
                x = x * 1.0000001 + 0.0000001;
            }
            aDyn[i] = x;
        }
        t2 = omp_get_wtime();
        double dynTime = t2 - t1;
        cout << "Parallel (Dynamic):  " << dynTime << "s | Speedup: " << (serialTime / dynTime) << "\n";

        // 4. Parallel Schedule: Guided
        t1 = omp_get_wtime();
        #pragma omp parallel for schedule(guided)
        for (int i = 0; i < n; i++) {
            double x = aGuid[i];
            int work = (i % 1000) * 100;
            for (int j = 0; j < work; j++) {
                x = x * 1.0000001 + 0.0000001;
            }
            aGuid[i] = x;
        }
        t2 = omp_get_wtime();
        double guidTime = t2 - t1;
        cout << "Parallel (Guided):   " << guidTime << "s | Speedup: " << (serialTime / guidTime) << "\n";

        // Verification
        bool isValid = true;
        for (int i = 0; i < n; i++) {
            if (aSer[i] != aStat[i] || aSer[i] != aDyn[i] || aSer[i] != aGuid[i]) {
                isValid = false;
                break;
            }
        }

        if (isValid) {
            cout << "Verification:        SUCCESS\n";
        } else {
            cout << "Verification:        FAILED\n";
        }

        delete[] aSer;
        delete[] aStat;
        delete[] aDyn;
        delete[] aGuid;
    }

    return 0;
}