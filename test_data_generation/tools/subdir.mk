################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../tools/HDF5_test_file_generator.cpp 

OBJS += \
./tools/HDF5_test_file_generator.o 

CPP_DEPS += \
./tools/HDF5_test_file_generator.d 


# Each subdirectory must supply rules for building sources it contributes
tools/%.o: ../tools/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/home/pqm78245/lib/h5_cpp/include -I/home/pqm78245/lib/h5_cpp/bin -I/home/pqm78245/lib/h5_cpp/share -I"/home/pqm78245/percivalProcessing/cppProcessing2.0/src" -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


