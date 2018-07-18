# Parallelization-tools

This repository aim at getting familiar with the parallel programming API and how they can speed up the code. The first 2 homeworks contain the codes (which are written in C) and also the time analysis with different number of threads (for openmp) or processes (MPI) and convergence study with different number of sampling points. 

HW1- Numerical Integration (Trapezoid and Montecarlo method) using openmp

HW2- Numerical Integration (Trapezoid and Montecarlo method) using MPI and opencl (CUDA code will be added soon)

HW3- Code profiling of HW1 and HW2 codes (not GPU codes) using valgrind and gprof

The GPU code has been optimised with respect to the number of work groups, that have been formed based on the work group size and no of sampling points provided. The reports attached demonstrate how good GPUs are with respect to scaling a problem.

Overall in this problem, it is observer that the speed up provided by GPU is the fastest(about 2000 times faster in case of motecarlo intgration), followed by MPI (Upto 6 times faster than the serial code) and openmp(upto 2 times faster than the serial code).
