#include <stdio.h>
#include <math.h>
#include <omp.h>

#define N 10000000
#define pi 3.141592653589

/* By composite trapezoid rule: integration of a function f(x) from limits a to b is equal to h*( f(a)*0.5 + f(a+h) +f(a+2h) ..... f(a+(n-1)h)+0.5*f(b) )  where h=(b-a)/N   */

int main()
{
	double a,b, time1, time2, total_time,error; int i;
	double h, answer, x;
	time1=omp_get_wtime();
	a=0; b=pi;
	h=(b-a)/N;
	x=a;
	answer=0.5*(sin(a)+sin(b));

	for(i=1; i<N;i++)
	{
		x=x+h;
		answer+=sin(x);
	}
	answer=answer*h;
	error= 100.0*(answer-2.0)/2.0;
	time2=omp_get_wtime();
	total_time=time2-time1;
	printf("N= %d \n", N);
	printf("Integration Answer = %0.9f \n", answer);
	printf("Error = %0.10f \n", error);
	printf("Time: %0.9f \n", total_time);

	return 0;
}
