################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/PartitionSort/OptimizedMITree.cpp \
../src/PartitionSort/PartitionSort.cpp \
../src/PartitionSort/SortableRulesetPartitioner.cpp \
../src/PartitionSort/red_black_tree.cpp \
../src/PartitionSort/test_red_black_tree.cpp 

OBJS += \
./src/PartitionSort/OptimizedMITree.o \
./src/PartitionSort/PartitionSort.o \
./src/PartitionSort/SortableRulesetPartitioner.o \
./src/PartitionSort/red_black_tree.o \
./src/PartitionSort/test_red_black_tree.o 

CPP_DEPS += \
./src/PartitionSort/OptimizedMITree.d \
./src/PartitionSort/PartitionSort.d \
./src/PartitionSort/SortableRulesetPartitioner.d \
./src/PartitionSort/red_black_tree.d \
./src/PartitionSort/test_red_black_tree.d 


# Each subdirectory must supply rules for building sources it contributes
src/PartitionSort/%.o: ../src/PartitionSort/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++14 -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


