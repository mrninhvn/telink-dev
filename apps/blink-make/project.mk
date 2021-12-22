################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 

OBJS += $(OUT_PATH)/main.o 

-include $(TEL_PATH)/components/boot/8258/boot.mk
-include $(TEL_PATH)/components/drivers/8258/drivers_8258.mk

# Each subdirectory must supply rules for building sources it contributes
$(OUT_PATH)/%.o: ./%.c
	@echo 'Building file: $<'
	@tc32-elf-gcc $(GCC_FLAGS) $(INCLUDE_PATHS) -c -o"$@" "$<"
