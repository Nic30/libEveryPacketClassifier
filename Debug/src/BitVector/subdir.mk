################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/BitVector/BitSet.cpp \
../src/BitVector/BitVector.cpp \
../src/BitVector/EqnMatcher.cpp \
../src/BitVector/LongestPrefixMatch.cpp 

OBJS += \
./src/BitVector/BitSet.o \
./src/BitVector/BitVector.o \
./src/BitVector/EqnMatcher.o \
./src/BitVector/LongestPrefixMatch.o 

CPP_DEPS += \
./src/BitVector/BitSet.d \
./src/BitVector/BitVector.d \
./src/BitVector/EqnMatcher.d \
./src/BitVector/LongestPrefixMatch.d 


# Each subdirectory must supply rules for building sources it contributes
src/BitVector/%.o: ../src/BitVector/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++14 -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


