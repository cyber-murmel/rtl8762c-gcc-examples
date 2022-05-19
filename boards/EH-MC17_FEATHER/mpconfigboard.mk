MICROPY_VFS_LFS1 ?= 1

FLASH_SIZE = 0x00080000
FIRMWARE_SIZE = 0x00019000 # 100 kiB

OEM_CONFIG = $(BUILD)/oem_config.bin
OTA_BANK0_HEADER = $(BOARD_DIR)/blobs/ota_bank0_header.bin
PATCH = $(BOARD_DIR)/blobs/patch.bin
FSBL = $(BOARD_DIR)/blobs/fsbl.bin
