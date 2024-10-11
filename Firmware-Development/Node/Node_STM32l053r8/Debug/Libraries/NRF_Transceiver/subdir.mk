################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Libraries/NRF_Transceiver/NRF_Transceiver.cpp 

OBJS += \
./Libraries/NRF_Transceiver/NRF_Transceiver.o 

CPP_DEPS += \
./Libraries/NRF_Transceiver/NRF_Transceiver.d 


# Each subdirectory must supply rules for building sources it contributes
Libraries/NRF_Transceiver/%.o Libraries/NRF_Transceiver/%.su Libraries/NRF_Transceiver/%.cyclo: ../Libraries/NRF_Transceiver/%.cpp Libraries/NRF_Transceiver/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m0plus -std=gnu++14 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L053xx -c -I../Core/Inc -I../Drivers/STM32L0xx_HAL_Driver/Inc -I../Drivers/STM32L0xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L0xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/Windows/Documents/Github/alexis-nrfcomms/Firmware-Development/Node/Node_STM32l053r8/Libraries/nRF24L01p" -I"C:/Users/Windows/Documents/Github/alexis-nrfcomms/Firmware-Development/Node/Node_STM32l053r8/Libraries/NRF_Transceiver" -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Libraries-2f-NRF_Transceiver

clean-Libraries-2f-NRF_Transceiver:
	-$(RM) ./Libraries/NRF_Transceiver/NRF_Transceiver.cyclo ./Libraries/NRF_Transceiver/NRF_Transceiver.d ./Libraries/NRF_Transceiver/NRF_Transceiver.o ./Libraries/NRF_Transceiver/NRF_Transceiver.su

.PHONY: clean-Libraries-2f-NRF_Transceiver

