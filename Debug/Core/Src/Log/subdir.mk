################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/Log/lcd_log.c 

OBJS += \
./Core/Src/Log/lcd_log.o 

C_DEPS += \
./Core/Src/Log/lcd_log.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/Log/%.o Core/Src/Log/%.su Core/Src/Log/%.cyclo: ../Core/Src/Log/%.c Core/Src/Log/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F769xx -DDEBUG -c -I../Drivers/CMSIS/Include -I../Drivers/CMSIS/Device/ST/STM32F7xx/Include -I../Core/Inc -I../Drivers/STM32F7xx_HAL_Driver/Inc -I../Core/Src/BSP -I../Drivers/STM32F7xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM7/r0p1 -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-Log

clean-Core-2f-Src-2f-Log:
	-$(RM) ./Core/Src/Log/lcd_log.cyclo ./Core/Src/Log/lcd_log.d ./Core/Src/Log/lcd_log.o ./Core/Src/Log/lcd_log.su

.PHONY: clean-Core-2f-Src-2f-Log

