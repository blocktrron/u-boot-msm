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

#define MSM_GPIO(x)		(1 << (31 - x))

static void set_led(int mask, int val)
{
	ccsr_gpio_t *pgpio = (void *)(CONFIG_SYS_MPC85xx_GPIO_ADDR);

	if (val)
		clrbits_be32(&pgpio->gpdat, mask);
	else
		setbits_be32(&pgpio->gpdat, mask);

	in_be32(&pgpio->gpdat);
}

enum animations {
	MSM_ANIMATION_PREBOOT = 0,
	MSM_ANIMATION_RECOVERY_START = 1,
	MSM_ANIMATION_RECOVERY_SUCCESS = 2,
	MSM_ANIMATION_BOOT = 3,
};

int do_msm_led(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int animation = MSM_ANIMATION_PREBOOT;
	int i;

	if (argc > 1) {
		if (!strcmp(argv[1], "preboot"))
			animation = MSM_ANIMATION_PREBOOT;
		else if (!strcmp(argv[1], "recovery-start"))
			animation = MSM_ANIMATION_RECOVERY_START;
		else if (!strcmp(argv[1], "recovery-success"))
			animation = MSM_ANIMATION_RECOVERY_SUCCESS;
		else if (!strcmp(argv[1], "boot"))
			animation = MSM_ANIMATION_BOOT;
		else
			return CMD_RET_USAGE;
	}

	if (animation == MSM_ANIMATION_PREBOOT) {
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
		#undef	LED_TOGGLE_DELAY
		#undef	LED_TOGGLE_COUNT
	} else if (animation == MSM_ANIMATION_RECOVERY_START) {
		#define LED_TOGGLE_DELAY	50
		#define LED_TOGGLE_COUNT	20
		for (i = 0; i < LED_TOGGLE_COUNT; i++) {
			set_led(MSM_GPIO(0), 1);
			set_led(MSM_GPIO(1), 1);
			set_led(MSM_GPIO(2), 1);
			set_led(MSM_GPIO(3), 1);
			mdelay(LED_TOGGLE_DELAY);
			set_led(MSM_GPIO(0), 0);
			set_led(MSM_GPIO(1), 0);
			set_led(MSM_GPIO(2), 0);
			set_led(MSM_GPIO(3), 0);
			mdelay(LED_TOGGLE_DELAY);
		}

		/* Keep Ethernet LED on */
		set_led(MSM_GPIO(1), 1);

		#undef	LED_TOGGLE_DELAY
		#undef	LED_TOGGLE_COUNT
	} else if (animation == MSM_ANIMATION_RECOVERY_SUCCESS) {
		#define LED_TOGGLE_DELAY	100
		#define LED_TOGGLE_COUNT	5
		for (i = 0; i < LED_TOGGLE_COUNT; i++) {
			set_led(MSM_GPIO(0), 1);
			set_led(MSM_GPIO(0), 1);
			set_led(MSM_GPIO(1), 0);
			set_led(MSM_GPIO(1), 0);
			set_led(MSM_GPIO(2), 0);
			set_led(MSM_GPIO(2), 0);
			set_led(MSM_GPIO(3), 1);
			set_led(MSM_GPIO(3), 1);
			mdelay(LED_TOGGLE_DELAY);
			set_led(MSM_GPIO(0), 0);
			set_led(MSM_GPIO(0), 0);
			set_led(MSM_GPIO(1), 1);
			set_led(MSM_GPIO(1), 1);
			set_led(MSM_GPIO(2), 1);
			set_led(MSM_GPIO(2), 1);
			set_led(MSM_GPIO(3), 0);
			set_led(MSM_GPIO(3), 0);
			mdelay(LED_TOGGLE_DELAY);
		}

		set_led(MSM_GPIO(0), 0);
		set_led(MSM_GPIO(0), 0);
		set_led(MSM_GPIO(1), 0);
		set_led(MSM_GPIO(1), 0);
		set_led(MSM_GPIO(2), 0);
		set_led(MSM_GPIO(2), 0);
		set_led(MSM_GPIO(3), 0);
		set_led(MSM_GPIO(3), 0);

		#undef	LED_TOGGLE_DELAY
		#undef	LED_TOGGLE_COUNT
	} else if (animation == MSM_ANIMATION_BOOT) {
		#define LED_TOGGLE_DELAY	100
		#define LED_TOGGLE_COUNT	5
		for (i = 0; i < LED_TOGGLE_COUNT; i++) {
			set_led(MSM_GPIO(0), 1);
			mdelay(LED_TOGGLE_DELAY);
			set_led(MSM_GPIO(0), 0);
			set_led(MSM_GPIO(1), 1);
			mdelay(LED_TOGGLE_DELAY);
			set_led(MSM_GPIO(1), 0);
			set_led(MSM_GPIO(2), 1);
			mdelay(LED_TOGGLE_DELAY);
			set_led(MSM_GPIO(2), 0);
			set_led(MSM_GPIO(3), 1);
			mdelay(LED_TOGGLE_DELAY);
			set_led(MSM_GPIO(3), 0);
		}

		#undef	LED_TOGGLE_DELAY
		#undef	LED_TOGGLE_COUNT
	}

	/* Turn on Power LED */
	set_led(MSM_GPIO(0), 1);

}

