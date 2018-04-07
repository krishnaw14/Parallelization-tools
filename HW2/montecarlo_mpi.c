#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

#define N 10000
#define pi 3.141592653589

int main(int argc, char ** argv)
{
	int rank, size, i, num;
	double x,y,sum=0, answer, error; 
	int recv_msg, send_msg;
	double time1, time2, total_time;

	MPI_Status status;

    MPI_Init(&argc,&argv);

    time1 = MPI_Wtime();

	MPI_Comm_rank(MPI_COMM_WORLD,&rank); // Rank is the identifier of a processes
    MPI_Comm_size(MPI_COMM_WORLD,&size); // size stores the total number of processes created

    num=N/(size); // Equally dividing the number of sampling points between each thread
	send_msg=0; // this is local to each thread and stores the number of points (out of num) lying within the curve sin(x)
	for(i =0;i<num;i++)
	{
		x=(rand()*pi)/RAND_MAX;
		y=(rand()*1.0)/RAND_MAX;

		if(y-sin(x)<=0) // To check if (x,y) lies withinn the curve y = sin(x)
		{
			send_msg += 1;  
		}

	}	

	//Will be following master and slave model

	if (rank==0)
	{
		sum=send_msg; // To store the sum done by thread 0
		for(i=1;i<size;i++) // To receive and add the sums done by processes 1 to (size-1)
		{
			MPI_Recv(&recv_msg, 1, MPI_INT, i, MPI_ANY_TAG, MPI_COMM_WORLD, &status );
			sum+=recv_msg; //adding the calculation done by processes 1 to (size-1)

		}	
 
		answer=(sum/N)*pi;  //sum stores the total points lying within the curve y = sin(x) out of N sampling points. Our sample space is the pix1 area containing the curve y = sin(x).

		error =100*(answer-2)/2;
		time2 = MPI_Wtime();
		total_time = time2- time1;

		printf("Answer = %0.5f \n", answer);
		printf("N = %d \n", N);
		printf("Time = %0.5f \n", total_time);
		printf("Error = %0.5f \n", error);
		printf("Number of processes = %d \n", size);

	}

	else
		MPI_Send(&send_msg, 1, MPI_INT, 0, 10, MPI_COMM_WORLD); //send the send_msg to master

	MPI_Finalize();

	return 0;
}