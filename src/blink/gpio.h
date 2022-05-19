#ifndef _GPIO_H
#define _GPIO_H

#include "core_cm4.h"                       /* Processor and core peripherals */

#define REG_ADDR_GPIO 0x40001000UL
#define gpio_dir(NAME, DIR) gpio->DATADIR.pins._##NAME = DIR
#define gpio_data(NAME, DIR) gpio->DATAOUT.pins._##NAME = DIR

typedef union {
    struct {
        int _P0_0: 1;
        int _P0_1: 1;
        int _P0_2: 1;
        int _P0_3: 1;
        int _P0_4: 1;
        int _P0_5: 1;
        int _P0_6: 1;
        int _P0_7: 1;
        int _P1_0: 1;
        int _P1_1: 1;
        int _P1_2: 1;
        int _P1_3: 1;
        int _P1_4: 1;
        int _P1_5: 1;
        int _P1_6: 1;
        int _P1_7: 1;
        int _P2_0: 1;
        int _P2_1: 1;
        int _P2_2: 1;
        int _P2_3: 1;
        int _P2_4: 1;
        int _P2_5: 1;
        int _P2_6: 1;
        int _P2_7: 1;
        int _P3_0_H_0: 1;
        int _P3_1_H_1: 1;
        int _P3_2_H_2: 1;
        int _P3_3: 1;
        int _P3_4_P4_0: 1;
        int _P3_5_P4_1: 1;
        int _P3_6_P4_2: 1;
        int _P4_3: 1;
    } pins;
    unsigned int value: 32;
} gpio_register_t;

struct gpio_reg_s
{
    __IO gpio_register_t DATAOUT;                              /*!< Data register: data output */
    __IO gpio_register_t DATADIR;                              /*!< Data direction register */
    __IO gpio_register_t DATASRC;                              /*!< Data source register  */
    gpio_register_t RSVD[9];
    __IO gpio_register_t INTEN;                                /*!< Interrupt enable register */
    __IO gpio_register_t INTMASK;                              /*!< Interrupt mask register */
    __IO gpio_register_t INTTYPE;                              /*!< Interrupt level register */
    __IO gpio_register_t INTPOLARITY;                          /*!< Interrupt polarity register */
    __IO gpio_register_t INTSTATUS;                            /*!< Interrupt status of Port A  */
    __IO gpio_register_t RAWINTSTATUS;           /*!< Raw interrupt status of Port A (premasking) */
    __IO gpio_register_t DEBOUNCE;                             /*!< Debounce enable register */
    __O  gpio_register_t INTCLR;                               /*!< clear interrupt register */
    __I  gpio_register_t DATAIN;                               /*!< external port register */
    gpio_register_t RSVD1[3];
    __IO gpio_register_t LSSYNC;                 /*!< Level-sensitive synchronization enable register*/
    __I  gpio_register_t IDCODE;                               /*!< ID code register */
    __IO gpio_register_t INTBOTHEDGE;                          /*!< Both edge to trigger interrupt*/
} *gpio = ((struct gpio_reg_s *) REG_ADDR_GPIO);

#endif // _GPIO_H
