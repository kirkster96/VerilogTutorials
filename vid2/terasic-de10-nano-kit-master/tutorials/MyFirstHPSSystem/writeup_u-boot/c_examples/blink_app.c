/*
 * Copyright (c) 2017 Intel Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include <common.h>
#include <exports.h>

#include "hps_0_arm_a9_0.h"

/* Macro for reading from memory */
#define readl(addr)	*((volatile unsigned int *)(addr))

/* Macro for writing to memory */
#define writel(value,addr)	*((volatile unsigned int *)(addr)) = (value)

#define LED_DELAY_US 250000

void demo_sysid(void);
void demo_ocram(void);
void demo_leds(void);
void demo_switches(void);
void demo_buttons(void);

int blink_app(int argc, char * const argv[]) {

	int i;

	/* Start application */
	app_startup(argv);

	/* Display welcome message */
	printf("\n");
	printf("Blink application started.\n");
	printf("\n");

	/* Display command line parameters */
	printf("Number of command line parameters: %d\n", argc);
	for(i=0; i<argc; i++)
		printf("Command line parameter %d = %s\n", (i + 1), argv[i]);

	printf("\n");

	/* Display U-Booot ABI version */
	printf("U-Boot ABI version %d\n", (int)get_version());
	printf("\n");

	/* Exercise FPGA IP */
	demo_sysid();
	demo_ocram();
	demo_leds();
	demo_switches();
	demo_buttons();

	/* Goodbye message */
	printf("Blink Application completed.\n");
	printf("\n");

	return(0);
}

void demo_sysid(void) {

	/* Display SysID parameters */
	printf("SysID.id = 0x%08x\n", (int)readl(SYSTEM_ID_BASE));
	printf("SysID.timestamp = 0x%08x\n", (int)readl(SYSTEM_ID_BASE + 0x4));
	printf("\n");
}

void demo_ocram(void) {

	unsigned int * ptr = (unsigned int *)OCRAM_64K_BASE;
	unsigned int i;

	printf("Writing pattern into FPGA OCRAM.\n");
	for(i = 0; i< OCRAM_64K_SIZE_VALUE / sizeof(unsigned int); i++)
		ptr[i] = i;

	printf("Checking pattern from FPGA OCRAM: ");
	for(i = 0; i< OCRAM_64K_SIZE_VALUE / sizeof(unsigned int); i++) {
		if(ptr[i] != i) {
			printf("FAILED.\n");
			return;
		}
	}

	printf("passed.\n");
	printf("\n");
}

void demo_leds(void) {

	unsigned int led = 0;

	/* Blink the LEDs until any key is pressed */
	printf("Blinking LEDs\n");
	printf("Press any key to stop LED lightshow\n");
	while(!tstc()) {

		/* Turn on LED */
		writel(1L << led, LED_PIO_BASE);

		/* Delay */
		udelay(LED_DELAY_US);

		/* Advance to next LED */
		led = (led + 1) % LED_PIO_DATA_WIDTH;
	}

	/* Discard the key that was pressed */
	(void) getc();
	printf("\n");
}

void demo_switches(void) {

	unsigned int switches, prev_switches;

	/* Display state of switches */
	printf("Displaying the state of switches\n");
	printf("Slide switch SW0, Sw1, SW2 or SW3 to observe updates\n");
	printf("Press any key to stop displaying the switches\n");
	prev_switches = ~readl(SWITCH_PIO_BASE);

	while(!tstc()) {
		switches = readl(SWITCH_PIO_BASE);
		if(switches != prev_switches) {
			printf("Switch value: 0x%01x\n", (int)switches);
			prev_switches = switches;
		}
	}

	/* Discard the key that was pressed */
	(void) getc();
	printf("\n");
}

void demo_buttons(void) {

	unsigned int buttons, prev_buttons;

	/* Display state of buttons */
	printf("Displaying the state of buttons\n");
	printf("Press push button KEY1 to observe updates\n");
	printf("Press any key to stop displaying the buttons\n");
	prev_buttons = ~readl(BUTTON_PIO_BASE);

	while(!tstc()) {
		buttons = readl(BUTTON_PIO_BASE);
		if(buttons != prev_buttons) {
			printf("Button value: 0x%01x\n", (int)buttons);
			prev_buttons = buttons;
		}
	}

	/* Discard the key that was pressed */
	(void) getc();
	printf("\n");
}

