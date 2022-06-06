#include <rtl876x_rcc.h>

#ifndef _RCC_H
#define _RCC_H

typedef enum {
    ADC_PERIPH = APBPeriph_ADC,
    CODEC_PERIPH = APBPeriph_CODEC,
    FLASH_PERIPH = APBPeriph_FLASH,
    GDMA_PERIPH = APBPeriph_GDMA,
    GPIO_PERIPH = APBPeriph_GPIO,
    I2C0_PERIPH = APBPeriph_I2C0,
    I2C1_PERIPH = APBPeriph_I2C1,
    I2S0_PERIPH = APBPeriph_I2S0,
    I2S1_PERIPH = APBPeriph_I2S1,
    IR_PERIPH = APBPeriph_IR,
    KEYSCAN_PERIPH = APBPeriph_KEYSCAN,
    LCD_PERIPH = APBPeriph_LCD,
    QDEC_PERIPH = APBPeriph_QDEC,
    SPI0_PERIPH = APBPeriph_SPI0,
    SPI1_PERIPH = APBPeriph_SPI1,
    SPI2W_PERIPH = APBPeriph_SPI2W,
    TIMER_PERIPH = APBPeriph_TIMER,
    UART0_PERIPH = APBPeriph_UART0,
    UART1_PERIPH = APBPeriph_UART1,
    UART2_PERIPH = APBPeriph_UART2,
} abp_periph_t;

typedef enum {
    ADC_CLOCK = APBPeriph_ADC_CLOCK,
    CODEC_CLOCK = APBPeriph_CODEC_CLOCK,
    FLASH_CLOCK = APBPeriph_FLASH_CLOCK,
    GDMA_CLOCK = APBPeriph_GDMA_CLOCK,
    GPIO_CLOCK = APBPeriph_GPIO_CLOCK,
    I2C0_CLOCK = APBPeriph_I2C0_CLOCK,
    I2C1_CLOCK = APBPeriph_I2C1_CLOCK,
    I2S0_CLOCK = APBPeriph_I2S0_CLOCK,
    I2S1_CLOCK = APBPeriph_I2S1_CLOCK,
    IR_CLOCK = APBPeriph_IR_CLOCK,
    KEYSCAN_CLOCK = APBPeriph_KEYSCAN_CLOCK,
    LCD_CLOCK = APBPeriph_LCD_CLOCK,
    QDEC_CLOCK = APBPeriph_QDEC_CLOCK,
    SPI0_CLOCK = APBPeriph_SPI0_CLOCK,
    SPI1_CLOCK = APBPeriph_SPI1_CLOCK,
    SPI2W_CLOCK = APBPeriph_SPI2W_CLOCK,
    TIMER_CLOCK = APBPeriph_TIMER_CLOCK,
    UART0_CLOCK = APBPeriph_UART0_CLOCK,
    UART1_CLOCK = APBPeriph_UART1_CLOCK,
    UART2_CLOCK = APBPeriph_UART2_CLOCK,
} abp_clock_t;

typedef struct {
    const abp_periph_t abp_periph;
    const abp_clock_t abp_clock;
} rcc_periph_t;

#define RCC_PERIPH(PERIPH)                                          \
    {                                                               \
        .abp_periph = PERIPH##_PERIPH, .abp_clock = PERIPH##_CLOCK, \
    }

static inline void rcc_periph_set(const rcc_periph_t* rcc_periph_p, bool state)
{
    RCC_PeriphClockCmd(rcc_periph_p->abp_periph, rcc_periph_p->abp_clock, state);
}

#endif //_RCC_H
