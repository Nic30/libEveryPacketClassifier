################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/HyperCuts/HyperCuts.cpp 

OBJS += \
./src/HyperCuts/HyperCuts.o 

CPP_DEPS += \
./src/HyperCuts/HyperCuts.d 


# Each subdirectory must supply rules for building sources it contributes
src/HyperCuts/%.o: ../src/HyperCuts/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++14 -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


