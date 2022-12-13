#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#include <unistd.h>
#else
#include <CL/cl.h>
#endif

#include "err_code.h"

//pick up device type from compiler command line or from
//the default type
#ifndef DEVICE
#define DEVICE CL_DEVICE_TYPE_DEFAULT
#endif

extern double wtime();       // returns time since some fixed past point (wtime.c)
extern int output_device_info(cl_device_id );


//------------------------------------------------------------------------------

#define INTERVALS 10000000

// Kernel code should have the initialization of the array from[] with 0.0 and the calculation of the array to[]
// Host program should have the serial code
// Create buffers for from and to

const char *KernelSource = "\n" \
"__kernel void calc1(__global double* from, const unsigned int count)                                        \n" \
"{                                                                                                           \n" \
"    int i = get_global_id(0);                                                                               \n" \
"    if(i < count-1 && i != 0)                                                                               \n" \
"     from[i] = 0.0;                                                                                         \n" \
"                                                                                                            \n" \
"}                                                                                                           \n" \
"__kernel void calc2(__global double* from, __global double* to, const unsigned int count, int time_steps)   \n" \
"{                                                                                                           \n" \
"   int i = get_global_id(0);                                                                                \n" \
"   while(time_steps-- > 0){                                                                                 \n" \
"     if(i < count-1 && i != 0)                                                                              \n" \
"       to[i] = from[i] + 0.1*(from[i-1] - 2*from[i] + from[i+1]);                                           \n" \
"                                                                                                            \n" \
"     __global double* tmp = from;                                                                                    \n" \
"     from = to;                                                                                             \n" \
"     to = tmp;                                                                                             \n" \
"   }                                                                                                        \n" \
"}                                                                                                           \n" \
"\n";

int main(int argc, char **argv)
{
  /* Declare and initialize data */

  int err;

  double time2;
  time_t time1 = clock();
  double *h_to = (double*) malloc(INTERVALS * sizeof(double));
  double *h_from = (double*) malloc(INTERVALS * sizeof(double));
  int    time_steps = 100;
  unsigned int count = INTERVALS;

  size_t global;                  // global domain size

  cl_device_id     device_id;     // compute device id
  cl_context       context;       // compute context
  cl_command_queue commands;      // compute command queue
  cl_program       program;       // compute program
  cl_kernel        ko_calc1;      // compute kernel#1
  cl_kernel        ko_calc2;      // compute kernel#2

  cl_mem d_to;
  cl_mem d_from;

  /* Set up initial and boundary conditions. */
  h_from[0] = 1.0;
  h_from[INTERVALS - 1] = 0.0;
  h_to[0] = h_from[0];
  h_to[INTERVALS - 1] = h_from[INTERVALS - 1];

  printf("Number of intervals: %ld. Number of time steps: %d\n", INTERVALS, time_steps);
  
  /* Set up platform and GPU device */

  cl_uint numPlatforms;

  //Find # of platforms 
  err = clGetPlatformIDs(0, NULL, &numPlatforms);
  checkError(err, "Finding platforms");
  if(numPlatforms == 0){
    printf("Found 0 platforms!\n");
    return EXIT_FAILURE;
  }

  // Get all platforms
  cl_platform_id Platform[numPlatforms];
  err = clGetPlatformIDs(numPlatforms, Platform, NULL);
  checkError(err, "Getting platforms");

  // Secure a GPU
  for(int i = 0; i < numPlatforms; i++){
    err = clGetDeviceIDs(Platform[i], CL_DEVICE_TYPE_GPU, 1, &device_id, NULL);
    if (err == CL_SUCCESS) break;
  }

  if(device_id == NULL) checkError(err, "Finding a device");

  err = output_device_info(device_id);
  checkError(err, "Printing device output");

  // Create a compute context
  context = clCreateContext(0, 1, &device_id, NULL, NULL, &err);
  checkError(err, "Creating context");

  // Create a command queue
  commands = clCreateCommandQueue(context, device_id, 0, &err);
  checkError(err, "Creating command queue");

  // Create the compute program from the source buffer
  program = clCreateProgramWithSource(context, 1, (const char **) & KernelSource, NULL, &err);
  checkError(err, "Creating program");

  // Build the program
  err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
  if(err != CL_SUCCESS){
    size_t len;
    char buffer[2048];

    printf("Error: Failed to build program executable!\n%s\n", err_code(err));
    clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &len);
    printf("%s\n", buffer);
    return EXIT_FAILURE;
  }

  // Create the compute kernel from the program
  ko_calc1 = clCreateKernel(program, "calc1", &err);
  checkError(err, "Creating kernel#1");

  ko_calc2 = clCreateKernel(program, "calc2", &err);
  checkError(err, "Creating kernel#2");

  // Create input to and from and send to kernel
  d_to = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, sizeof(double) * INTERVALS, h_to, &err);
  checkError(err, "Creating buffer d_to");

  d_from = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, sizeof(double) * INTERVALS, h_from, &err);
  checkError(err, "Creating buffer d_from");

  // Set the arguments to our compute kernel
  err = clSetKernelArg(ko_calc1, 0, sizeof(cl_mem), &d_from);
  err |= clSetKernelArg(ko_calc1, 1, sizeof(unsigned int), &count);
  checkError(err, "Setting kernel#1 arguments");

  double rtime1 = wtime();

  // Execute the kernel
  // Let OpenCL routine choose the work-group size
  global = count;
  err = clEnqueueNDRangeKernel(commands, ko_calc1, 1, NULL, &global, NULL, 0, NULL, NULL);
  checkError(err, "Enqueueing kernel#1");

  // Wait for the commands to complete before stopping the timer
  //err = clFinish(commands);
  //checkError(err, "Waiting for kernel#1 to finish");

  rtime1 = wtime() - rtime1;
  printf("\nThe kernel#1 ran in %lf seconds\n", rtime1);

  err = clSetKernelArg(ko_calc2, 0, sizeof(cl_mem), &d_from);
  err |= clSetKernelArg(ko_calc2, 1, sizeof(cl_mem), &d_to);
  err |= clSetKernelArg(ko_calc2, 2, sizeof(unsigned int), &count);
  err |= clSetKernelArg(ko_calc2, 3, sizeof(int), &time_steps);
  checkError(err, "Setting kernel#2 arguments");

  double rtime2 = wtime();

  err = clEnqueueNDRangeKernel(commands, ko_calc2, 1, NULL, &global, NULL, 0, NULL, NULL);
  checkError(err, "Enqueueing kernel#2");

  err = clFinish(commands);
  checkError(err, "Witing for kernel#2 to finish");

  rtime2 = wtime() - rtime2;
  printf("\nThe kernel#2 ran in %lf seconds\n", rtime2);

  err = clEnqueueReadBuffer(commands, d_to, CL_TRUE, 0, sizeof(double) * count, h_to, 0, NULL, NULL);
  if(err != CL_SUCCESS){
    printf("Error: Failed to read output array!\n%s\n", err_code(err));
    exit(1);
  }

  // Summarise results
  time2 = (clock() - time1) / (double) CLOCKS_PER_SEC;

  printf("Elapsed time (s) = %f\n", time2);

  for(long i = 2; i < 30; i += 2)
   printf("Interval %ld: %f\n", i, h_to[i]);
  
  clReleaseMemObject(d_to);
  clReleaseMemObject(d_from);
  clReleaseProgram(program);
  clReleaseKernel(ko_calc1);
  clReleaseKernel(ko_calc2);
  clReleaseCommandQueue(commands);
  clReleaseContext(context);

  free(h_from);
  free(h_to);

  return 0;

}        
