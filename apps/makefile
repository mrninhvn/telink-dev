##########################################################################################
# Set SDK Path , if you copy this project other dir, please set TEL_PATH absolute path
#########################################################################################
TEL_PATH := ..
TOOLCHAIN_PATH := ../../tc32/bin
export PATH := $(TOOLCHAIN_PATH):$(PATH)

#########################################################################################
# The following content need not be modified by the user!!!!!
#########################################################################################

ifndef APP
APP := null
endif

PROJECT_NAME := $(APP)
PROJECT_PATH := ./$(APP)
BUILD_PATH := ./.build
OUT_PATH :=$(BUILD_PATH)/$(PROJECT_PATH)

#include the SDK makefile
OBJ_SRCS := 
S_SRCS := 
ASM_SRCS := 
C_SRCS := 
S_UPPER_SRCS := 
O_SRCS := 
FLASH_IMAGE := 
ELFS := 
OBJS := 
LST := 
SIZEDUMMY := 
OUT_DIR :=

# ################################################################################
# # Automatically-generated file. Do not edit! subdir.mk
# ################################################################################

# # Add inputs and outputs from these tool invocations to the build variables 
# S_UPPER_SRCS += \
# ../div_mod.S 

# OBJS += $(OUT_PATH)/div_mod.o 

# # Each subdirectory must supply rules for building sources it contributes
# $(OUT_PATH)/%.o: ../../%.S
# 	@echo 'Building file: $<'
# 	@echo 'Invoking: TC32 CC/Assembler'
# 	tc32-elf-gcc -DMCU_CORE_8269 -c -o"$@" "$<"
# 	@echo 'Finished building: $<'
# 	@echo ' '

# ################################################################################
# # Automatically-generated file. Do not edit! object.mk
# ################################################################################

GCC_FLAGS := \
-ffunction-sections \
-fdata-sections \
-Wall \
-O2 \
-fpack-struct \
-fshort-enums \
-finline-small-functions \
-std=gnu99 \
-fshort-wchar \
-fms-extensions 

INCLUDE_PATHS := -I$(TEL_PATH)/components -I$(PROJECT_PATH)

GCC_FLAGS += $(TEL_CHIP)

ifeq ($(RETENTION_RAM_SIZE), 32KB)
	LS_FLAGS := $(TEL_PATH)/components/boot/boot_32k_retn_8253_8258.link
else
	LS_FLAGS := $(TEL_PATH)/components/boot/boot_16k_retn_8251_8253_8258.link
endif

#include Project makefile
-include $(PROJECT_PATH)/project.mk

# Add inputs and outputs from these tool invocations to the build variables 
LST_FILE := $(OUT_PATH)/$(PROJECT_NAME).lst
BIN_FILE := $(OUT_PATH)/$(PROJECT_NAME).bin
ELF_FILE := $(OUT_PATH)/$(PROJECT_NAME).elf
UART_BOOT:= $(TEL_PATH)/components/TB-02_boot_V0.5.bin

SIZEDUMMY := sizedummy

# All Target
all: pre-build main-build

# Main-build Target
main-build: $(ELF_FILE) secondary-outputs

# Tool invocations
$(ELF_FILE): $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@tc32-elf-ld --gc-sections -L $(TEL_PATH)/components/proj_lib -T $(LS_FLAGS) -o $(ELF_FILE) $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

$(LST_FILE): $(ELF_FILE)
	@echo 'Invoking: TC32 Create Extended Listing'
	tc32-elf-objdump -x -D -l -S  $(ELF_FILE)  > $(LST_FILE)
	@echo 'Finished building: $@'
	@echo ' '

$(BIN_FILE): $(ELF_FILE)
	@echo 'Create Flash image (binary format)'
	tc32-elf-objcopy -v -O binary $(ELF_FILE)  $(BIN_FILE)
	python3 $(TEL_PATH)/tools/tl_firmware_tools.py add_crc $(BIN_FILE)
	@echo 'Finished building: $@'
	@echo ' '

sizedummy: $(ELF_FILE)
	@echo 'Invoking: Print Size'
	tc32-elf-size -t $(ELF_FILE)
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean: | $(APP)
	-$(RM) $(FLASH_IMAGE) $(ELFS) $(OBJS) $(LST) $(SIZEDUMMY) $(ELF_FILE) $(BIN_FILE) $(LST_FILE)
	-@echo ' '

clear:
	-rm -rf $(BUILD_PATH)/*
	-@echo ' '

pre-build: | $(APP)
	mkdir -p $(foreach s,$(OUT_DIR),$(OUT_PATH)$(s))
	-@echo ' '

flash:
	python $(TEL_PATH)/tools/Telink_Tools.py -p $(DOWNLOAD_PORT) burn $(BIN_FILE) 

erase_fw:
	python $(TEL_PATH)/tools/Telink_Tools.py -p $(DOWNLOAD_PORT) erase_flash 0x4000 44

erase_all:
	python $(TEL_PATH)/tools/Telink_Tools.py -p $(DOWNLOAD_PORT) erase_flash 0x4000 124

monitor:
	python $(TEL_PATH)/tools/monitor.py --port $(DOWNLOAD_PORT) --baud 115200 $(ELF_FILE)

combine_fw:
	python $(TEL_PATH)/tools/tl_firmware_tools.py combine $(UART_BOOT) $(BIN_FILE) $(OUT_PATH)/$(PROJECT_NAME)_with_boot.bin

secondary-outputs: $(BIN_FILE) $(LST_FILE) $(SIZEDUMMY)

.PHONY: all clean clear dependents flash erase_fw monitor combine_fw pre-build
.SECONDARY: main-build 