################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../usbd_drivers/usbd_core.c \
../usbd_drivers/usbd_int.c \
../usbd_drivers/usbd_sdr.c 

OBJS += \
./usbd_drivers/usbd_core.o \
./usbd_drivers/usbd_int.o \
./usbd_drivers/usbd_sdr.o 

C_DEPS += \
./usbd_drivers/usbd_core.d \
./usbd_drivers/usbd_int.d \
./usbd_drivers/usbd_sdr.d 


# Each subdirectory must supply rules for building sources it contributes
usbd_drivers/%.o: ../usbd_drivers/%.c usbd_drivers/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU Arm Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Ofast -fmessage-length=0 -ffunction-sections -fdata-sections  -g -DAT_START_F403A_V1 -DNDEBUG -DAT32F403ACGU7 -DUSE_STDPERIPH_DRIVER -I"../include" -I"F:\AT32-workspace\AT32F403ACGU7_clone_windows_desktop_virtual_com_port\JPEG" -I"F:\AT32-workspace\AT32F403ACGU7_clone_windows_desktop_virtual_com_port\cdc" -I"F:\AT32-workspace\AT32F403ACGU7_clone_windows_desktop_virtual_com_port\usbd_drivers" -I"F:\AT32-workspace\AT32F403ACGU7_clone_windows_desktop_virtual_com_port\Display" -I"../include/libraries/drivers/inc" -I"../include/libraries/cmsis/cm4/core_support" -I"../include/libraries/cmsis/cm4/device_support" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


