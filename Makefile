BOARD ?= EMB1082
BOARD_DIR ?= boards/$(BOARD)
TARGET ?= 00-blink
TARGET_DIR ?= ./src/targets/$(TARGET)

FTDI_VERSION ?= ft2232h
FTDI_SWD_CHANNEL ?= 0

ifeq ($(wildcard $(BOARD_DIR)/.),)
$(error Invalid BOARD specified: $(BOARD_DIR))
endif

include ./mkenv.mk
include $(BOARD_DIR)/board_config.mk
PORT ?= /dev/ttyUSB1

CROSS_COMPILE ?= arm-none-eabi-

DIRS = $(sort \
	./ \
	./src/ \
	./config/ \
	$(BUILD) \
	$(BOARD_DIR) \
	$(TARGET_DIR)/ \
	sdk/inc/ \
	sdk/inc/platform/ \
	sdk/inc/platform/cmsis/ \
	sdk/inc/peripheral/ \
	sdk/inc/bluetooth/profile/ \
	sdk/inc/bluetooth/profile/server/ \
	sdk/inc/bluetooth/gap/ \
	sdk/src/ble/privacy/ \
	sdk/inc/os/ \
	sdk/inc/app/ \
	sdk/src/ble/privacy \
)

INC += $(addprefix -I,$(DIRS))

CFLAGS_CORTEX_M4 = -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard
# CFLAGS = $(INC) -Wall -fdata-sections -ffunction-sections -std=c11 $(CFLAGS_CORTEX_M4) $(COPT)
CFLAGS = $(INC) -Wall -fdata-sections -ffunction-sections -std=c99 $(CFLAGS_CORTEX_M4) $(COPT)
LDFLAGS = $(CFLAGS_CORTEX_M4) -specs=nano.specs -Trtl8762c.ld -Wl,-Map=$(basename $@).map,--cref -Wl,--gc-sections -Wl,--defsym=FIRMWARE_SIZE=$(FIRMWARE_SIZE)

CFLAGS += -include $(TARGET_DIR)/app_flags.h -DFIRMWARE_SIZE=$(FIRMWARE_SIZE) -DFLASH_SIZE=$(FLASH_SIZE)

# Tune for Debugging or Optimization
ifeq ($(DEBUG), 1)
CFLAGS += -O0 -g -ggdb -gdwarf-2
else
CFLAGS += -Os -DNDEBUG
CFLAGS += -fdata-sections -ffunction-sections
endif

# Flags for optional C++ source code
CXXFLAGS += $(filter-out -std=c99,$(CFLAGS))
CXXFLAGS += $(CXXFLAGS_MOD)

# Flags for user C modules
LDFLAGS += $(LDFLAGS_MOD)

LIBS = -lc -lm -lnosys sdk/bin/rom_symbol_gcc.axf \
	sdk/bin/auto_k_rf_bonding_lib_DUT.lib \
	sdk/bin/auto_k_rf_bonding_lib_golden.lib \
	sdk/bin/auto_k_rf.lib \
	sdk/bin/bee2_adc_lib.lib \
	sdk/bin/bee2_sdk.lib \
	sdk/bin/electric_rtk_public_meter.lib \
	sdk/bin/gap_bt5.lib \
	sdk/bin/gap_utils.lib \
	sdk/bin/ima_adpcm_lib.lib \
	sdk/bin/msbc_lib.lib \
	sdk/bin/opus_celt_enc_lib.lib \
	sdk/bin/sbc_lib.lib \
	sdk/bin/system_trace.lib \

# Source Files
include $(TARGET_DIR)/mksrc.mk

SRC_S += sdk/src/mcu/rtl876x/arm/startup_rtl8762c_gcc.s

SRC_C += \
    sdk/src/mcu/rtl876x/system_rtl8762c.c \
	sdk/src/mcu/peripheral/rtl876x_rcc.c \
	sdk/src/mcu/peripheral/rtl876x_gpio.c \
	sdk/src/mcu/peripheral/rtl876x_uart.c \
	sdk/src/ble/privacy/privacy_mgnt.c \

OBJ += $(addprefix $(BUILD)/, $(SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_CXX:.cpp=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_S:.s=.o))

OEM_CONFIG ?= $(BUILD)/oem_config.bin
OTA_BANK0_HEADER ?= $(BUILD)/ota_bank0_header.bin

# vendor binaries
FSBL_VENDOR ?= fsbl_MP_master\#\#_1.1.4.0_c6f6dbf5-6d099f4054016ab6d562698d14e662e9.bin
PATCH_VENDOR ?= Patch_MP_release\#_1.0.611.1_130fa89-1c796670a5129533908722a146121972.bin
FSBL ?= $(BUILD)/fsbl.bin
PATCH ?= $(BUILD)/patch.bin

# Custom Targets
all: $(BUILD)/firmware.bin
.PHONY: all

image: $(BUILD)/image.bin
.PHONY: image

## Firmware
$(BUILD)/firmware.elf: $(OBJ)
	$(ECHO) "LINK $@"
	$(Q)$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)
	$(Q)$(SIZE) $@

