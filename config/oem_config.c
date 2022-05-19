#include <stddef.h>

#include "config/flash_map.h"

// parameters
#define BT_MAC_ADDRESS {0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff}

enum { LDO, SWR };
#define LDO_SWR_MODE LDO

#define RUN_MODE (NORMAL)

#define TX_POWER_LE1M (GAIN_7_5_DBM)

#define OTA_AES_KEY {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F}

#define XTAL_CAP_CAL (0x48)

// helpers
#define PRE_OTA_AES_KEY (0x01)

#if (LDO_SWR_MODE==LDO)
#define LDO_SWR_0 (0x00)
#define LDO_SWR_1 (0x00)
#define LDO_SWR_2 (0x00)
#define LDO_SWR_3 (0x00)
#elif (LDO_SWR_MODE==SWR)
#define LDO_SWR_0 (0x04)
#define LDO_SWR_1 (0x04)
#define LDO_SWR_2 (0x80)
#define LDO_SWR_3 (0x01)
#endif

#define STRUCT_ARRAY_SETTER(NAME, FIELD, VALUE) .FIELD = {[0 ... (sizeof(NAME.FIELD)-1)] = VALUE}

// data structures
#pragma pack(push, 1)

typedef enum {
	NORMAL = 0x01,
	MP = 0x00,
} run_mode_t;

typedef enum {
	GAIN_NEG20_DBM = 0x30,
	GAIN_0_DBM = 0x80,
	GAIN_3_DBM = 0x90,
	GAIN_4_DBM = 0xa0,
	GAIN_7_5_DBM = 0xd0,
} tx_power_t;

typedef struct {
	enum {
		BT_MAC_ADDRESS_ID 	 		= 0x0033,
		XTAL_CAP_CAL_ID 			= 0x0148,
		RUN_MODE_ID					= 0x0178,
		PRE_OTA_AES_KEY_ID			= 0x01B0,
		LDO_SWR_0_ID				= 0x01F0,
		LDO_SWR_1_ID				= 0x01F7,
		LDO_SWR_2_ID				= 0x0208,
		LDO_SWR_3_ID				= 0x0209,
		OTA_AES_KEY_ID				= 0x0238,
		OTA_BANK0_ADDR_ID           = 0x0280,
		OTA_BANK0_SIZE_ID           = 0x0284,
		OTA_BANK1_ADDR_ID           = 0x0288,
		OTA_BANK1_SIZE_ID           = 0x028c,
		FTL_ADDR_ID 				= 0x0290,
		FTL_SIZE_ID 				= 0x0294,
		OTA_TMP_ADDR_ID 			= 0x0298,
		OTA_TMP_SIZE_ID 			= 0x029c,
		TX_POWER_LE1M_ID 			= 0x0347,
		TX_POWER_LE2M_ID 			= 0x0348,
		TX_POWER_LE2M_2402MHZ_ID 	= 0x0349,
		TX_POWER_LE2M_2480MHZ_ID 	= 0x034a,
	} id : 16;
	unsigned len: 8;
} entry_header_t;

#define SINGLE_ENTRY_TYPE(type) struct { \
	entry_header_t header; \
	type data; \
	char mask[sizeof(type)]; \
}

#define ARRAY_ENTRY_TYPE(length) struct { \
	entry_header_t header; \
	char data[length]; \
	char mask[length]; \
}

typedef struct {
	char magic[8];
    unsigned size:32;
	char _ff0[0x168];
	char hash[32];
	char _ff1[0x26c];
} header_t;

typedef struct {
	unsigned int magic;
	unsigned short len;

	#ifdef BT_MAC_ADDRESS
	ARRAY_ENTRY_TYPE(6) mac;
	#endif

	#ifdef LDO_SWR_MODE
	SINGLE_ENTRY_TYPE(unsigned char) ldo_swr[4];
	#endif

	#ifdef RUN_MODE
	SINGLE_ENTRY_TYPE(run_mode_t) run_mode;
	#endif

	#ifdef TX_POWER_LE1M
	SINGLE_ENTRY_TYPE(tx_power_t) tx_power_le1m;
	#endif

	#ifdef OTA_BANK0_ADDR
	SINGLE_ENTRY_TYPE(unsigned int) ota_bank0_addr;
	#endif

	#ifdef OTA_BANK0_SIZE
	SINGLE_ENTRY_TYPE(unsigned int) ota_bank0_size;
	#endif

	#ifdef OTA_BANK1_ADDR
	SINGLE_ENTRY_TYPE(unsigned int) ota_bank1_addr;
	#endif

	#ifdef OTA_BANK1_SIZE
	SINGLE_ENTRY_TYPE(unsigned int) ota_bank1_size;
	#endif

	#ifdef FTL_ADDR
	SINGLE_ENTRY_TYPE(unsigned int) ftl_addr;
	#endif

	#ifdef FTL_SIZE
	SINGLE_ENTRY_TYPE(unsigned int) ftl_size;
	#endif

	#ifdef OTA_TMP_ADDR
	SINGLE_ENTRY_TYPE(unsigned int) ota_tmp_addr;
	#endif

	#ifdef OTA_TMP_SIZE
	SINGLE_ENTRY_TYPE(unsigned int) ota_tmp_size;
	#endif

	#ifdef OTA_AES_KEY
	SINGLE_ENTRY_TYPE(unsigned char) pre_ota_aes_key;
	ARRAY_ENTRY_TYPE(32) ota_aes_key;
	#endif

	#ifdef XTAL_CAP_CAL
	SINGLE_ENTRY_TYPE(unsigned char) xtal_cap_cal;
	#endif
} config_t;

