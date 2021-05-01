################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../Core/Startup/startup_stm32l412c8tx.s 

S_DEPS += \
./Core/Startup/startup_stm32l412c8tx.d 

OBJS += \
./Core/Startup/startup_stm32l412c8tx.o 


# Each subdirectory must supply rules for building sources it contributes
Core/Startup/startup_stm32l412c8tx.o: ../Core/Startup/startup_stm32l412c8tx.s
	arm-none-eabi-gcc -fsingle-precision-constant -Wdouble-promotion -mcpu=cortex-m4 -g3 -c -x assembler-with-cpp -MMD -MP -MF"Core/Startup/startup_stm32l412c8tx.d" -MT"$@" --specs=nano_c_standard_cpp.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@" "$<"

