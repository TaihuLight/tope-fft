#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <math.h>

#ifndef TOPEFFT_H
#define TOPEFFT_H
#include "topefft.h"
#endif

cl_ulong profileThis(cl_event a) {
	cl_ulong start = 0, end = 0;
	clGetEventProfilingInfo(a, CL_PROFILING_COMMAND_END, 
							sizeof(cl_ulong), &end, NULL);
	clGetEventProfilingInfo(a, CL_PROFILING_COMMAND_START, 
							sizeof(cl_ulong), &start, NULL);
	return (end - start);
}

void topeFFTInit(struct topeFFT *f)
{
	// Platform
	f->error = clGetPlatformIDs(1, &f->platform_id, &f->ret_num_platforms);
	$CHECKERROR

	// Device
	f->error = clGetDeviceIDs(	f->platform_id, 
								CL_DEVICE_TYPE_DEFAULT, 1, &f->device, 
								&f->ret_num_devices);
	$CHECKERROR

	// Some Info
	size_t info_size;
	cl_char info[1024];
	cl_ulong info_num;
	f->error = clGetDeviceInfo(	f->device, CL_DEVICE_VENDOR, sizeof(info), 
								info, &info_size);
	fprintf(stderr,"Device to Use: %s\n", info);

	f->error = clGetDeviceInfo(	f->device, CL_DEVICE_NAME, sizeof(info), 
								info, &info_size);
	fprintf(stderr,"%s\n", info);

	f->error = clGetDeviceInfo(	f->device, CL_DEVICE_GLOBAL_MEM_SIZE, 
								sizeof(info_num), &info_num, &info_size);
	fprintf(stderr,"Global Memory: %f Mb\n", (float)(info_num/1024/1024));
	f->error = clGetDeviceInfo(f->device, CL_DEVICE_LOCAL_MEM_SIZE, 
								sizeof(info_num), &info_num, &info_size);
	fprintf(stderr,"Local Memory Size: %f Kb\n", (float)(info_num/1024));
	
	f->error = clGetDeviceInfo(	f->device, 
								CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE, 
								sizeof(info), &info, &info_size);
	fprintf(stderr,"double Support: %s\n", info == 0 ? "no" : "yes"); 

	// Context
	f->context = clCreateContext(NULL, 1, &f->device, NULL, NULL, &f->error);
	$CHECKERROR

	// Command Queue 	
	f->command_queue = clCreateCommandQueue(f->context, f->device, 
											CL_QUEUE_PROFILING_ENABLE, 
											&f->error);
	$CHECKERROR

	#if 0
	#if 1 //1D
	FILE *fp; 
	char *source_buf;
	size_t source_size;
	char fileName[] = "/opt/topefft/kernels1D.ptx";
	fp = fopen(fileName, "r");
	if (!fp) {
		fprintf(stderr, "Failed to load kernels. Check path.\n");
		exit(1);
	}
	source_buf = (char*)malloc(MAX_BINARY_SIZE);
	source_size = fread(source_buf, 1, MAX_BINARY_SIZE, fp);
	fclose(fp);
	cl_int bin_status;
	f->program1D = clCreateProgramWithBinary(	f->context, 1, &f->device,
												(const size_t *)&source_size, 
												(const unsigned char **)&source_buf,
												&bin_status, &f->error);
	$CHECKERROR

	char buffer[2048];
	f->error = clBuildProgram(f->program1D, 1, &f->device, 
								"-cl-nv-verbose", NULL, NULL);
	clGetProgramBuildInfo(	f->program1D, f->device, CL_PROGRAM_BUILD_LOG, 
							sizeof(buffer), buffer, NULL);
	$CHECKERROR
	#endif

	#if 1 //2D
	char file2D[] = "/opt/topefft/kernels2D.ptx";
	fp = fopen(file2D, "r");
	if (!fp) {
		fprintf(stderr, "Failed to load kernels. Check path.\n");
		exit(1);
	}
	source_size = fread(source_buf, 1, MAX_BINARY_SIZE, fp);
	fclose(fp);
	f->program2D = clCreateProgramWithBinary(	f->context, 1, &f->device,
												(const size_t *)&source_size, 
												(const unsigned char **)&source_buf,
												&bin_status, &f->error);
	$CHECKERROR
	f->error = clBuildProgram(f->program2D, 1, &f->device, 
								"-cl-nv-verbose", NULL, NULL);
	clGetProgramBuildInfo(	f->program2D, f->device, CL_PROGRAM_BUILD_LOG, 
							sizeof(buffer), buffer, NULL);
	clBuildProgramChecker(&f->error, buffer);
	$CHECKERROR
	#endif
	
	#if 1
	char file3D[] = "/opt/topefft/kernels3D.ptx";
	fp = fopen(file3D, "r");
	if (!fp) {
		fprintf(stderr, "Failed to load kernels. Check path.\n");
		exit(1);
	}
	source_size = fread(source_buf, 1, MAX_BINARY_SIZE, fp);
	fclose(fp);
	f->program3D = clCreateProgramWithBinary(	f->context, 1, &f->device,
												(const size_t *)&source_size, 
												(const unsigned char **)&source_buf,
												&bin_status, &f->error);
	$CHECKERROR
	f->error = clBuildProgram(f->program3D, 1, &f->device, 
								"-cl-nv-verbose", NULL, NULL);
	clGetProgramBuildInfo(	f->program3D, f->device, CL_PROGRAM_BUILD_LOG, 
							sizeof(buffer), buffer, NULL);
	$CHECKERROR
	#endif

	#endif

	#if 1
	#if 1 /* Prepare 1D Program */
	// CL File		
	FILE *fp; 
	char *source_str;
	size_t source_size;
	char fileName[] = "/opt/topefft/kernels1D.cl";
	fp = fopen(fileName, "r");
	if (!fp) {
		fprintf(stderr, "Failed to load kernels. Check path.\n");
		exit(1);
	}
	source_str = (char*)malloc(MAX_SOURCE_SIZE);
	source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
	fclose(fp);
	f->program1D = clCreateProgramWithSource(	f->context, 1, 
												(const char **)&source_str, 
												(const size_t *)&source_size, 
												&f->error);
	$CHECKERROR

	// Build
	char buffer[2048];
	f->error = clBuildProgram(	f->program1D, 1, &f->device, 
								"-cl-nv-verbose", NULL, NULL);
	clGetProgramBuildInfo(	f->program1D, f->device, CL_PROGRAM_BUILD_LOG, 
							sizeof(buffer), buffer, NULL);
	size_t ret_value_size;
	clGetProgramBuildInfo(	f->program1D, f->device, CL_PROGRAM_BUILD_LOG, 0, 
							NULL, &ret_value_size);
	char *register1 = malloc(ret_value_size+1);
	clGetProgramBuildInfo(	f->program1D, f->device, CL_PROGRAM_BUILD_LOG, 
							ret_value_size, register1, NULL);
	register1[ret_value_size] = '\0';
	fprintf(stderr, "%s\n", register1);
	#endif

	#if 1 /* Prepare 2D Program */
	char file2D[] = "/opt/topefft/kernels2D.cl";
	fp = fopen(file2D, "r");
	if (!fp) {
		fprintf(stderr, "Failed to load kernels. Check path.\n");
		exit(1);
	}
	source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
	fclose(fp);
	f->program2D = clCreateProgramWithSource(	f->context, 1, 
												(const char **)&source_str, 
												(const size_t *)&source_size, 
												&f->error);
	$CHECKERROR

	// Build
	f->error = clBuildProgram(	f->program2D, 1, &f->device, 
								"-cl-nv-verbose", NULL, NULL);
	clGetProgramBuildInfo(	f->program2D, f->device, CL_PROGRAM_BUILD_LOG, 
							sizeof(buffer), buffer, NULL);
	clGetProgramBuildInfo(	f->program2D, f->device, CL_PROGRAM_BUILD_LOG, 0, 
							NULL, &ret_value_size);
	register1 = realloc(register1, ret_value_size+1);
	clGetProgramBuildInfo(	f->program2D, f->device, CL_PROGRAM_BUILD_LOG, 
							ret_value_size, register1, NULL);
	register1[ret_value_size] = '\0';
	fprintf(stderr, "%s\n", register1);
	#endif

	#if 1 /* Prepare 3D Program */
	char file3D[] = "/opt/topefft/kernels3D.cl";
	fp = fopen(file3D, "r");
	if (!fp) {
		fprintf(stderr, "Failed to load kernels. Check path.\n");
		exit(1);
	}
	source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
	fclose(fp);
	f->program3D = clCreateProgramWithSource(	f->context, 1, 
												(const char **)&source_str, 
												(const size_t *)&source_size, 
												&f->error);
	$CHECKERROR

	// Build
	f->error = clBuildProgram(	f->program3D, 1, &f->device, 
								"-cl-nv-verbose", NULL, NULL);
	clGetProgramBuildInfo(	f->program3D, f->device, CL_PROGRAM_BUILD_LOG, 
							sizeof(buffer), buffer, NULL);
	clGetProgramBuildInfo(	f->program3D, f->device, CL_PROGRAM_BUILD_LOG, 0, 
							NULL, &ret_value_size);
	register1 = realloc(register1, ret_value_size+1);
	clGetProgramBuildInfo(	f->program3D, f->device, CL_PROGRAM_BUILD_LOG, 
							ret_value_size, register1, NULL);
	register1[ret_value_size] = '\0';
	fprintf(stderr, "%s\n", register1);
	#endif
	#endif
}

