################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/HyperSplit/HyperSplit.cpp \
../src/HyperSplit/HyperSplit_nodes.cpp 

OBJS += \
./src/HyperSplit/HyperSplit.o \
./src/HyperSplit/HyperSplit_nodes.o 

CPP_DEPS += \
./src/HyperSplit/HyperSplit.d \
./src/HyperSplit/HyperSplit_nodes.d 


# Each subdirectory must supply rules for building sources it contributes
src/HyperSplit/%.o: ../src/HyperSplit/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++14 -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


