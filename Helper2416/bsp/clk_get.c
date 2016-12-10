#include <raw_api.h>
#include <2416_reg.h>
#include <uart.h>


#define MPLL 0
#define UPLL 1


/* input clock of PLL */
#define CONFIG_SYS_CLK_FREQ	12000000	/* the SMDK2416 has 12MHz input clock */

/* ------------------------------------------------------------------------- */
/* NOTE: This describes the proper use of this file.
 *
 * CONFIG_SYS_CLK_FREQ should be defined as the input frequency of the PLL.
 *
 * get_FCLK(), get_HCLK(), get_PCLK() and get_UCLK() return the clock of
 * the specified bus in HZ.
 */
/* ------------------------------------------------------------------------- */

static RAW_U32 get_PLLCLK(int pllreg)
{
	RAW_U32 r, m, p, s;

	r = MPLLCON_REG;
	m = ((r>>14) & 0x3ff);
	p = ((r>>5) & 0x3f);
	s = r & 0x7;

	/* XXX: To Do calculate UPLL */

	return (m * (CONFIG_SYS_CLK_FREQ / (p << s)));
}

/* return FCLK frequency */
RAW_U32 get_FCLK(void)
{
	return(get_PLLCLK(MPLL));
}

/* return HCLK frequency */
RAW_U32 get_HCLK(void)
{
	RAW_U32 hclk_div = (CLKDIV0CON_REG & 0x3) + 1;
	RAW_U32 pre_div = ((CLKDIV0CON_REG>>4) & 0x3) + 1;

	return get_FCLK()/(hclk_div * pre_div);
}

/* return PCLK frequency */
RAW_U32 get_PCLK(void)
{
	return ((CLKDIV0CON_REG & 0x4) ? get_HCLK()/2 : get_HCLK());
}

/* return UCLK frequency */
RAW_U32 get_UCLK(void)
{
	return (get_PLLCLK(UPLL));
}

/* return ARMCORE frequency */
RAW_U32 get_ARMCLK(void)
{
	RAW_U32 div_val = CLKDIV0CON_REG;
	RAW_U32 fclk = get_FCLK();
	RAW_U32 arm_clk = 0;

	switch((div_val >> 9) & 0xf) {
	case 0:
		arm_clk = fclk;
		break;

	case 1:
		arm_clk = fclk/2;
		break;

	case 2:
		arm_clk = fclk/3;
		break;

	case 3:
		arm_clk = fclk/4;
		break;

	case 5:
		arm_clk = fclk/6;
		break;

	case 7:
		arm_clk = fclk/8;
		break;

	case 13:
		arm_clk = fclk/12;
		break;

	case 15:
		arm_clk = fclk/16;
		break;

	}

	return arm_clk;
}

void print_cpuinfo(void)
{
	Uart_Printf("\nCPU:   S3C2416@%luHz\n", get_ARMCLK());
	Uart_Printf("       Fclk = %luHz, Hclk = %luHz, Pclk = %luHz\n",
			get_FCLK(), get_HCLK(), get_PCLK());

}


