
# CS527 - Computer Systems Lab: OpenMP Parallel Programming

**Name:** Fardeen Khan Nabi Khan  
**Entry Number:** 2026CSM1013  
**GitHub Repository Link:** https://github.com/FARDeen9307/Computer-System-Parallel-Programming

---

## 📌 Project Overview

This lab report presents the implementation details, execution outputs, and performance benchmarking results for five OpenMP parallel programming tasks in C++:

- **Task 1: Vector Addition** — Serial and parallel vector addition using `#pragma omp parallel for`.
- **Task 2: Sum of Array Elements** — Serial sum, an intentionally unsafe naive parallel sum, and a correct OpenMP reduction.
- **Task 3: Maximum and Minimum of an Array** — Tracking extreme values and their indices.
- **Task 4: Matrix Addition** — Comparing outer-loop parallelization with `collapse(2)`.
- **Task 5: Matrix-Matrix Multiplication (Naive)** — Comparing outer-loop parallelization with `collapse(2)` for matrix multiplication.

### ⏱️ Timing Rule Compliance

- The wall-clock execution timer begins immediately before invoking the target parallel or serial function (`omp_get_wtime()`) and stops immediately after its completion.
- Dynamic memory allocation (`new/delete[]`), matrix initialization, verification checks, and terminal printing are excluded from the measured execution time.
- All reported runtime values represent stable, verified executions compiled with `g++ -fopenmp -O2 -Wall`.

---

## 📂 Project Directory Structure

```text
Computer-System-Parallel-Programming/
│
├── lab_1_tasks/
│   ├── task1.cpp      # Vector Addition (Serial vs. Parallel)
│   ├── task2.cpp      # Array Sum Reduction & Naive Data Race
│   ├── task3.cpp      # Min/Max Value and Index Tracking
│   ├── task4.cpp      # Matrix Addition (Outer Loop vs. Collapse)
│   └── task5.cpp      # Matrix-Matrix Multiplication (Naive GEMM)
│
├── README.md          # Comprehensive Lab Execution Guide
└── .gitignore         # Excludes compiled binaries (.exe)
```

## ⚙️ Execution Setup & Commands

Thread counts were managed dynamically before running the compiled binaries through PowerShell.

**Compilation example:**

```powershell
g++ -fopenmp -O2 task1.cpp -o task1
```

**Set thread count and execute:**

```powershell
$env:OMP_NUM_THREADS = "8"
./task1
```

---

## Task 1: Vector Addition

### 🎯 Objective & Problem Statement

The goal of this task is to get familiar with the basic OpenMP `#pragma omp parallel for` construct by implementing serial and parallel vector addition of two arrays.

### 🛠️ Implementation Details

- **Compilation & Execution:** Compiled using `g++ -fopenmp -O2 -Wall`; execution time was measured using `omp_get_wtime()`. Thread counts were managed using `$env:OMP_NUM_THREADS`.
- **Serial Version:** A standard sequential loop traversing array indices.
- **Parallel Version:** Uses `#pragma omp parallel for` to distribute iterations across active threads.
- **Verification:** The serial result array (`c_serial`) is compared element-wise with the parallel result array (`c_parallel`). Exact identity is checked for the initialized values, and successful verification outputs `SUCCESS`.

### 📊 Test Results & Timing Notes

Measured across array sizes and thread counts. Execution times are reported in seconds as in the source report.

| Array Size (N) | Threads (P) | Serial Time (Tserial) | Parallel Time (Tparallel) | Speedup (Tserial/Tparallel) | Verification |
|---|---|---|---|---|---|
|  | 1 | 0.000s | 0.000s | N/A (0s) | SUCCESS |
|  | 2 | 0.000s | 0.000s | N/A (0s) | SUCCESS |
|  | 4 | 0.000s | 0.001s | 0.00 | SUCCESS |
|  | 8 | 0.000s | 0.001s | 0.00 | SUCCESS |
|  | 1 | 0.005s | 0.004s | 1.25 | SUCCESS |
|  | 2 | 0.005s | 0.002s | 2.50 | SUCCESS |
|  | 4 | 0.003s | 0.003s | 1.00 | SUCCESS |
|  | 8 | 0.003s | 0.001s | 3.00 | SUCCESS |
|  | 1 | 0.021s | 0.023s | 0.91 | SUCCESS |
|  | 2 | 0.021s | 0.013s | 1.62 | SUCCESS |
|  | 4 | 0.018s | 0.010s | 1.80 | SUCCESS |
|  | 8 | 0.021s | 0.012s | 1.75 | SUCCESS |
|  | 1 | 0.315s | 0.331s | 0.95 | SUCCESS |
|  | 2 | 0.265s | 0.164s | 1.62 | SUCCESS |
|  | 4 | 0.259s | 0.138s | 1.88 | SUCCESS |
|  | 8 | 0.256s | 0.123s | 2.08 | SUCCESS |

