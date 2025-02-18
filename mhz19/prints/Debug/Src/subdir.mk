################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/main.c \
../Src/timer2.c \
../Src/usart1.c \
../Src/usart2.c 

OBJS += \
./Src/main.o \
./Src/timer2.o \
./Src/usart1.o \
./Src/usart2.o 

C_DEPS += \
./Src/main.d \
./Src/timer2.d \
./Src/usart1.d \
./Src/usart2.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o Src/%.su Src/%.cyclo: ../Src/%.c Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DSTM32 -DSTM32F3 -DDEBUG -DSTM32F303RETx -c -I../Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Src

clean-Src:
	-$(RM) ./Src/main.cyclo ./Src/main.d ./Src/main.o ./Src/main.su ./Src/timer2.cyclo ./Src/timer2.d ./Src/timer2.o ./Src/timer2.su ./Src/usart1.cyclo ./Src/usart1.d ./Src/usart1.o ./Src/usart1.su ./Src/usart2.cyclo ./Src/usart2.d ./Src/usart2.o ./Src/usart2.su

.PHONY: clean-Src

