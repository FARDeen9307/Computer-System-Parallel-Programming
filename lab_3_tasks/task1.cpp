#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <omp.h>

using namespace std;


void merge( int *a, int *tmp, int left, int mid, int right) {
    int i = left;
    int j = mid + 1;
    int k = left;

    while ( i <= mid && j <= right) {
        if (a[i] <= a[j]) {
            tmp[k++] = a[i++];
        } 
        
        else {
            tmp[k++] = a[j++];
        }

    }

    while (i <= mid) {

        tmp[k++] = a[i++];
    }

    while ( j <= right) {

        tmp[k++] = a[j++];
    }

    for ( i = left; i <= right; i++) {

        a[i] = tmp[i];
    }
}



void mergeSortSer( int *a, int *tmp, int left, int right) {
    if ( left >= right){

     return;
}

    int mid = left + ( right - left) / 2;
    mergeSortSer( a, tmp, left, mid);
    mergeSortSer( a, tmp, mid + 1, right);
    merge( a, tmp, left, mid, right);

}


void mergeSortPar( int *a, int *tmp, int left, int right) {

    if (left >= right){

     return;
    }

    
    if ( right - left < 2048) {

        mergeSortSer(a, tmp, left, right);
        return;

    }

    int mid = left + ( right - left) / 2;

    #pragma omp task shared( a, tmp) firstprivate( left, mid)
    mergeSortPar( a, tmp, left, mid);

    #pragma omp task shared( a, tmp) firstprivate(mid, right)
    mergeSortPar(a, tmp, mid + 1, right);

    #pragma omp taskwait
    merge(a, tmp, left, mid, right);
}

int main() {

    int sizes[] = {100000, 1000000, 10000000}; 
    int numSizes = 3;

    for ( int s = 0; s < numSizes; s++) {

        int n = sizes[s];

        cout << "\n----------------------------------------\n";
        cout << "Testing N = " << n << " for Parallel Merge Sort\n";

        int *aSer = new int[n];
        int *aPar = new int[n];
        int *tmpSer = new int[n];
        int *tmpPar = new int[n];

        
        for ( int i = 0; i < n; i++) {

            int val = (n - i);
            aSer[i] = val;
            aPar[i] = val;
        }

        
        double t1 = omp_get_wtime();
        mergeSortSer(aSer, tmpSer, 0, n - 1);
        double t2 = omp_get_wtime();
        double serialTime = t2 - t1;
        cout << "Serial Time:   " << serialTime << "s\n";

        
        t1 = omp_get_wtime();
        #pragma omp parallel
        {
            #pragma omp single
            mergeSortPar(aPar, tmpPar, 0, n - 1);
        }
        t2 = omp_get_wtime();
        double parallelTime = t2 - t1;
        cout << "Parallel Time: " << parallelTime << "s\n";

        
        
        double speedup = 0.0;
        if ( parallelTime > 0.0) {
            speedup = serialTime / parallelTime;

        }

        cout << "Speedup (Par):  " << speedup << endl;


        bool isSorted = true;
        for ( int i = 0; i < n; i++) {
            if ( aSer[i] != aPar[i]) {
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
        delete[] tmpSer;
        delete[] tmpPar;


    }
    

    return 0;
}