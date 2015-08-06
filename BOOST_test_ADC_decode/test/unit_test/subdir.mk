################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../test/unit_test/test_ADC_decode.cpp \
../test/unit_test/test_HDF5_loader.cpp \
../test/unit_test/test_HDF5_writer.cpp \
../test/unit_test/test_main.cpp \
../test/unit_test/test_percival_frame.cpp 

OBJS += \
./test/unit_test/test_ADC_decode.o \
./test/unit_test/test_HDF5_loader.o \
./test/unit_test/test_HDF5_writer.o \
./test/unit_test/test_main.o \
./test/unit_test/test_percival_frame.o 

CPP_DEPS += \
./test/unit_test/test_ADC_decode.d \
./test/unit_test/test_HDF5_loader.d \
./test/unit_test/test_HDF5_writer.d \
./test/unit_test/test_main.d \
./test/unit_test/test_percival_frame.d 


# Each subdirectory must supply rules for building sources it contributes
test/unit_test/test_ADC_decode.o: ../test/unit_test/test_ADC_decode.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -I"/home/pqm78245/percivalProcessing/cppProcessing2.0/Include" -I"/home/pqm78245/percivalProcessing/cppProcessing2.0/src" -I/dls_sw/prod/tools/RHEL6-x86_64/hdf5/1-8-14/prefix/include -O0 -g3 -pg -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"test/unit_test/test_ADC_decode.d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

test/unit_test/%.o: ../test/unit_test/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -I"/home/pqm78245/percivalProcessing/cppProcessing2.0/Include" -I"/home/pqm78245/percivalProcessing/cppProcessing2.0" -I"/home/pqm78245/percivalProcessing/cppProcessing2.0/src" -I/dls_sw/prod/tools/RHEL6-x86_64/hdf5/1-8-14/prefix/include -O0 -g3 -pg -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