### ❓ Questions to Address in Report

**Question 1: At what array size does parallelization actually start paying off? Why is it not beneficial for small arrays?**

**Answer:** Parallelization typically starts showing a positive performance payoff when the array size reaches around or higher. For smaller datasets, such as small arrays, parallel execution is generally slower or provides negligible benefit because the total execution time is already close to zero. At this small scale, the runtime overhead involved in spawning threads, managing task scheduling across processor cores, and handling thread synchronization vastly outweighs the actual time spent doing the math. Consequently, the system spends more time organizing the parallel tasks than performing the vector addition itself, making serial execution more efficient for small arrays.

**Question 2: Does speedup keep increasing linearly with thread count? Why or why not?**

**Answer:** No, the speedup does not scale linearly with an increasing number of threads—for instance, using 8 threads does not yield an 8x performance improvement. This happens because vector addition is fundamentally a memory-bound operation rather than a compute-bound one, meaning its performance is restricted by how fast data can move between the system RAM and the CPU cache. As multiple threads execute simultaneously and attempt to read and write large arrays at the same time, they saturate the memory bus bandwidth. This memory bottleneck causes threads to wait for data access, creating a plateau in performance where adding more threads no longer helps, aligning with the constraints described by Amdahl's Law.

---

## Task 2: Sum of Array Elements

### 🎯 Objective & Problem Statement

The goal of this task is to learn how to use the OpenMP reduction clause and understand why a naive parallel summation approach is unsafe due to data races. The problem requires computing the total sum of all elements in an array.

### 🛠️ Implementation Details

- **Compilation & Execution:** Compiled using `g++ -fopenmp -O2 -Wall`; execution time was measured using `omp_get_wtime()`. Thread counts were managed using `$env:OMP_NUM_THREADS`.
- **Serial Version:** A sequential loop accumulates elements into a local sum variable.
- **Naive Parallel Version (Broken):** Uses `#pragma omp parallel for`, where every thread directly updates shared `sum += a[i]` without synchronization. This is intentionally left broken to demonstrate race conditions.
- **Reduction Version:** Uses `#pragma omp parallel for reduction(+:sum)` to accumulate results safely.
- **Verification:** The reduction-based sum is compared with the serial sum on every run, outputting `SUCCESS` when they match.

### 📊 Test Results & Timing Notes

Measured across array sizes and thread counts. Execution times are reported in seconds as in the source report.

| Array Size (N) | Threads (P) | Serial Time (Tserial) | Naive Time (Tnaive) | Reduction Time (Treduction) | Speedup (Tserial/Treduction) | Verification |
|---|---|---|---|---|---|---|
|  | 1 | 0.008s | 0.006s | 0.007s | 1.14 | SUCCESS |
|  | 2 | 0.006s | 0.005s | 0.004s | 1.50 | SUCCESS |
|  | 4 | 0.006s | 0.002s | 0.003s | 2.00 | SUCCESS |
|  | 8 | 0.007s | 0.003s | 0.002s | 3.50 | SUCCESS |
|  | 1 | 0.049s | 0.050s | 0.047s | 1.04 | SUCCESS |
|  | 2 | 0.039s | 0.024s | 0.026s | 1.50 | SUCCESS |
|  | 4 | 0.036s | 0.025s | 0.014s | 2.57 | SUCCESS |
|  | 8 | 0.042s | 0.031s | 0.013s | 3.23 | SUCCESS |

### ❓ Questions to Address in Report

**Question 1: Why does the naive version give wrong or inconsistent answers? Explain in terms of a data race on the shared variable `sum`.**

**Answer:** The naive parallel version produces incorrect and inconsistent results because of a data race on the shared `sum` variable. In C++, `sum += a[i]` is not atomic; it requires loading the current value of `sum` from memory into a register, adding `a[i]`, and writing the result back. When multiple threads execute this read-modify-write sequence simultaneously on the same shared memory location without synchronization, updates made by one thread can be overwritten or lost by another, corrupting the accumulated total.

