################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/percival_ADC_decode.cpp \
../src/percival_calib_params.cpp 

OBJS += \
./src/percival_ADC_decode.o \
./src/percival_calib_params.o 

CPP_DEPS += \
./src/percival_ADC_decode.d \
./src/percival_calib_params.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"/home/pqm78245/percivalProcessing/cppProcessing2.0/Include" -I/home/pqm78245/lib/h5_cpp/include -I/home/pqm78245/lib/h5_cpp/bin -I"/home/pqm78245/percivalProcessing/cppProcessing2.0/src" -O3 -pg -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