typedef struct {
	header_t header;
	config_t config;
} oem_config_t;

#define ENTRY_VALUE(CONFIG, ENTRY, VALUE) .ENTRY = { \
	.header = { \
		.id = VALUE##_ID, \
		.len = sizeof(CONFIG.ENTRY.data) \
	}, \
	.data = VALUE, \
	.mask = {[0 ... (sizeof(CONFIG.ENTRY.mask)-1)] = 0xFF}, \
},

#define ENTRY_VALUE_MASK(CONFIG, ENTRY, VALUE, MASK) .ENTRY = { \
	.header = { \
		.id = VALUE##_ID, \
		.len = sizeof(CONFIG.ENTRY.data) \
	}, \
	.data = VALUE, \
	.mask = MASK, \
},

oem_config_t oem_config = {
	.header = {
		.magic = "\x05\x00\xf0\xff\x8d\x27\x00\x00",
		.size = sizeof(config_t),
		STRUCT_ARRAY_SETTER(oem_config.header, _ff0, 0xff),
		STRUCT_ARRAY_SETTER(oem_config.header, hash, 0xff),
		STRUCT_ARRAY_SETTER(oem_config.header, _ff1, 0xff),
	},
	.config	= {
		.magic = 0x8721bee2,
		// measure from end of len field to end of struct
		.len = sizeof(config_t) - offsetof(config_t, len) - sizeof(oem_config.config.len),

		#ifdef BT_MAC_ADDRESS
		ENTRY_VALUE(oem_config.config, mac, BT_MAC_ADDRESS)
		#endif

		#ifdef LDO_SWR_MODE
		ENTRY_VALUE_MASK(oem_config.config, ldo_swr[0], LDO_SWR_0, {0x04})
		ENTRY_VALUE_MASK(oem_config.config, ldo_swr[1], LDO_SWR_1, {0x04})
		ENTRY_VALUE_MASK(oem_config.config, ldo_swr[2], LDO_SWR_2, {0x80})
		ENTRY_VALUE_MASK(oem_config.config, ldo_swr[3], LDO_SWR_3, {0x01})
		#endif

		#ifdef RUN_MODE
		ENTRY_VALUE_MASK(oem_config.config, run_mode, RUN_MODE, {0x01})
		#endif

		#ifdef TX_POWER_LE1M
		ENTRY_VALUE(oem_config.config, tx_power_le1m, TX_POWER_LE1M)
		#endif

		#ifdef OTA_BANK0_ADDR
		ENTRY_VALUE(oem_config.config, ota_bank0_addr, OTA_BANK0_ADDR)
		#endif

		#ifdef OTA_BANK0_SIZE
		ENTRY_VALUE(oem_config.config, ota_bank0_size, OTA_BANK0_SIZE)
		#endif

		#ifdef OTA_BANK1_ADDR
		ENTRY_VALUE(oem_config.config, ota_bank1_addr, OTA_BANK1_ADDR)
		#endif

		#ifdef OTA_BANK1_SIZE
		ENTRY_VALUE(oem_config.config, ota_bank1_size, OTA_BANK1_SIZE)
		#endif

		#ifdef FTL_ADDR
		ENTRY_VALUE(oem_config.config, ftl_addr, FTL_ADDR)
		#endif

		#ifdef FTL_SIZE
		ENTRY_VALUE(oem_config.config, ftl_size, FTL_SIZE)
		#endif

		#ifdef OTA_TMP_ADDR
		ENTRY_VALUE(oem_config.config, ota_tmp_addr, OTA_TMP_ADDR)
		#endif

		#ifdef OTA_TMP_SIZE
		ENTRY_VALUE(oem_config.config, ota_tmp_size, OTA_TMP_SIZE)
		#endif

		#ifdef OTA_AES_KEY
		ENTRY_VALUE(oem_config.config, pre_ota_aes_key, PRE_OTA_AES_KEY)
		ENTRY_VALUE(oem_config.config, ota_aes_key, OTA_AES_KEY)
		#endif

		#ifdef XTAL_CAP_CAL
		ENTRY_VALUE(oem_config.config, xtal_cap_cal, XTAL_CAP_CAL)
		#endif
	},
};

#pragma pack(pop)
