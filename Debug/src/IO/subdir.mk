################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/IO/InputReader.cpp \
../src/IO/OutputWriter.cpp 

OBJS += \
./src/IO/InputReader.o \
./src/IO/OutputWriter.o 

CPP_DEPS += \
./src/IO/InputReader.d \
./src/IO/OutputWriter.d 


# Each subdirectory must supply rules for building sources it contributes
src/IO/%.o: ../src/IO/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++14 -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


