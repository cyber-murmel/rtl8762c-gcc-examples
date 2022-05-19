/* Entry Point */
ENTRY(Reset_Handler)
INPUT(mem_define.ld)

MEMORY
{
APP_DATA_ON (rwx)   : ORIGIN = 0x00207C00, LENGTH = APP_GLOBAL_SIZE
HEAP_DATA_ON (rwx)  : ORIGIN = (0x00207C00 + APP_GLOBAL_SIZE) , LENGTH = (65*1024) - APP_GLOBAL_SIZE
CACHE_DATA_ON (rwx)   : ORIGIN = 0x0021C000, LENGTH = SHARE_CACHE_RAM_SIZE
APP_FLASH (r)   : ORIGIN = APP_ADDR, LENGTH = APP_SIZE
UNINIT_RAM : ORIGIN = 0x00217f00, LENGTH = 256
}

/* Define output sections */
SECTIONS
{
    . = APP_ADDR;
    app.bin APP_ADDR :
    {
        KEEP(* (.app.flash.header))
        KEEP(* (.app.flash.header.auth))
    }AT > APP_FLASH
    APP_FLASH_HEADER_EXT  :
    {
        KEEP(* (.app.flash.header_ext))
    }AT > APP_FLASH

    FLASH_START_ADDR :
    {
         build/sdk/src/mcu/rtl876x/arm/startup_rtl8762c_gcc.o(RESET)
         . = ALIGN(4);
    __flash_start_load_ad__ = LOADADDR(FLASH_START_ADDR);
    __flash_start_exe_ad__ = ADDR(FLASH_START_ADDR);
    }AT > APP_FLASH

    FLASH_TEXT  :
    {
        *(.text*)
        . = ALIGN(4);
        *(.rodata*)
        . = ALIGN(4);
        *(.app.flash.text)
        *(.app.flash.rodata)
    }AT > APP_FLASH
    RAM_VECTOR_TABLE 0x00200000:
    {
        __ram_vector_table_start__ = .;
        KEEP(*(VECTOR))
        __ram_vector_table_end__ = .;
    }AT > APP_FLASH
    __ram_vector_load_ad__ = LOADADDR(RAM_VECTOR_TABLE);
    __ram_vector_table_length__ = __ram_vector_table_end__ - __ram_vector_table_start__;
    RAM_DATA_ON  :
    {
        . = ALIGN(4);
        __ram_dataon_rw_start__ = .;
        *(.data*)
        * (.ram.dataon.data)
        __ram_dataon_rw_end__ = .;
        . = ALIGN(4);
    } > APP_DATA_ON  AT > APP_FLASH
    BSS (NOLOAD):
    {
        . = ALIGN(4);
        __ram_dataon_zi_start__ = .;
        * (.ram.dataon.bss*)
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        __ram_dataon_zi_end__ = .;
    } > APP_DATA_ON  AT > APP_FLASH
    __ram_rw_load_ad__ = LOADADDR(RAM_DATA_ON);
    __ram_dataon_rw_length__ = __ram_dataon_rw_end__ - __ram_dataon_rw_start__;
    __ram_dataon_zi_length__ = __ram_dataon_zi_end__ - __ram_dataon_zi_start__;

    DATA_RAM_UNIT  :
    {
        * (.uninit.ram)
    } > UNINIT_RAM AT > APP_FLASH

    HEAP_DATA_ON  :
    {
        __heap_start__ = .;
        end = __heap_start__;
        _end = end;
        __end = end;
        KEEP(*(.heap))
        __heap_end__ = .;
        __HeapLimit = __heap_end__;
    } > HEAP_DATA_ON  AT > APP_FLASH

    CACHE_DATA_ON (OVERLAY):
    {
        __cache_dataon_start__ = . ;
        *(.ram.sharecacheram.text)
        __cache_dataon_end__ = .;
    }>CACHE_DATA_ON AT> APP_FLASH
    __cache_dataon_length__ = __cache_dataon_end__ - __cache_dataon_start__;
    __cache_data_load_ad__ = LOADADDR(RAM_DATA_ON);
}
