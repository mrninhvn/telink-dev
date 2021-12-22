################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
OUT_DIR += /boot/8258

ifeq ($(RETENTION_RAM_SIZE), 32KB)
	BOOT_FLAG := -DMCU_STARTUP_8258_RET_32K
else
	BOOT_FLAG := -DMCU_STARTUP_8258_RET_16K
endif

ifeq ($(USE_FREE_RTOS), 1)
	BOOT_FLAG += -DUSE_FREE_RTOS
endif

OBJS += \
$(OUT_PATH)/boot/8258/cstartup_8258.o \
$(OUT_PATH)/boot/8258/cstartup_8258_RET_16K.o \
$(OUT_PATH)/boot/8258/cstartup_8258_RET_32K.o 

OBJS += $(OUT_PATH)/boot/div_mod.o

# Each subdirectory must supply rules for building sources it contributes
$(OUT_PATH)/boot/%.o: $(TEL_PATH)/components/boot/%.S
	@echo 'Building file: $<'
	@tc32-elf-gcc $(BOOT_FLAG) -c -o"$@" "$<"

