#include <iostream>
#include <omp.h>

using namespace std;

int main() {
    int a[5] = {1, 2, 3, 4, 5};
    int b[5] = {6, 7, 8, 9, 10};
    int c[5];

    #pragma omp parallel num_threads(5)
    {
        // Declared inside so tid is private to each thread
        int tid = omp_get_thread_num();

        c[tid] = a[tid] + b[tid];

        #pragma omp critical
        {
            cout << "c[" << tid << "]=" << c[tid] << endl;
        }
    }

    return 0;
}