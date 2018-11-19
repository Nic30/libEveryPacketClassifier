################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/TupleMerge/SlottedTable.cpp \
../src/TupleMerge/TupleMergeOffline.cpp \
../src/TupleMerge/TupleMergeOnline.cpp 

OBJS += \
./src/TupleMerge/SlottedTable.o \
./src/TupleMerge/TupleMergeOffline.o \
./src/TupleMerge/TupleMergeOnline.o 

CPP_DEPS += \
./src/TupleMerge/SlottedTable.d \
./src/TupleMerge/TupleMergeOffline.d \
./src/TupleMerge/TupleMergeOnline.d 


# Each subdirectory must supply rules for building sources it contributes
src/TupleMerge/%.o: ../src/TupleMerge/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++14 -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


