// UART baud rate calculator for RTL8762C and probably RTL8762D, RTL8762E, RTL87x3x, etc.
// this header file is supposed to be included in a source file, not header file.

// by wuwbobo2021 <https://github.com/wuwbobo2021>, <wuwbobo@outlook.com>
// inspired by RTL8195A UART baudrate calculation code in sdk-ameba-v4.0b-gcc,
// and measurements from the oscilloscope. free for any proper usage, but without warranty.

// Date: 2023-09-26

#ifndef RTLBAUD_H
#define RTLBAUD_H

#include <stdbool.h>
#include <stdint.h>
#include <math.h>

#ifdef RTL_UART_CALC_DEBUG
	#define dbg_print_args(str, ...) printf(str, __VA_ARGS__)
#else
	#define dbg_print_args(str, ...)
#endif

typedef struct {
	uint16_t div;
	uint16_t ovsr;
	uint16_t ovsr_adj;
	uint32_t baud_actual;
	bool is_valid;

	// set by user
	uint32_t baud_target;
	float max_err_percent;
} Rtl_Uart_BaudRate_Config;

#define T_CLK (1.0/(20*1000*1000))

static uint16_t Adj_Table[] =
	{0x000, 0x010, 0x022, 0x052, 0x0aa, 0x155, 0x16d, 0x1bb, 0x1f7};

static bool rtl_baud_calc(Rtl_Uart_BaudRate_Config* conf)
{
	conf->is_valid = false;
	if (conf->baud_target < 50) return false;

	double t_baud_target = 1.0 / conf->baud_target;

	for (int ovsr = 15; ovsr >= 1; ovsr--) {
		double ovsr_actual = 0.5*ovsr + 2.5;
		double div_tmp = (t_baud_target / ovsr_actual) / T_CLK;
		if (floor(div_tmp) > UINT16_MAX) continue;

		uint16_t div = floor(div_tmp);
		float t_clk_divided = T_CLK*div;
		double t_baud_err = t_baud_target - t_clk_divided*ovsr_actual;
		double t_adj_unit = t_clk_divided / 2.0 / 9.0;
		uint16_t adj_bits = round(t_baud_err / t_adj_unit);
		if (adj_bits > 8) continue;

		dbg_print_args("t_baud_err: %f us  t_adj_unit: %f us \n",
			t_baud_err*1000.0*1000.0, t_adj_unit*1000.0*1000.0);

		float t_baud_actual = t_clk_divided*ovsr_actual + t_adj_unit*adj_bits;
		float err_percent = 100.0 * fabs(t_baud_actual - t_baud_target)
		                          / t_baud_target;
		if (err_percent > conf->max_err_percent) continue;

		uint32_t baud_actual = round(1.0/t_baud_actual);
		dbg_print_args(
			"div: %u  ovsr_actual: %f  adj_bits: %u  err: %f percent  baud: %u \n",
			div, ovsr_actual, adj_bits, err_percent, baud_actual
		);

		#ifdef RTL_UART_CALC_DEBUG
			continue;
		#endif

		conf->is_valid = true;
		conf->div = div; conf->ovsr = ovsr;
		conf->ovsr_adj = Adj_Table[adj_bits];
		conf->baud_actual = baud_actual;
		return true;
	}

	return false;
}

static Rtl_Uart_BaudRate_Config rtl_baud_auto_calc(uint32_t baud)
{
	Rtl_Uart_BaudRate_Config conf;
	conf.is_valid = false;
	if (baud < 50) return conf;

	for (float err_per = 1.0; err_per <= 5.0; err_per += 0.5) {
		conf.baud_target = baud;
		conf.max_err_percent = err_per;
		if (rtl_baud_calc(&conf)) return conf;
	}

	return conf;
}

#endif //RTLBAUD_H
