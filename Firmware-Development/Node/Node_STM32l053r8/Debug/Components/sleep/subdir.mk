################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Components/sleep/A.cpp 

OBJS += \
./Components/sleep/A.o 

CPP_DEPS += \
./Components/sleep/A.d 


# Each subdirectory must supply rules for building sources it contributes
Components/sleep/%.o Components/sleep/%.su Components/sleep/%.cyclo: ../Components/sleep/%.cpp Components/sleep/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m0plus -std=gnu++14 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L053xx -c -I../Core/Inc -I../Drivers/STM32L0xx_HAL_Driver/Inc -I../Drivers/STM32L0xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L0xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Components-2f-sleep

clean-Components-2f-sleep:
	-$(RM) ./Components/sleep/A.cyclo ./Components/sleep/A.d ./Components/sleep/A.o ./Components/sleep/A.su

.PHONY: clean-Components-2f-sleep

