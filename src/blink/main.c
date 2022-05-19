#include <rtl876x.h>
#include <rtl876x_wdg.h>
#include <rtl876x_pinmux.h>
#include <rtl876x_rcc.h>
#include <rtl876x_gpio.h>
#include <platform_utils.h>

#define LED_PAD P0_6
#define LED_GPIO_PIN GPIO_GetPin(LED_PAD)

static void toggle_led() {
    GPIO_WriteBit(LED_GPIO_PIN, !GPIO_ReadOutputDataBit(LED_GPIO_PIN));
}

int main(void)
{
    // WDG_Disable();

    Pad_Config(LED_PAD, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_HIGH);

    Pinmux_Config(LED_PAD, DWGPIO);

    RCC_PeriphClockCmd(APBPeriph_GPIO, APBPeriph_GPIO_CLOCK, ENABLE);

    GPIO_InitTypeDef Gpio_Struct;
    GPIO_StructInit(&Gpio_Struct);
    Gpio_Struct.GPIO_Pin = LED_GPIO_PIN;
    Gpio_Struct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_Init(&Gpio_Struct);

    while (1)
    {
        toggle_led();
        platform_delay_ms(1000);
        // for(unsigned u=0; u<5; u++){
        //     // __NOP();
        // }
    }
}
