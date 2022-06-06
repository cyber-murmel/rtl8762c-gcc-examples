#include <stddef.h>

#include "config/flash_map.h"
#include "sdk/inc/platform/patch_header_check.h"

#define STRUCT_ARRAY_SETTER(NAME, FIELD, VALUE) .FIELD = {[0 ... (sizeof(NAME.FIELD)-1)] = VALUE}

// data structures
#pragma pack(push, 1)

T_OTA_HEADER_FORMAT ota_header = {
	.ctrl_header = {
		.ic_type = 5,
		.secure_version = 0,
		.ctrl_flag.flag_value = {
            .xip = 0b1,
            .enc = 0b1,
            .load_when_boot = 0b1,
            .enc_load = 0b1,
            .enc_key_select = 0b111,
            .not_ready = 0b1,
            .not_obsolete = 0b1,
            .integrity_check_en_in_boot = 0b1,
            .rsvd = 0b111111,
		},
	    .image_id = OTA,
		.crc16 = 0,
		.payload_len = 0,
	},
	STRUCT_ARRAY_SETTER(ota_header, uuid, 0xff),
	.exe_base = 0xFFFFFFFF,
	.load_base = 0xFFFFFFFF,
	STRUCT_ARRAY_SETTER(ota_header, rsvd0, 0xff),
	.auto_enter_dfu_mode_pattern = 0xFFFF,
	.single_bank_ota_pattern = 0xFFFF,
	.magic_pattern = 0x5A5A12A5,
	STRUCT_ARRAY_SETTER(ota_header, rsvd1, 0xff),
	.git_ver = {
		.ver_info.version = 0xFFFFFFFF,
		._version_commitid = 0xFFFFFFFF,
	    STRUCT_ARRAY_SETTER(ota_header.git_ver, _customer_name, 0xff),
	},
	.rsaPubKey = {
		STRUCT_ARRAY_SETTER(ota_header.rsaPubKey, N, 0xff),
		STRUCT_ARRAY_SETTER(ota_header.rsaPubKey, E, 0xff),
	},
	STRUCT_ARRAY_SETTER(ota_header, sha256, 0xff),
    .ver_val = 0x01000001,
    .secure_boot_addr = BANK0_SECURE_BOOT_ADDR,
    .secure_boot_size = BANK0_SECURE_BOOT_SIZE,
    .rom_patch_addr = BANK0_ROM_PATCH_ADDR,
    .rom_patch_size = BANK0_ROM_PATCH_SIZE,
    .app_addr = BANK0_APP_ADDR,
    .app_size = BANK0_APP_SIZE,
    .app_data1_addr = BANK0_APP_DATA1_ADDR,
    .app_data1_size = BANK0_APP_DATA1_SIZE,
    .app_data2_addr = BANK0_APP_DATA2_ADDR,
    .app_data2_size = BANK0_APP_DATA2_SIZE,
    .app_data3_addr = BANK0_APP_DATA3_ADDR,
    .app_data3_size = BANK0_APP_DATA3_SIZE,
    .app_data4_addr = BANK0_APP_DATA4_ADDR,
    .app_data4_size = BANK0_APP_DATA4_SIZE,
    .app_data5_addr = BANK0_APP_DATA5_ADDR,
    .app_data5_size = BANK0_APP_DATA5_SIZE,
    .app_data6_addr = BANK0_APP_DATA6_ADDR,
    .app_data6_size = BANK0_APP_DATA6_SIZE,
};

#pragma pack(pop)
