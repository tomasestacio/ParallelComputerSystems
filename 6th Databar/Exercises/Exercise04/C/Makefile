
ifndef CC
	CC = gcc
endif

CCFLAGS=-O3 -lm

LIBS = -lOpenCL -fopenmp

CL_DIR = /zhome/bb/5/41921/Public/OpenCL/AMDAPPSDK-3.0/include
LIB_DIR = /zhome/bb/5/41921/Public/OpenCL/AMDAPPSDK-3.0/lib/x86_64

COMMON_DIR = ../../C_common

# Change this variable to specify the device type
# to the OpenCL device type of choice. You can also
# edit the variable in the source.
ifndef DEVICE
	DEVICE = CL_DEVICE_TYPE_DEFAULT
endif

# Check our platform and make sure we define the APPLE variable
# and set up the right compiler flags and libraries
PLATFORM = $(shell uname -s)
ifeq ($(PLATFORM), Darwin)
	LIBS = -framework OpenCL 
endif

CCFLAGS += -D DEVICE=$(DEVICE)

vadd: vadd_c.c $(COMMON_DIR)/wtime.c $(COMMON_DIR)/device_info.c
	$(CC) $^ $(CCFLAGS) $(LIBS) -I $(COMMON_DIR) -I $(CL_DIR) -L$(LIB_DIR) -o $@


clean:
	rm -f vadd
