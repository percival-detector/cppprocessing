################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../profile/main.cpp 

OBJS += \
./profile/main.o 

CPP_DEPS += \
./profile/main.d 


# Each subdirectory must supply rules for building sources it contributes
profile/%.o: ../profile/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"/home/pqm78245/percivalProcessing/cppProcessing2.0/Include" -I"/home/pqm78245/percivalProcessing/cppProcessing2.0/src" -I/dls_sw/prod/tools/RHEL6-x86_64/hdf5/1-8-14/prefix/include -O3 -pg -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


