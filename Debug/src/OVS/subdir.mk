################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/OVS/TupleSpaceSearch.cpp \
../src/OVS/cmap.cpp 

OBJS += \
./src/OVS/TupleSpaceSearch.o \
./src/OVS/cmap.o 

CPP_DEPS += \
./src/OVS/TupleSpaceSearch.d \
./src/OVS/cmap.d 


# Each subdirectory must supply rules for building sources it contributes
src/OVS/%.o: ../src/OVS/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++14 -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


