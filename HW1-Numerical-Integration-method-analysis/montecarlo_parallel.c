#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

#define N 10000000
#define pi 3.141592653589

int main()
{
	double x,y,sum=0, answer, error; int i;
	double time1, time2, total_time;
	omp_set_num_threads(8);
	time1=omp_get_wtime();

	#pragma omp parallel for private(x,y) reduction(+:sum)
	for(i=0;i<N;i++)
	{
		x=(rand()*pi)/RAND_MAX;
		y=(rand()*1.0)/RAND_MAX;

		if(y-sin(x)<=0)
			sum+=1;
	}
	answer=(sum/N)*pi;
	time2=omp_get_wtime();
	total_time=time2-time1;
	error=100.0*(answer-2.0)/2;
	printf("N= %d \n", N);
	printf("Integration Answer = %0.10f \n", answer);
	printf("Error = %0.10f \n", error);
	printf("Time: %0.9f \n", total_time);
	return 0;
}