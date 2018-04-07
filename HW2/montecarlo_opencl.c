

// To run the file on terminal of mac: $ gcc montecarlo_opencl.c -framework Opencl 
// 									   $ ./a.out

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <sys/types.h>
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#include <unistd.h>
#else
#include <CL/cl.h>
#endif

#define N 512  //Number of Sampling points
#define MAX_SOURCE_SIZE (0x100000)
#define pi 3.141592653589 

void checkError(cl_int err, char* msg) //To print the error and terminate the program if any error occurs during the opencl API calls
{
	if(err != CL_SUCCESS)
	{
		printf("\n %s \n", msg);
		exit(0);
	}}

int main(int argc, char** argv)
{
	float *workGroupSums; // Array to hold the sums evaluated by each work group
    int numPoints = N;    // default number of steps (updated later to device preferable)  
    float a, b, h;
    size_t num_work_groups;
    size_t work_group_size; 
    float answer,error, total_time; //We will add the work group sums and multiply that with h to get the final answer

    clock_t time1, time2;

    workGroupSums = calloc(sizeof(float), num_work_groups);  //Allocate memory in the host for the work sroup sums

    a=0; b=pi; h = (b-a)/numPoints;

    cl_mem d_workGroupSums; //Memory location of the work group sums

    //Load the kernel
   	FILE *fp;
	const char fileName[]="./montecarlo.cl"; //This .cl file is present in the same directory as the montecarlo_opencl.c
	size_t source_size;
	char* source_str;
	cl_int i;

	fp = fopen(fileName, "r");
	if(!fp)
	{
		fprintf(stderr, "Failed to load kernel.\n");
		exit(1);
	}
	source_str = (char*) malloc(MAX_SOURCE_SIZE); //create char* for the kernel
	source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
	fclose(fp);

	cl_int err;
    cl_device_id     device_id;     // find device id
    cl_context       context;       // find context
    cl_command_queue commands;      // find command queue
    cl_program       program;       // find program
    cl_kernel        kernel;     // find kernel
    cl_uint numPlatforms;

    err = clGetPlatformIDs(0, NULL, &numPlatforms); //this API will give all the available OPencl platforms available
	if(err!=CL_SUCCESS)
	{
		printf("Unable to find any opencl platforms\n");
		exit(0);
	}
	if(numPlatforms==0)
	{
		printf("No opencl platforms available \n");
		exit(0);
	}

	cl_platform_id Platform[numPlatforms]; 
	err = clGetPlatformIDs(numPlatforms, Platform, NULL);

	//Get a GPU device for the computations
	for(i=0;i<numPlatforms;i++)
	{
		err = clGetDeviceIDs(Platform[i], CL_DEVICE_TYPE_GPU, 1, &device_id, NULL);
		if(err == CL_SUCCESS)
			break;
	}
	if (device_id == NULL)
	{
		printf("Error in finding a GPU platform \n");
		exit(0);
	}

	time1 =clock(); //Start the time after the GPU device is found on which the computation has to be performed

	context = clCreateContext(0, 1, &device_id, NULL, NULL, &err); 
	checkError(err, "Unable to create context for the GPU device");

	commands = clCreateCommandQueue(context, device_id, 0, &err); 
	checkError(err, "Unable to create a command queue for the context");

	program = clCreateProgramWithSource(context, 1, (const char **) & source_str, NULL, &err);
	checkError(err, "Unable to create program from source");

	err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL); 
	checkError(err, "Unable to build program program");

	kernel = clCreateKernel(program, "monte", &err); //the name monte refers to the function name in the kernel defined for this program
	checkError(err, "Unable to create kernel");

	//Get the info about the work groups from this API and then accordingly create the number of work groups based on the number of sampling points
	err = clGetKernelWorkGroupInfo (kernel, device_id, CL_KERNEL_WORK_GROUP_SIZE, sizeof(size_t), &work_group_size, NULL);
	checkError(err, "Unable to get info on work groups");
	num_work_groups = numPoints/(work_group_size);


    //Creating memory space in device for the local sums of the work-groups
    d_workGroupSums = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(float) * num_work_groups, NULL, &err);

    //Setting up the arguments of the kernel
    err = clSetKernelArg(kernel, 0, sizeof(float), &h); 
    err = clSetKernelArg(kernel, 1, sizeof(float) * work_group_size, NULL);
    err = clSetKernelArg(kernel, 2, sizeof(cl_mem), &d_workGroupSums);

    size_t global = numPoints ; //Global space of the work groups 
    size_t local = work_group_size; //Local space of the work groups 
    //double rtime = wtime();
    err = clEnqueueNDRangeKernel(commands, kernel, 1, NULL, &global, &local, 0, NULL, NULL);
	checkError(err, "Error in enqueueing the kernel");

    err = clEnqueueReadBuffer(commands,d_workGroupSums, CL_TRUE, 0, sizeof(float) * num_work_groups, workGroupSums, 0, NULL, NULL);
    checkError(err, "Error in copying workGroupSums from device to host");
    answer = 0.0f;
    for (unsigned int i = 0; i < num_work_groups; i++)
    {
    	//printf("workGroupSum[%d]  = %f \n", i, workGroupSums[i]);
        answer += workGroupSums[i];
    }
    answer *= (pi/N); //We are taking the sampling points in a rectangle of pix1.
    error = (answer-2)*50; //Actual Answer = 2
    time2=clock();
    total_time =(double)(time2-time1)/(CLOCKS_PER_SEC);

    printf("ANSWER = %f \n", answer);
    printf("ERROR = %f \n", error);
    printf("TOTAL TIME = %f \n", total_time);

    free(source_str);
    free(workGroupSums);
    clReleaseMemObject(d_workGroupSums);
    clReleaseProgram(program);
    clReleaseKernel(kernel);
    clReleaseCommandQueue(commands);
    clReleaseContext(context);


    return 0;

}