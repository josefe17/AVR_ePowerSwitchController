################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
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

# Add inputs and outputs from these tool invocations to the build variables 
LSS += \
AVR_ePowerSwitchController.lss \

FLASH_IMAGE += \
AVR_ePowerSwitchController.hex \

SIZEDUMMY += \
sizedummy \

AVRDUDEDUMMY += \
avrdudedummy \


# All Target
all: AVR_ePowerSwitchController.elf secondary-outputs

# Tool invocations
AVR_ePowerSwitchController.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: AVR C Linker'
	avr-gcc -Wl,-Map,AVR_ePowerSwitchController.map -mmcu=atmega328p -o "AVR_ePowerSwitchController.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

AVR_ePowerSwitchController.lss: AVR_ePowerSwitchController.elf
	@echo 'Invoking: AVR Create Extended Listing'
	-avr-objdump -h -S AVR_ePowerSwitchController.elf  >"AVR_ePowerSwitchController.lss"
	@echo 'Finished building: $@'
	@echo ' '

AVR_ePowerSwitchController.hex: AVR_ePowerSwitchController.elf
	@echo 'Create Flash image (ihex format)'
	-avr-objcopy -R .eeprom -R .fuse -R .lock -R .signature -O ihex AVR_ePowerSwitchController.elf  "AVR_ePowerSwitchController.hex"
	@echo 'Finished building: $@'
	@echo ' '

sizedummy: AVR_ePowerSwitchController.elf
	@echo 'Invoking: Print Size'
	-avr-size --format=berkeley -t AVR_ePowerSwitchController.elf
	@echo 'Finished building: $@'
	@echo ' '

avrdudedummy: AVR_ePowerSwitchController.elf
	@echo 'Invoking: AVRDude'
	C:\WinAVR-20100110\bin\avrdude -pm328p -cusbasp -PUSB -e -Uflash:w:AVR_ePowerSwitchController.hex:a
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(FLASH_IMAGE)$(ELFS)$(OBJS)$(ASM_DEPS)$(AVRDUDEDUMMY)$(S_DEPS)$(SIZEDUMMY)$(S_UPPER_DEPS)$(LSS)$(C_DEPS) AVR_ePowerSwitchController.elf
	-@echo ' '

secondary-outputs: $(LSS) $(FLASH_IMAGE) $(SIZEDUMMY) $(AVRDUDEDUMMY)

.PHONY: all clean dependents

-include ../makefile.targets
