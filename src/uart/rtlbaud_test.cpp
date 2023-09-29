// code for testing rtlbaud on PC

#include <stdlib.h>
#include <stdio.h>

//#define RTL_UART_CALC_DEBUG
#include "rtlbaud.h"

int main()
{
	int baud = 0;
	printf("input baudrate target: ");
	scanf("%u", &baud);
	
	Rtl_Uart_BaudRate_Config conf;

	#ifdef RTL_UART_CALC_DEBUG
		conf.baud_target = baud;
		conf.max_err_percent = 3;
		rtl_baud_calc(&conf);
	#else
		conf = rtl_baud_auto_calc(baud);
		if (conf.is_valid) {
			printf("div: %u  ovsr: %u  ovsr_adj: %#x  baud: %u \n",
				conf.div, conf.ovsr, conf.ovsr_adj, conf.baud_actual);
		}
	#endif

	return 0;
}
