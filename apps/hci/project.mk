#########################################################################################
# As a new project, you should  the following variables		

# First, select the chip type, the chip type can be:					
#			TEL_CHIP := -DCHIP_TYPE=CHIP_TYPE_8261
#			TEL_CHIP := -DCHIP_TYPE=CHIP_TYPE_8266
#			TEL_CHIP := -DCHIP_TYPE=CHIP_TYPE_8267
#			TEL_CHIP := -DCHIP_TYPE=CHIP_TYPE_8269
#
# Second, Set serial port number for downloading firmware
#
# Third, Include required SDK makefile
#
# Fourth, Add OBJS source file
#########################################################################################

# Select Chip  
TEL_CHIP := -DCHIP_TYPE=CHIP_TYPE_8258

# Select link lib
LIBS :=  -llt_8258

# Set serial port number for downloading firmware
DOWNLOAD_PORT := /dev/ttyUSB0

# include SDK makefile
-include $(TEL_PATH)/components/boot/8258/boot.mk
-include $(TEL_PATH)/components/drivers/8258/drivers_8258.mk
-include $(TEL_PATH)/components/common/common.mk
-include $(TEL_PATH)/components/vendor/common/vendor_common.mk

# Add inputs and outputs from these tool invocations to the build variables 
OBJS += \
$(OUT_PATH)/app.o \
$(OUT_PATH)/main.o


# Each subdirectory must supply rules for building sources it contributes
$(OUT_PATH)/%.o: $(PROJECT_PATH)/%.c
	@echo 'Building file: $<'
	@tc32-elf-gcc $(GCC_FLAGS) $(INCLUDE_PATHS) -c -o"$@" "$<"