################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Application/Operation/Operation.c 

OBJS += \
./Application/Operation/Operation.o 

C_DEPS += \
./Application/Operation/Operation.d 


# Each subdirectory must supply rules for building sources it contributes
Application/Operation/%.o Application/Operation/%.su Application/Operation/%.cyclo: ../Application/Operation/%.c Application/Operation/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0plus -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L053xx -c -I../Core/Inc -I../Drivers/STM32L0xx_HAL_Driver/Inc -I../Drivers/STM32L0xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L0xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/Windows/Documents/Github/alexis-nrfcomms/Firmware-Development/Server/Server_STM32l053r8/Application/Operation" -I"C:/Users/Windows/Documents/Github/alexis-nrfcomms/Firmware-Development/Server/Server_STM32l053r8/Application/nRF24L01p" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Application-2f-Operation

clean-Application-2f-Operation:
	-$(RM) ./Application/Operation/Operation.cyclo ./Application/Operation/Operation.d ./Application/Operation/Operation.o ./Application/Operation/Operation.su

.PHONY: clean-Application-2f-Operation

