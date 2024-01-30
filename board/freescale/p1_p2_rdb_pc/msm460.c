/*
 * Copyright 2024 David Bauer <mail@david-bauer.net>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <command.h>
#include <hwconfig.h>
#include <pci.h>
#include <i2c.h>
#include <asm/processor.h>
#include <asm/mmu.h>
#include <asm/cache.h>
#include <asm/immap_85xx.h>
#include <asm/fsl_pci.h>
#include <fsl_ddr_sdram.h>
#include <asm/io.h>
#include <asm/fsl_law.h>
#include <asm/fsl_lbc.h>
#include <asm/mp.h>
#include <miiphy.h>
#include <libfdt.h>
#include <fdt_support.h>
#include <fsl_mdio.h>
#include <tsec.h>
#include <vsc7385.h>
#include <ioports.h>
#include <asm/fsl_serdes.h>
#include <netdev.h>

static void set_led(int mask, int val)
{
	ccsr_gpio_t *pgpio = (void *)(CONFIG_SYS_MPC85xx_GPIO_ADDR);

	if (val)
		clrbits_be32(&pgpio->gpdat, mask);
	else
		setbits_be32(&pgpio->gpdat, mask);

	in_be32(&pgpio->gpdat);
}

int do_msm_led_animation(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int i;

	#define MSM_GPIO(x)		(1 << (31 - x))
	#define LED_TOGGLE_DELAY	100
	#define LED_TOGGLE_COUNT	3

	for (i = 0; i < LED_TOGGLE_COUNT; i++) {
		set_led(MSM_GPIO(0), 1);
		mdelay(LED_TOGGLE_DELAY);
		set_led(MSM_GPIO(1), 1);
		mdelay(LED_TOGGLE_DELAY);
		set_led(MSM_GPIO(2), 1);
		mdelay(LED_TOGGLE_DELAY);
		set_led(MSM_GPIO(3), 1);
		mdelay(LED_TOGGLE_DELAY);
		set_led(MSM_GPIO(0), 0);
		mdelay(LED_TOGGLE_DELAY);
		set_led(MSM_GPIO(1), 0);
		mdelay(LED_TOGGLE_DELAY);
		set_led(MSM_GPIO(2), 0);
		mdelay(LED_TOGGLE_DELAY);
		set_led(MSM_GPIO(3), 0);
		mdelay(LED_TOGGLE_DELAY);
	}

	/* Turn on Power LED */
	set_led(MSM_GPIO(0), 1);

	#undef	MSM_GPIO
	#undef	LED_TOGGLE_DELAY
	#undef	LED_TOGGLE_COUNT
}

U_BOOT_CMD(
	msm_led_animation, CONFIG_SYS_MAXARGS, 0, do_wdog_toggle,
	"Show LED animation",
	"Show LED animation"
);