################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/ByteCuts/ByteCuts.cpp \
../src/ByteCuts/ByteCutsNode.cpp \
../src/ByteCuts/TreeBuilder.cpp 

OBJS += \
./src/ByteCuts/ByteCuts.o \
./src/ByteCuts/ByteCutsNode.o \
./src/ByteCuts/TreeBuilder.o 

CPP_DEPS += \
./src/ByteCuts/ByteCuts.d \
./src/ByteCuts/ByteCutsNode.d \
./src/ByteCuts/TreeBuilder.d 


# Each subdirectory must supply rules for building sources it contributes
src/ByteCuts/%.o: ../src/ByteCuts/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++14 -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


