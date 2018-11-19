################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Trees/TreeUtils.cpp 

OBJS += \
./src/Trees/TreeUtils.o 

CPP_DEPS += \
./src/Trees/TreeUtils.d 


# Each subdirectory must supply rules for building sources it contributes
src/Trees/%.o: ../src/Trees/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++14 -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


