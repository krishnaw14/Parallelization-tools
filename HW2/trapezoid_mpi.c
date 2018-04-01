#include <stdio.h>
#include <math.h>
#include <mpi.h>

#define pi 3.141592653589
#define N 100000000

int main(int argc, char ** argv)
{
	double a,b, time1, time2, total_time, error;
	double send_msg, recv_msg, sum=0; 
	int i, num;
	double h, answer, x, local_a, range;

	int rank, size;

	a=0;
	b=pi;
	h=(b-a)/N;
	
	MPI_Status status;



	MPI_Init(&argc, &argv);

	time1 = MPI_Wtime();

    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD,&size);

    num = N/(size);
    range=(b-a)/size;

    	local_a=a + rank*range;
    	send_msg=0;
    	for(i=0;i<num;i++)
    	{
    		x=local_a+(i*h) ;
    		send_msg+=sin(x);
    	}

     if(rank==0)
    {
    	answer=send_msg;
    	for(i=1;i<size;i++)
		{
			MPI_Recv(&recv_msg, 1, MPI_DOUBLE, i, MPI_ANY_TAG, MPI_COMM_WORLD, &status );
			answer+=recv_msg;

		}

		answer=h*(sin(a)+sin(b)+answer);

		error =100*(answer-2)/2;

		time2= MPI_Wtime();	
		total_time	= time2-time1;

		printf("Answer = %0.9f \n", answer);
		printf("Time = %0.9f \n", total_time);
		printf("Error = %0.9f \n", error);
		printf("Number of threads = %d \n", size);
    }

    else
    	MPI_Send(&send_msg, 1, MPI_DOUBLE, 0, 10, MPI_COMM_WORLD);

	MPI_Finalize();

	return 0;

} 