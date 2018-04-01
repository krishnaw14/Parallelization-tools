#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

#define N 10000000
#define pi 3.141592653589

int main(int argc, char ** argv)
{
	int rank, size, i, num;
	double x,y,sum=0, answer, error; 
	int recv_msg, send_msg;
	double time1, time2, total_time;

	MPI_Status status;

    MPI_Init(&argc,&argv);

	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD,&size);

    num=N/(size);
	send_msg=0;
	for(i =0;i<num;i++)
	{
		x=(rand()*pi)/RAND_MAX;
		y=(rand()*1.0)/RAND_MAX;

		if(y-sin(x)<=0)
		{
			send_msg += 1;
		}

	}	

	if (rank==0)
	{
		sum=send_msg;
		for(i=1;i<size;i++)
		{
			MPI_Recv(&recv_msg, 1, MPI_INT, i, MPI_ANY_TAG, MPI_COMM_WORLD, &status );
			sum+=recv_msg;

		}		
		answer=(sum/N)*pi;
		printf("Answer = %0.5f \n", answer);

	}

	else
		MPI_Send(&send_msg, 1, MPI_INT, 0, 10, MPI_COMM_WORLD);

	MPI_Finalize();

	return 0;
}