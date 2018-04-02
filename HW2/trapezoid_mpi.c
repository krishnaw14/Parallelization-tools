#include <stdio.h>
#include <math.h>
#include <mpi.h>

#define pi 3.141592653589
#define N 100000000

int main(int argc, char ** argv)
{
	double a,b, time1, time2, total_time, error;
	double send_msg, recv_msg; 
	int i, num;
	double h, answer, x, local_a, range;

	int rank, size;

	a=0;
	b=pi;
	h=(b-a)/N;
	
	MPI_Status status;


	MPI_Init(&argc, &argv);

	time1 = MPI_Wtime();

    MPI_Comm_rank(MPI_COMM_WORLD,&rank); // Each thread will be identified by its rank
    MPI_Comm_size(MPI_COMM_WORLD,&size); //size stores total no of threads or communication units

    num = N/(size); // Divide the number of sampling points equally into threads
    range=(b-a)/size; // Divide the range of the numbers equally between the threads

    	local_a =  a + rank*range; // Each thread will have their different starting point and will do the summation of sin(local_a), sin(local_a+h), ...... sin( local_a+(num-1)h )
    	send_msg=0; //This will be local to each thread and will store the sum of sin(local_a), sin(local_a+h), ...... sin( local_a+(num-1)h )

    	for(i=0;i<num;i++)
    	{
    		x=local_a+(i*h) ;
    		send_msg+=sin(x);
    	}

    	// Following a master and slave model
     if(rank==0)
    {
    	answer=send_msg; //To store the sum done by thread 0
    	for(i=1;i<size;i++) //To receive and add the sums done by threads 1 to (size-1)
		{
			MPI_Recv(&recv_msg, 1, MPI_DOUBLE, i, MPI_ANY_TAG, MPI_COMM_WORLD, &status ); 
			answer+=recv_msg; //Adding the sums done by the threads 1 to (size-1)

		}

		answer=h*(sin(a)+sin(b)+answer); //We had calculated sin(a+h)+sin(a+2h).....sin(a+(n-1)h)... Now we complete the formula integral = h*(0.5sin(a)+ sin(a+h)+sin(a+2h).....sin(a+(n-1)h) + 0.5sin(b) )

		error =100*(answer-2)/2; 

		time2= MPI_Wtime();	
		total_time	= time2-time1;

		printf("Answer = %0.9f \n", answer);
		printf("N = %d \n", N);
		printf("Time = %0.9f \n", total_time);
		printf("Error = %0.9f \n", error);
		printf("Number of threads = %d \n", size);
    }

    else
    	MPI_Send(&send_msg, 1, MPI_DOUBLE, 0, 10, MPI_COMM_WORLD); //send it to the master

	MPI_Finalize();

	return 0;

} 