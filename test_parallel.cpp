#include <iostream>
#include <omp.h>

using namespace std;

int main() {
    // Fork a team of threads
    #pragma omp parallel
    {
        // Get the current thread's unique ID
        int thread_id = omp_get_thread_num();

        // Get the total number of threads in the team
        int total_threads = omp_get_num_threads();

        // Use critical section to prevent scrambled terminal output
        #pragma omp critical
        {
            cout << "Hello World from Thread " << thread_id 
                 << " of " << total_threads << endl;
        }
    }

    return 0;
}