$(BUILD)/firmware.bin: $(BUILD)/firmware.elf
	$(Q)$(OBJCOPY) --output-target=binary $^ $@

## Image
$(BUILD)/image.bin: $(OEM_CONFIG) $(OTA_BANK0_HEADER) $(PATCH) $(FSBL) $(BUILD)/firmware.bin
	$(Q)$(DD) if=$(OEM_CONFIG) bs=1 seek=0 of=$@
	$(Q)$(DD) if=$(OTA_BANK0_HEADER) bs=1 seek=4096 of=$@
	$(Q)$(DD) if=$(PATCH) bs=1 seek=8192 of=$@
	$(Q)$(DD) if=$(FSBL) bs=1 seek=49152 of=$@
	$(Q)$(DD) if=$(BUILD)/firmware.bin bs=1 seek=53248 of=$@

### Image Parts
$(BUILD)/oem_config.bin: $(BUILD)/oem_config.o
	$(Q)$(OBJCOPY) --output-target=binary $< $@
	$(Q)$(PYTHON) tools/crc_patch.py $@

$(BUILD)/oem_config.o: config/oem_config.c | $(BUILD)
	$(Q)$(CC) -c -Wno-override-init $(CFLAGS) $< -o $@

$(BUILD)/ota_bank0_header.bin: $(BUILD)/ota_bank0_header.o
	$(Q)$(OBJCOPY) --output-target=binary $< $@
	$(Q)$(PYTHON) tools/crc_patch.py $@

$(BUILD)/ota_bank0_header.o: config/ota_bank0_header.c | $(BUILD)
	$(Q)$(CC) -c -Wno-override-init $(CFLAGS) $< -o $@

$(BUILD)/fsbl.bin: $(FSBL_VENDOR) | $(BUILD)
	$(Q)$(DD) if=$< bs=1 skip=512 of=$@

$(BUILD)/patch.bin: $(PATCH_VENDOR) | $(BUILD)
	$(Q)$(DD) if=$< bs=1 skip=512 of=$@

backup-$(BOARD).bin: | $(BUILD)
	$(Q)$(PYTHON) tools/rtltool/rtltool.py --port $(PORT) read_flash 0x00801000 $(FLASH_SIZE) $@

# Helper Targets
$(BUILD):
	$(Q)$(MKDIR) -p $@

debug_pico: $(BUILD)/firmware.elf
	$(Q)$(GDB) $< \
		$(addprefix --directory ,$(DIRS)) \
		-ex "target extended-remote | openocd -c 'log_output openocd.log' \
			-f interface/cmsis-dap.cfg \
			-c 'transport select swd' \
			-f debug/rtl8762c.cfg \
			-f debug/gdb.cfg" \
		-ex 'monitor halt'
.PHONY: debug_pico

debug_ftdi: $(BUILD)/firmware.elf
	$(Q)$(GDB) $< \
		$(addprefix --directory ,$(DIRS)) \
		-ex "target extended-remote | openocd -c 'log_output openocd.log' \
			-f debug/ftdi/$(FTDI_VERSION).cfg \
			-c 'ftdi_channel $(FTDI_SWD_CHANNEL)' \
			-f debug/ftdi/swd.cfg \
			-f debug/rtl8762c.cfg" \
			-f debug/gdb.cfg" \
		-ex 'monitor reset halt' \
		-ex 'break main' \
		-ex 'continue'
.PHONY: debug_ftdi

debug_stlink: $(BUILD)/firmware.elf
	$(Q)$(GDB) $< \
		$(addprefix --directory ,$(DIRS)) \
		-ex "target extended-remote | openocd -c 'log_output openocd.log' \
			-f interface/stlink.cfg \
			-f debug/rtl8762c.cfg \
			-f debug/gdb.cfg" \
		-ex 'monitor halt'
.PHONY: debug_stlink

flash: $(OEM_CONFIG) $(OTA_BANK0_HEADER) $(FSBL) $(BUILD)/firmware.bin
	$(Q)$(PYTHON) tools/rtltool/rtltool.py --port $(PORT) write_flash \
		0x00801000 $(OEM_CONFIG) \
		0x00802000 $(OTA_BANK0_HEADER) \
		0x0080E000 $(BUILD)/firmware.bin
.PHONY: flash

flash_image: $(BUILD)/image.bin
	$(Q)$(PYTHON) tools/rtltool/rtltool.py --port $(PORT) write_flash 0x00801000 $<
.PHONY: flash

backup: backup-$(BOARD).bin
.PHONY: backup

playback: backup-$(BOARD).bin
	$(Q)$(PYTHON) tools/rtltool/rtltool.py --port $(PORT) write_flash 0x00801000 $<
.PHONY: playback

term:
	python3 -m serial.tools.miniterm --exit-char 24 --rts 0 --dtr 0 $(PORT) 115200
.PHONY: term

format:
	find src/ -name '*.[ch]' -exec clang-format -i -style=WebKit {} +
.PHONY: format

include ./mkrules.mk