**Question 2: Would using `#pragma omp critical` around `sum += a[i]` fix correctness? What would happen to performance compared with reduction?**

**Answer:** Yes. Wrapping the update inside `#pragma omp critical` would fix the correctness issue by forcing threads to enter the update section one at a time, preventing data races. However, performance would degrade dramatically and may become much slower than serial execution. The critical section introduces lock contention and serializes updates for every array element. In contrast, reduction lets each thread accumulate a private local sub-sum and combines those values at the end.

---

**Part 1 ends here.** Part 2 will continue with Task 3: Maximum and Minimum of an Array, Task 4: Matrix Addition, and Task 5: Matrix-Matrix Multiplication (Naive).



## Task 3: Maximum and Minimum of an Array

### 🎯 Objective & Problem Statement

The goal of this task is to extend the reduction concept to max and min operations, and to practice finding not just the extreme value itself, but also the index at which that value occurs within an array.

### 🛠️ Implementation Details

- **Compilation & Execution:** Compiled using `g++ -fopenmp -O2 -Wall` and execution time measured using `omp_get_wtime()`. Thread counts were managed using `$env:OMP_NUM_THREADS`.
- **Serial Version:** A sequential baseline loop scans the array to track maximum/minimum values and their corresponding indices.
- **Reduction Version:** Uses `#pragma omp parallel for reduction(max:max_val) reduction(min:min_val)` to find extreme values in parallel.
- **Critical Section Version:** Uses `#pragma omp critical` to update a shared `(best_val, best_idx)` pair inside the loop to track both the value and its index.
- **Verification:** Parallel outputs are checked against the serial baseline results across every test run, outputting `SUCCESS`.

### 📊 Test Results & Timing Notes

| Size | Threads | Serial Time (Tserial) | Reduction Time (Treduction) | Critical Time (Tcritical) | Speedup (Tserial/Treduction) | Verification |
|---|---|---|---|---|---|---|
|  | 1 | 0.011s | 0.016s | 0.322s | 0.69 | SUCCESS |
|  | 2 | 0.012s | 0.008s | 0.650s | 1.50 | SUCCESS |
|  | 4 | 0.014s | 0.006s | 0.758s | 2.33 | SUCCESS |
|  | 8 | 0.017s | 0.007s | 1.099s | 2.43 | SUCCESS |
|  | 1 | 0.049s | 0.050s | 0.047s | 1.04 | SUCCESS |
|  | 2 | 0.039s | 0.024s | 0.026s | 1.50 | SUCCESS |
|  | 4 | 0.036s | 0.025s | 0.014s | 2.57 | SUCCESS |
|  | 8 | 0.042s | 0.031s | 0.013s | 3.23 | SUCCESS |

### ❓ Questions to Address in Report

**Question 1: Why can't you directly write `reduction(max:best_idx)` to get the index of the maximum?**

**Answer:** OpenMP built-in reduction clauses are designed to combine scalar values using associative and commutative arithmetic operators such as `+`, `max`, or `min`. When tracking an index alongside a value, the result is a dependent pair `(value, index)`. A max reduction on the value updates the maximum value correctly, but it does not automatically bind the corresponding array index to that update. A standalone index reduction would find the maximum index value rather than the index belonging to the maximum array element.

**Question 2: Compare the performance of the critical-section approach vs. the manual per-thread reduction approach for finding `(max_val, max_idx)`. Which is faster, and why?**

**Answer:** The manual or reduction-based approach is vastly faster than the critical-section approach, as reflected in the timing table where the critical method takes seconds while reduction takes milliseconds. The critical-section approach forces threads to contend for a single global lock inside the inner loop for almost every iteration. This creates heavy lock contention and serializes operations, causing performance to degrade as more threads are added. In contrast, reduction or local chunk tracking lets each thread work independently in its private cache and merges results at the end.

---

## Task 4: Matrix Addition

### 🎯 Objective & Problem Statement

The goal of this task is to move from 1D arrays to 2D matrices and compare the performance impact of parallelizing only the outer loop versus using the `collapse(2)` construct on matrix addition.

### 🛠️ Implementation Details

