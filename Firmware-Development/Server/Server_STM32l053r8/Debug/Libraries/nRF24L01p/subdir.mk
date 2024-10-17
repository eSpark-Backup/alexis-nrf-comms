################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Libraries/nRF24L01p/nrf24l01p.c 

C_DEPS += \
./Libraries/nRF24L01p/nrf24l01p.d 

OBJS += \
./Libraries/nRF24L01p/nrf24l01p.o 


# Each subdirectory must supply rules for building sources it contributes
Libraries/nRF24L01p/%.o Libraries/nRF24L01p/%.su Libraries/nRF24L01p/%.cyclo: ../Libraries/nRF24L01p/%.c Libraries/nRF24L01p/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0plus -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L053xx -c -I../Core/Inc -I../Drivers/STM32L0xx_HAL_Driver/Inc -I../Drivers/STM32L0xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L0xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Libraries-2f-nRF24L01p

clean-Libraries-2f-nRF24L01p:
	-$(RM) ./Libraries/nRF24L01p/nrf24l01p.cyclo ./Libraries/nRF24L01p/nrf24l01p.d ./Libraries/nRF24L01p/nrf24l01p.o ./Libraries/nRF24L01p/nrf24l01p.su

.PHONY: clean-Libraries-2f-nRF24L01p

