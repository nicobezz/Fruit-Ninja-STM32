################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/Components/adv7533/adv7533.c 

OBJS += \
./Core/Src/Components/adv7533/adv7533.o 

C_DEPS += \
./Core/Src/Components/adv7533/adv7533.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/Components/adv7533/%.o Core/Src/Components/adv7533/%.su Core/Src/Components/adv7533/%.cyclo: ../Core/Src/Components/adv7533/%.c Core/Src/Components/adv7533/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F769xx -DDEBUG -c -I../Drivers/CMSIS/Include -I../Drivers/CMSIS/Device/ST/STM32F7xx/Include -I../Core/Inc -I../Drivers/STM32F7xx_HAL_Driver/Inc -I../Core/Src/BSP -I../Drivers/STM32F7xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM7/r0p1 -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-Components-2f-adv7533

clean-Core-2f-Src-2f-Components-2f-adv7533:
	-$(RM) ./Core/Src/Components/adv7533/adv7533.cyclo ./Core/Src/Components/adv7533/adv7533.d ./Core/Src/Components/adv7533/adv7533.o ./Core/Src/Components/adv7533/adv7533.su

.PHONY: clean-Core-2f-Src-2f-Components-2f-adv7533

