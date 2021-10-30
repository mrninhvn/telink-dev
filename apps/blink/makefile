#########################################################################################
# As a new project, you should  the following variables		

# First, select the chip type, the chip type can be:					
#			TEL_CHIP := -DCHIP_TYPE=CHIP_TYPE_8261
#			TEL_CHIP := -DCHIP_TYPE=CHIP_TYPE_8266
#			TEL_CHIP := -DCHIP_TYPE=CHIP_TYPE_8267
#			TEL_CHIP := -DCHIP_TYPE=CHIP_TYPE_8269
#
# Second, select the link lib, the available link lib file in Telink SDK proj_lib folder
#
# Third, Set Telink SDK path to  variable TEL_PATH
# 
# Forth, Set Project Name
#########################################################################################

# Select Chip  
TEL_CHIP := -DCHIP_TYPE=CHIP_TYPE_8258

# Select link lib
LIBS :=  -llt_8258

##########################################################################################
#Set SDK Path , if you copy this project other dir, please set TEL_PATH absolute path
#设置 SDK Path，目前使用的是相对路径，如果你将此工程复制到别的文件夹，请将TEL_PATH设为绝对路径
# 比如： TEL_PATH := /home/ospanic/Telink_825X_SDK
#########################################################################################
TEL_PATH := ../..

# Set Project Name
PROJECT_NAME := blink

# 设置下载固件的串口号
DOWNLOAD_PORT := /dev/ttyUSB0

#########################################################################################
# The following content need not be modified by the user!!!!!							#
#########################################################################################
PROJECT_PATH := .
OUT_PATH :=$(PROJECT_PATH)/build

# ifneq ($(TEL_PATH)/make/makefile, $(wildcard $(TEL_PATH)/make/makefile))
# $(error "Please check if the TEL_PATH is correct, 请检查 TEL_PATH 是否设置正确！！！")
# endif

#include the SDK makefile
# -include $(TEL_PATH)/make/makefile

#include Sources makefile
-include $(PROJECT_PATH)/sources.mk

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
	python3 $(TEL_PATH)/make/tl_firmware_tools.py add_crc $(BIN_FILE)
	@echo 'Finished building: $@'
	@echo ' '

sizedummy: $(ELF_FILE)
	@echo 'Invoking: Print Size'
	tc32-elf-size -t $(ELF_FILE)
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(FLASH_IMAGE) $(ELFS) $(OBJS) $(LST) $(SIZEDUMMY) $(ELF_FILE) $(BIN_FILE) $(LST_FILE)
	-@echo ' '

pre-build:
	mkdir -p $(foreach s,$(OUT_DIR),$(OUT_PATH)$(s))
#-"D:\tel\ble_sdk_release/getver.sh"
	-@echo ' '

flash:
	python $(TEL_PATH)/make/Telink_Tools.py -p $(DOWNLOAD_PORT) burn $(BIN_FILE) 

erase_fw:
	python $(TEL_PATH)/make/Telink_Tools.py -p $(DOWNLOAD_PORT) erase_flash 0x4000 44

erase_all:
	python $(TEL_PATH)/make/Telink_Tools.py -p $(DOWNLOAD_PORT) erase_flash 0x4000 124

monitor:
	python $(TEL_PATH)/make/monitor.py --port $(DOWNLOAD_PORT) --baud 115200 $(ELF_FILE)

combine_fw:
	python $(TEL_PATH)/make/tl_firmware_tools.py combine $(UART_BOOT) $(BIN_FILE) $(OUT_PATH)/$(PROJECT_NAME)_with_boot.bin

secondary-outputs: $(BIN_FILE) $(LST_FILE) $(SIZEDUMMY)

.PHONY: all clean dependents flash erase_fw monitor combine_fw pre-build
.SECONDARY: main-build