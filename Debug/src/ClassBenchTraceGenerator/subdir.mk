################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/ClassBenchTraceGenerator/trace_tools.cc 

CC_DEPS += \
./src/ClassBenchTraceGenerator/trace_tools.d 

OBJS += \
./src/ClassBenchTraceGenerator/trace_tools.o 


# Each subdirectory must supply rules for building sources it contributes
src/ClassBenchTraceGenerator/%.o: ../src/ClassBenchTraceGenerator/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++14 -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