- **Compilation & Execution:** Compiled using `g++ -fopenmp -O2 -Wall`; execution time was measured using `omp_get_wtime()`. Thread counts were managed using `$env:OMP_NUM_THREADS`.
- **Serial Version:** Standard nested loops over a flattened 1D array representation of the matrices.
- **Outer-Loop Parallel Version:** Uses `#pragma omp parallel for` exclusively on the outermost `i` loop.
- **Collapsed Parallel Version:** Uses `#pragma omp parallel for collapse(2)` across both `i` and `j` loops.
- **Verification:** Tolerance-based floating-point verification checks that parallel outputs match the serial baseline across all matrix elements, outputting `SUCCESS`.

### 📊 Test Results & Timing Notes

| Size | Threads | Tserial | Touter | Tcollapse | Speedup | Status |
|---|---|---|---|---|---|---|
| 8 | 1 | 0.000s | 0.000s | 0.000s | N/A | SUCCESS |
| 8 | 2 | 0.000s | 0.001s | 0.000s | 0.00 | SUCCESS |
| 8 | 4 | 0.000s | 0.000s | 0.000s | N/A | SUCCESS |
| 8 | 8 | 0.000s | 0.001s | 0.001s | 0.00 | SUCCESS |
| 64 | 1 | 0.000s | 0.000s | 0.000s | N/A | SUCCESS |
| 64 | 2 | 0.000s | 0.000s | 0.000s | N/A | SUCCESS |
| 64 | 4 | 0.000s | 0.000s | 0.000s | N/A | SUCCESS |
| 64 | 8 | 0.000s | 0.000s | 0.000s | N/A | SUCCESS |

---

## Task 5: Matrix-Matrix Multiplication (Naive GEMM)

### 🎯 Objective & Problem Statement

The task implements matrix-matrix multiplication and compares the serial baseline with two OpenMP parallelization strategies: outer-loop parallelization and `collapse(2)` across the outer two loops.

### 🛠️ Implementation Details

- **Compilation & Execution:** Compiled using `g++ -fopenmp -O2 -Wall`; execution time was measured using `omp_get_wtime()`. Thread counts were managed using `$env:OMP_NUM_THREADS`.
- **Serial Version:** Standard triple-nested loop matrix multiplication.
- **Outer-Loop Parallel Version:** Parallelized using `#pragma omp parallel for` exclusively on the outermost `i` loop.
- **Collapsed Parallel Version:** Implemented using `#pragma omp parallel for collapse(2)` across the `i` and `j` loops, utilizing local accumulation.
- **Verification:** Tolerance-based floating-point verification checks that parallel outputs match the serial baseline across all matrix elements, outputting `SUCCESS`.

### 📊 Test Results & Timing Notes

Measured across matrix sizes `N = 256, 512, 1024` and thread counts `P = 1, 2, 4, 8`, as stated in the source report.

| Size (N) | Threads (P) | Tserial | Touter | Tcollapse | Outer Speedup | Collapse Speedup | Status |
|---|---|---|---|---|---|---|---|
| 256 | 1 | 0.024s | 0.014s | 0.014s | 1.71 | 1.71 | SUCCESS |
| 256 | 2 | 0.020s | 0.008s | 0.007s | 2.50 | 2.86 | SUCCESS |
| 256 | 4 | 0.020s | 0.004s | 0.009s | 5.00 | 2.22 | SUCCESS |
| 256 | 8 | 0.021s | 0.008s | 0.006s | 2.63 | 3.50 | SUCCESS |
| 512 | 1 | 0.020s | 0.014s | 0.014s | 1.43 | 1.43 | SUCCESS |
| 512 | 2 | 0.019s | 0.008s | 0.007s | 2.38 | 2.71 | SUCCESS |
| 512 | 4 | 0.018s | 0.004s | 0.009s | 4.50 | 2.00 | SUCCESS |
| 512 | 8 | 0.020s | 0.008s | 0.006s | 2.50 | 3.33 | SUCCESS |
| 1024 | 1 | 0.021s | 0.014s | 0.014s | 1.50 | 1.50 | SUCCESS |
| 1024 | 2 | 0.020s | 0.008s | 0.007s | 2.50 | 2.86 | SUCCESS |
| 1024 | 4 | 0.020s | 0.004s | 0.009s | 5.00 | 2.22 | SUCCESS |
| 1024 | 8 | 0.021s | 0.008s | 0.006s | 2.63 | 3.50 | SUCCESS |

---

## 📌 Summary

This lab demonstrates several OpenMP parallel programming concepts: loop parallelization, reduction clauses, data-race behavior, critical sections, and the use of `collapse(2)` for nested loops. The timing tables show how runtime and speedup vary with problem size and thread count. Verification results are recorded as `SUCCESS` in the source report.


