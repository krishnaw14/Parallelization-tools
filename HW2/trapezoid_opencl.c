
// To run the file on terminal of mac: $ gcc trapezoid_opencl.c -framework Opencl 
// 									   $ ./a.out


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <sys/types.h>
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#include <unistd.h>
#else
#include <CL/cl.h>
#endif

#define N 1024 //Number of Sampling points
#define MAX_SOURCE_SIZE (0x100000)
#define pi 3.141592653589

void checkError(cl_int err, char* msg) //To print the error and terminate the program if any error occurs during the opencl API calls
{
	if(err != CL_SUCCESS)
	{
		printf("\n %s \n", msg);
		exit(0);
	}
}

int main(int argc, char** argv)
{
	float *workGroupSums; // Array to hold the sums evaluated by each work group
    int numPoints = N;    // default number of steps (updated later to device preferable)  
    float a, b, h;
    size_t num_work_groups;
    size_t work_group_size; 
    float answer,error; //We will add the work group sums and multiply that with h to get the final answer
    double total_time;
    clock_t time1, time2;

    a=0; b=pi; h = (b-a)/numPoints;

    workGroupSums = calloc(sizeof(float), num_work_groups);  //Allocate memory in the host for the work sroup sums

    cl_mem d_workGroupSums; //Memory location of the work group sums

    //Load the kernel
   	FILE *fp;
	const char fileName[]="./trapezoid.cl";
	size_t source_size;
	char* source_str;
	cl_int i;

	fp = fopen(fileName, "r");
	if(!fp)
	{
		fprintf(stderr, "Failed to load kernel.\n");
		exit(1);
	}
	source_str = (char*) malloc(MAX_SOURCE_SIZE);
	source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
	fclose(fp);

	cl_int err;
    cl_device_id        device;     
    cl_context       context;       
    cl_command_queue commands;      
    cl_program       program;       
    cl_kernel        kernel;     
    cl_uint numPlatforms;

    err = clGetPlatformIDs(0, NULL, &numPlatforms);
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

	//Get a GPU
	for(i=0;i<numPlatforms;i++)
	{
		err = clGetDeviceIDs(Platform[i], CL_DEVICE_TYPE_GPU, 1, &device, NULL);
		if(err == CL_SUCCESS)
			break;
	}
	if (device == NULL)
	{
		printf("Error in finding a GPU platform \n");
		exit(0);
	}

	time1 =clock();

	context = clCreateContext(0, 1, &device, NULL, NULL, &err);
	checkError(err, "Unable to create context for the GPU device");

	commands = clCreateCommandQueue(context, device, 0, &err); 
	checkError(err, "Unable to create a command queue for the context");

	program = clCreateProgramWithSource(context, 1, (const char **) & source_str, NULL, &err);
	checkError(err, "Unable to create program from source");

	err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
	checkError(err, "Unable to build program program");

	kernel = clCreateKernel(program, "pi", &err);
	checkError(err, "Unable to create kernel");

	err = clGetKernelWorkGroupInfo (kernel, device, CL_KERNEL_WORK_GROUP_SIZE, sizeof(size_t), &work_group_size, NULL);
	checkError(err, "Unable to get info on work groups");
	num_work_groups = numPoints/(work_group_size);

    d_workGroupSums = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(float) * num_work_groups, NULL, &err);

    err = clSetKernelArg(kernel, 0, sizeof(float), &h);
    err = clSetKernelArg(kernel, 1, sizeof(float) * work_group_size, NULL);
    err = clSetKernelArg(kernel, 2, sizeof(cl_mem), &d_workGroupSums);

    size_t global = numPoints ;
    size_t local = work_group_size;
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
    answer *= h;
    error=100.0*(answer-2)/2;
    time2= clock();

    total_time =(double)(time2-time1)/(CLOCKS_PER_SEC);

    printf("ANSWER = %0.5f \n", answer);
    printf("ERROR = %0.5f \n", error);
    printf("TOTAL TIME: %f \n", total_time);

    free(source_str);
    free(workGroupSums);
    clReleaseProgram(program);
    clReleaseKernel(kernel);
    clReleaseCommandQueue(commands);
    clReleaseContext(context);
    clReleaseMemObject(d_workGroupSums);


    return 0;

}