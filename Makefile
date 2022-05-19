BOARD ?= EH-MC17_FEATHER
BOARD_DIR ?= boards/$(BOARD)
TARGET ?= blink
PORT ?= /dev/ttyUSB1

FTDI_VERSION ?= ft2232h
FTDI_SWD_CHANNEL ?= 0

ifeq ($(wildcard $(BOARD_DIR)/.),)
$(error Invalid BOARD specified: $(BOARD_DIR))
endif

include ./mkenv.mk
include $(BOARD_DIR)/mpconfigboard.mk

CROSS_COMPILE ?= arm-none-eabi-

DIRS = $(sort \
	./ \
	./src/$(TARGET)/ \
	./config/ \
	$(BUILD) \
	$(BOARD_DIR) \
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
)

INC += $(addprefix -I,$(DIRS))

CFLAGS_CORTEX_M4 = -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard
# CFLAGS = $(INC) -Wall -fdata-sections -ffunction-sections -std=c11 $(CFLAGS_CORTEX_M4) $(COPT)
CFLAGS = $(INC) -Wall -fdata-sections -ffunction-sections -std=c99 $(CFLAGS_CORTEX_M4) $(COPT)
LDFLAGS = $(CFLAGS_CORTEX_M4) -specs=nano.specs -Trtl8762c.ld -Wl,-Map=$(basename $@).map,--cref -Wl,--gc-sections -Wl,--defsym=FIRMWARE_SIZE=$(FIRMWARE_SIZE)

CFLAGS += -include src/$(TARGET)/app_flags.h -DFIRMWARE_SIZE=$(FIRMWARE_SIZE) -DFLASH_SIZE=$(FLASH_SIZE)

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
	sdk/bin/ADPCM.lib \
	sdk/bin/auto_k_rf_bonding_lib_DUT.lib \
	sdk/bin/auto_k_rf.lib \
	sdk/bin/bee2_adc_lib.lib \
	sdk/bin/bee2_sdk.lib \
	sdk/bin/gap_bt5.lib \
	sdk/bin/gap_utils.lib \
	sdk/bin/ima_adpcm_lib.lib \
	sdk/bin/msbc_lib.lib \
	sdk/bin/opus_celt_enc_lib.lib \
	sdk/bin/sbc_lib.lib \
	sdk/bin/sbc_lib_o3.lib \
	sdk/bin/system_trace.lib \

SRC_S =  sdk/src/mcu/rtl876x/arm/startup_rtl8762c_gcc.s

SRC_C = \
    sdk/src/mcu/rtl876x/system_rtl8762c.c \
	sdk/src/mcu/peripheral/rtl876x_rcc.c \
	sdk/src/mcu/peripheral/rtl876x_gpio.c \
	$(wildcard src/$(TARGET)/*.c) \

OBJ += $(addprefix $(BUILD)/, $(SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_CXX:.cpp=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_S:.s=.o))

all: $(BUILD)/image.bin

$(BUILD)/firmware.elf: $(OBJ)
	$(ECHO) "LINK $@"
	$(Q)$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)
	$(Q)$(SIZE) $@

$(BUILD)/firmware.bin: $(BUILD)/firmware.elf
	$(Q)$(OBJCOPY) --output-target=binary $^ $@

$(BUILD)/image.bin: $(BUILD)/firmware.bin $(BUILD)/oem_config.bin $(BUILD)/ota_bank_header.bin
	$(Q)$(DD) if=$(OEM_CONFIG) bs=1 seek=0 of=$@
	$(Q)$(DD) if=$(BUILD)/ota_bank_header.bin bs=1 seek=4096 of=$@
	$(Q)$(DD) if=sdk/config/patch.bin bs=1 skip=512 seek=8192 of=$@
	$(Q)$(DD) if=sdk/config/fsbl.bin bs=1 skip=512 seek=49152 of=$@
	$(Q)$(DD) if=$(BUILD)/firmware.bin bs=1 seek=53248 of=$@

$(BUILD)/oem_config.bin: $(BUILD)/oem_config.o
	$(Q)$(OBJCOPY) --output-target=binary $< $@
	$(Q)$(PYTHON) tools/crc_patch.py $@

$(BUILD)/oem_config.o: config/oem_config.c
	$(Q)$(CC) -c -Wno-override-init $(CFLAGS) $< -o $@

$(BUILD)/ota_bank_header.bin: $(BUILD)/ota_bank_header.o
	$(Q)$(OBJCOPY) --output-target=binary $< $@
	$(Q)$(PYTHON) tools/crc_patch.py $@
	cp $@ $@.bak
	# cp ota_bank0_header.bin $@
	# dd if=OTAHeader_Bank0-92d1bc8e773dda0c87a690481c7ac4d4.bin bs=1 skip=512 of=$@

$(BUILD)/ota_bank_header.o: config/ota_bank_header.c
	$(Q)$(CC) -c -Wno-override-init $(CFLAGS) $< -o $@

debug_ftdi: $(BUILD)/firmware.elf
	$(Q)$(GDB) $< \
		$(addprefix --directory ,$(DIRS)) \
		-ex "target extended-remote | openocd -c 'log_output openocd.log' -f debug/ftdi/$(FTDI_VERSION).cfg -c 'ftdi_channel $(FTDI_SWD_CHANNEL)' -f debug/ftdi/swd.cfg -f debug/rtl8762c.cfg" \
		-ex 'monitor reset halt' \
		-ex 'break main' \
		-ex 'continue'
.PHONY: debug_ftdi

flash: $(BUILD)/image.bin
	$(Q)$(PYTHON) tools/rtltool/rtltool.py --port $(PORT) write_flash 0x00801000 $<
.PHONY: flash


include ./mkrules.mk