U_BOOT_CMD(
	msmled, CONFIG_SYS_MAXARGS, 0, do_msm_led,
	"Show LED animation",
	"Show LED animation"
);

static int msm_read_reset_button(void)
{
	ccsr_gpio_t *pgpio = (void *)(CONFIG_SYS_MPC85xx_GPIO_ADDR);
	return !!(in_be32(&pgpio->gpdat) & MSM_GPIO(5));
}

int do_msm_reset(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	unsigned int duration = 5;
	int led_state = 1;
	int ret;
	int i;

	if (argc > 1)
		duration = simple_strtoul(argv[1], NULL, 10);

	printf("Press reset button for %u seconds.\n", duration);

	/* Read reset-button value */
	for (i = 0; i < duration * 100; i++) {
		if (!msm_read_reset_button()) {
			set_led(MSM_GPIO(0), 1);
			return 1;
		}
		mdelay(10);
		puts(".");

		if (i % 5 == 0) {
			set_led(MSM_GPIO(0), led_state);
			led_state = !led_state;
		}
	}

	/* Reset was pressed */
	puts("\nReset was pressed.\n");

	set_led(MSM_GPIO(0), 1);

	return 0;
}

U_BOOT_CMD(
	msmrst, CONFIG_SYS_MAXARGS, 0, do_msm_reset,
	"Check reset button state",
	"[duration-sec]"
);

int do_msm_recovery(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	unsigned int duration = 5;
	int led_state = 1;
	int ret;
	int i;

	/* Display recovery-start animation */
	run_command("msmled recovery-start", 0);

	do {
		/* We have an environment already. Execute recoverycmd. */
		ret = run_command("run recoverycmd", 0);
	} while (ret);

	/* Keep in Recovery-success animation if successful*/
	if (!ret) {
		while (1) {
			run_command("msmled recovery-success", 0);
		}
	}

	return 0;
}

U_BOOT_CMD(
	msmrec, CONFIG_SYS_MAXARGS, 0, do_msm_recovery,
	"Start TFTP UBI recovery",
	""
);

int do_msm_preboot(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int ret;
	int i;

	puts("Hewlett-Packard MSM460 Pre-Boot\n");

	/* Display and set mac-address */
	run_command("msmmac", 0);

	/* Display preboot animation */
	run_command("msmled preboot", 0);

	puts("Press reset button to enter recovery mode...\n");

	ret = run_command("msmrst 5", 0);
	if (ret) {
		puts("Reset button not pressed, continuing boot...\n");

		/* Display boot animation */
		run_command("msmled boot", 0);

		return 0;
	}

	/* Reset was pressed */
	puts("\nReset was pressed, booting into recovery mode...\n");

	run_command("msmrec", 0);

	return 0;
}

U_BOOT_CMD(
	msmpb, CONFIG_SYS_MAXARGS, 0, do_msm_preboot,
	"Preboot procedure",
	"This will start the preboot procedure checking for reset button press"
);

static int msm_parse_ethaddr(char *address_string, uchar *output)
{
	uchar enetaddr[6];

	eth_parse_enetaddr(address_string, enetaddr);
	if (is_valid_ether_addr(enetaddr)) {
		memcpy(output, enetaddr, 6);
		return 0;
	}

	return -1;
}

int do_msm_setmac(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	uchar *bdinfo_buf = (uchar *)0x7000000;
	uchar *eth_mac = bdinfo_buf + 0x1F822;
	uchar *wlan_mac = bdinfo_buf + 0x1F9BD;
	char env_mac[17];
	int ret;

	if (argc == 2 || argc  > 3) {
		printf("Usage: msmmac <eth-mac-address> <wlan-mac-address>\n");
		return 1;
	}

	if (run_command("run bid_read", 0)) {
		printf("Failed to read bdinfo\n");
		return 1;
	}

	if (argc == 1) {
		printf("Ethernet MAC address: %pM\n", eth_mac);
		printf("WLAN MAC address: %pM\n", wlan_mac);

		/* Set in env */
		sprintf(env_mac, "%02X:%02X:%02X:%02X:%02X:%02X",
		eth_mac[0], eth_mac[1], eth_mac[2], eth_mac[3], eth_mac[4], eth_mac[5]);
		setenv("ethaddr", env_mac);

		return 0;
	}

	ret = msm_parse_ethaddr(argv[1], eth_mac);
	if (ret) {
		printf("Invalid Ethernet MAC address\n");
		return 1;
	}

	ret = msm_parse_ethaddr(argv[2], wlan_mac);
	if (ret) {
		printf("Invalid WLAN MAC address\n");
		return 1;
	}

	if (run_command("run bid_write", 0)) {
		printf("Failed to read bdinfo\n");
		return 1;
	}

	return 0;
}

U_BOOT_CMD(
	msmmac, CONFIG_SYS_MAXARGS, 0, do_msm_setmac,
	"Set board MAC address",
	"<eth-mac-address> <wlan-mac-address>"
);