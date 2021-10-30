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


#include SDK makefile
# -include $(TEL_PATH)/make/application.mk
# -include $(TEL_PATH)/make/boot.mk
# -include $(TEL_PATH)/make/common.mk
# -include $(TEL_PATH)/make/drivers_8258.mk
# -include $(TEL_PATH)/make/vendor_common.mk
# -include $(TEL_PATH)/make/tinyFlash.mk

OUT_DIR += /drivers/8258
OBJS += \
$(OUT_PATH)/drivers/8258/adc.o \
$(OUT_PATH)/drivers/8258/gpio_8258.o \
$(OUT_PATH)/drivers/8258/timer.o \
$(OUT_PATH)/drivers/8258/clock.o \
$(OUT_PATH)/drivers/8258/analog.o \
# $(OUT_PATH)/drivers/8258/aes.o \
# $(OUT_PATH)/drivers/8258/analog.o \
# $(OUT_PATH)/drivers/8258/audio.o \
# $(OUT_PATH)/drivers/8258/bsp.o \
# $(OUT_PATH)/drivers/8258/clock.o \
# $(OUT_PATH)/drivers/8258/emi.o \
# $(OUT_PATH)/drivers/8258/flash.o \
# $(OUT_PATH)/drivers/8258/i2c.o \
# $(OUT_PATH)/drivers/8258/lpc.o \
# $(OUT_PATH)/drivers/8258/qdec.o \
# $(OUT_PATH)/drivers/8258/rf_pa.o \
# $(OUT_PATH)/drivers/8258/s7816.o \
# $(OUT_PATH)/drivers/8258/spi.o \
# $(OUT_PATH)/drivers/8258/timer.o \
# $(OUT_PATH)/drivers/8258/uart.o \
# $(OUT_PATH)/drivers/8258/watchdog.o 

# Each subdirectory must supply rules for building sources it contributes
$(OUT_PATH)/drivers/8258/%.o: $(TEL_PATH)/components/drivers/8258/%.c
	@echo 'Building file: $<'
	@tc32-elf-gcc $(GCC_FLAGS) $(INCLUDE_PATHS) -c -o"$@" "$<"


OUT_DIR += /boot/8251 /boot/8253 /boot/8258
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

ifeq ($(USE_FREE_RTOS), 1)
-include $(TEL_PATH)/make/freertos.mk
GCC_FLAGS += -DUSE_FREE_RTOS
endif