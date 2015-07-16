################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../h5cpp_examples/h5tutr_cmprss.cpp \
../h5cpp_examples/h5tutr_crtatt.cpp \
../h5cpp_examples/h5tutr_crtdat.cpp \
../h5cpp_examples/h5tutr_crtgrp.cpp \
../h5cpp_examples/h5tutr_crtgrpar.cpp \
../h5cpp_examples/h5tutr_crtgrpd.cpp \
../h5cpp_examples/h5tutr_extend.cpp \
../h5cpp_examples/h5tutr_rdwt.cpp \
../h5cpp_examples/h5tutr_subset.cpp 

OBJS += \
./h5cpp_examples/h5tutr_cmprss.o \
./h5cpp_examples/h5tutr_crtatt.o \
./h5cpp_examples/h5tutr_crtdat.o \
./h5cpp_examples/h5tutr_crtgrp.o \
./h5cpp_examples/h5tutr_crtgrpar.o \
./h5cpp_examples/h5tutr_crtgrpd.o \
./h5cpp_examples/h5tutr_extend.o \
./h5cpp_examples/h5tutr_rdwt.o \
./h5cpp_examples/h5tutr_subset.o 

CPP_DEPS += \
./h5cpp_examples/h5tutr_cmprss.d \
./h5cpp_examples/h5tutr_crtatt.d \
./h5cpp_examples/h5tutr_crtdat.d \
./h5cpp_examples/h5tutr_crtgrp.d \
./h5cpp_examples/h5tutr_crtgrpar.d \
./h5cpp_examples/h5tutr_crtgrpd.d \
./h5cpp_examples/h5tutr_extend.d \
./h5cpp_examples/h5tutr_rdwt.d \
./h5cpp_examples/h5tutr_subset.d 


# Each subdirectory must supply rules for building sources it contributes
h5cpp_examples/%.o: ../h5cpp_examples/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"/home/pqm78245/percivalProcessing/2.0/Include" -I/home/pqm78245/lib/h5_cpp/include -I/home/pqm78245/lib/h5_cpp/bin -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


