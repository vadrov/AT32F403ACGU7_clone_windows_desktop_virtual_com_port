################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include user/subdir.mk
-include usbd_drivers/subdir.mk
-include firmware/subdir.mk
-include cmsis/subdir.mk
-include cdc/subdir.mk
-include bsp/subdir.mk
-include JPEG/subdir.mk
-include Display/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := AT32F403ACGU7_clone_windows_desktop_virtual_com_port
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \
AT32F403ACGU7_clone_windows_desktop_virtual_com_port.hex \

SECONDARY_LIST += \
AT32F403ACGU7_clone_windows_desktop_virtual_com_port.lst \

SECONDARY_SIZE += \
AT32F403ACGU7_clone_windows_desktop_virtual_com_port.siz \


# All Target
all:
	+@$(MAKE) --no-print-directory pre-build && $(MAKE) --no-print-directory main-build

# Main-build Target
main-build: AT32F403ACGU7_clone_windows_desktop_virtual_com_port.elf secondary-outputs

# Tool invocations
AT32F403ACGU7_clone_windows_desktop_virtual_com_port.elf: $(OBJS) $(USER_OBJS) makefile objects.mk $(OPTIONAL_TOOL_DEPS)
	@echo 'Building target: $@'
	@echo 'Invoking: GNU Arm Cross C Linker'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Ofast -fmessage-length=0 -ffunction-sections -fdata-sections  -g -T "F:\AT32-workspace\AT32F403ACGU7_clone_windows_desktop_virtual_com_port/ldscripts/AT32F403AxG_FLASH.ld" -Xlinker --gc-sections -Wl,-Map,"AT32F403ACGU7_clone_windows_desktop_virtual_com_port.map" --specs=nano.specs --specs=nosys.specs -o "AT32F403ACGU7_clone_windows_desktop_virtual_com_port.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

AT32F403ACGU7_clone_windows_desktop_virtual_com_port.hex: AT32F403ACGU7_clone_windows_desktop_virtual_com_port.elf makefile objects.mk $(OPTIONAL_TOOL_DEPS)
	@echo 'Invoking: GNU Arm Cross Create Flash Image'
	arm-none-eabi-objcopy -O ihex "AT32F403ACGU7_clone_windows_desktop_virtual_com_port.elf"  "AT32F403ACGU7_clone_windows_desktop_virtual_com_port.hex"
	@echo 'Finished building: $@'
	@echo ' '

AT32F403ACGU7_clone_windows_desktop_virtual_com_port.lst: AT32F403ACGU7_clone_windows_desktop_virtual_com_port.elf makefile objects.mk $(OPTIONAL_TOOL_DEPS)
	@echo 'Invoking: GNU Arm Cross Create Listing'
	arm-none-eabi-objdump --source --all-headers --demangle --line-numbers --wide "AT32F403ACGU7_clone_windows_desktop_virtual_com_port.elf" > "AT32F403ACGU7_clone_windows_desktop_virtual_com_port.lst"
	@echo 'Finished building: $@'
	@echo ' '

AT32F403ACGU7_clone_windows_desktop_virtual_com_port.siz: AT32F403ACGU7_clone_windows_desktop_virtual_com_port.elf makefile objects.mk $(OPTIONAL_TOOL_DEPS)
	@echo 'Invoking: GNU Arm Cross Print Size'
	arm-none-eabi-size --format=berkeley "AT32F403ACGU7_clone_windows_desktop_virtual_com_port.elf"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(SECONDARY_FLASH)$(SECONDARY_LIST)$(SECONDARY_SIZE)$(ASM_DEPS)$(S_DEPS)$(S_UPPER_DEPS)$(C_DEPS) AT32F403ACGU7_clone_windows_desktop_virtual_com_port.elf
	-@echo ' '

pre-build:
	-make -f ../Display/makefile
	-@echo ' '

secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_LIST) $(SECONDARY_SIZE)

.PHONY: all clean dependents main-build pre-build

-include ../makefile.targets
