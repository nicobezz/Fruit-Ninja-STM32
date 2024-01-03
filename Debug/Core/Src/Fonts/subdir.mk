################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/Fonts/font12.c \
../Core/Src/Fonts/font16.c \
../Core/Src/Fonts/font20.c \
../Core/Src/Fonts/font24.c \
../Core/Src/Fonts/font8.c 

OBJS += \
./Core/Src/Fonts/font12.o \
./Core/Src/Fonts/font16.o \
./Core/Src/Fonts/font20.o \
./Core/Src/Fonts/font24.o \
./Core/Src/Fonts/font8.o 

C_DEPS += \
./Core/Src/Fonts/font12.d \
./Core/Src/Fonts/font16.d \
./Core/Src/Fonts/font20.d \
./Core/Src/Fonts/font24.d \
./Core/Src/Fonts/font8.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/Fonts/%.o Core/Src/Fonts/%.su Core/Src/Fonts/%.cyclo: ../Core/Src/Fonts/%.c Core/Src/Fonts/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F769xx -DDEBUG -c -I../Drivers/CMSIS/Include -I../Drivers/CMSIS/Device/ST/STM32F7xx/Include -I../Core/Inc -I../Drivers/STM32F7xx_HAL_Driver/Inc -I../Core/Src/BSP -I../Drivers/STM32F7xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM7/r0p1 -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-Fonts

clean-Core-2f-Src-2f-Fonts:
	-$(RM) ./Core/Src/Fonts/font12.cyclo ./Core/Src/Fonts/font12.d ./Core/Src/Fonts/font12.o ./Core/Src/Fonts/font12.su ./Core/Src/Fonts/font16.cyclo ./Core/Src/Fonts/font16.d ./Core/Src/Fonts/font16.o ./Core/Src/Fonts/font16.su ./Core/Src/Fonts/font20.cyclo ./Core/Src/Fonts/font20.d ./Core/Src/Fonts/font20.o ./Core/Src/Fonts/font20.su ./Core/Src/Fonts/font24.cyclo ./Core/Src/Fonts/font24.d ./Core/Src/Fonts/font24.o ./Core/Src/Fonts/font24.su ./Core/Src/Fonts/font8.cyclo ./Core/Src/Fonts/font8.d ./Core/Src/Fonts/font8.o ./Core/Src/Fonts/font8.su

.PHONY: clean-Core-2f-Src-2f-Fonts

