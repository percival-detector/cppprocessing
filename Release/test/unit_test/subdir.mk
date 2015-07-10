################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../test/unit_test/test_ADC_decode.cpp \
../test/unit_test/test_main.cpp 

OBJS += \
./test/unit_test/test_ADC_decode.o \
./test/unit_test/test_main.o 

CPP_DEPS += \
./test/unit_test/test_ADC_decode.d \
./test/unit_test/test_main.d 


# Each subdirectory must supply rules for building sources it contributes
test/unit_test/%.o: ../test/unit_test/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


