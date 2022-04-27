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

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <error.h>
#include <errno.h>
#include <string.h>
#include <sys/mman.h>
#include <termios.h>
#include <sys/utsname.h>

#include "hps_0_arm_a9_0.h"

/* Expected System Environment */
#define SYSFS_FPGA0_STATE_PATH "/sys/class/fpga_manager/fpga0/state"
#define SYSFS_FPGA0_STATE "operating"

#define SYSFS_LWH2F_BRIDGE_NAME_PATH "/sys/class/fpga_bridge/br0/name"
#define SYSFS_LWH2F_BRIDGE_NAME "lwhps2fpga"

#define SYSFS_LWH2F_BRIDGE_STATE_PATH "/sys/class/fpga_bridge/br0/state"
#define SYSFS_LWH2F_BRIDGE_STATE "enabled"

#define SYSFS_H2F_BRIDGE_NAME_PATH "/sys/class/fpga_bridge/br1/name"
#define SYSFS_H2F_BRIDGE_NAME "hps2fpga"

#define SYSFS_H2F_BRIDGE_STATE_PATH "/sys/class/fpga_bridge/br1/state"
#define SYSFS_H2F_BRIDGE_STATE "enabled"

#define LED_DELAY_US 250000

void validate_system_status(void);
void demo_sysid(void);
void demo_ocram(void);
void demo_leds(void);
void demo_switches(void);
void demo_buttons(void);

int main(int argc, char * const argv[]) {

	int i;
	int result;
	struct termios termios_s;
	struct termios original_termios;
	struct utsname uname_s;

	/*
	 * configure stdio as non-canonical with no echo so we can poll for key
	 * input as they occur rather than line by line and prevent echo back
	 * on the input terminal
	 */

	result = tcgetattr(STDIN_FILENO, &termios_s);
	if(result != 0)
		error(1, errno, "tcgetattr");

	original_termios = termios_s;
	termios_s.c_lflag &= ~ICANON;
	termios_s.c_lflag &= ~ECHO;
	result = tcsetattr(STDIN_FILENO, TCSANOW, &termios_s);
	if(result != 0)
		error(1, errno, "tcsetattr");

	/* configure stdin as non-blocking */
	fcntl(STDIN_FILENO, F_SETFL, fcntl(STDIN_FILENO, F_GETFL) | O_NONBLOCK);

	/* Display welcome message */
	printf("\n");
	printf("Blink application started.\n");
	printf("\n");

	/* Display command line parameters */
	printf("Number of command line parameters: %d\n", argc);
	for(i = 0 ; i < argc ; i++)
		printf("Command line parameter %d = %s\n", (i + 1), argv[i]);

	printf("\n");

	/* Display Linux version information */
	result = uname(&uname_s);
	if(result != 0)
		error(1, errno, "uname");

	printf("'%s'\n'%s'\n'%s'\n'%s'\n", uname_s.sysname, uname_s.release
					,uname_s.version, uname_s.machine);
	printf("\n");

	/* Exercise FPGA IP */
	validate_system_status();
	demo_sysid();
	demo_ocram();
	demo_leds();
	demo_switches();
	demo_buttons();

	/* Goodbye message */
	printf("Blink Application completed.\n");
	printf("\n");

	/* restore the original termios */
	result = tcsetattr(STDIN_FILENO, TCSANOW, &original_termios);
	if(result != 0)
		error(1, errno, "tcsetattr");

	return(0);
}

void validate_system_status(void) {

	int result;
	int sysfs_fd;
	char sysfs_str[256];

	/* validate the FPGA state */
	sysfs_fd = open(SYSFS_FPGA0_STATE_PATH, O_RDONLY);
	if(sysfs_fd < 0)
		error(1, errno, "open sysfs FPGA state");
	result = read(sysfs_fd, sysfs_str, strlen(SYSFS_FPGA0_STATE));
	if(result < 0)
		error(1, errno, "read sysfs FPGA state");
	close(sysfs_fd);
	if(strncmp(SYSFS_FPGA0_STATE, sysfs_str, strlen(SYSFS_FPGA0_STATE)))
		error(1, 0, "FPGA not in operate state");

	/* validate the LWH2F bridge name */
	sysfs_fd = open(SYSFS_LWH2F_BRIDGE_NAME_PATH, O_RDONLY);
	if(sysfs_fd < 0)
		error(1, errno, "open sysfs LWH2F bridge name");
	result = read(sysfs_fd, sysfs_str, strlen(SYSFS_LWH2F_BRIDGE_NAME));
	if(result < 0)
		error(1, errno, "read sysfs LWH2F bridge name");
	close(sysfs_fd);
	if(strncmp(SYSFS_LWH2F_BRIDGE_NAME, sysfs_str,
				strlen(SYSFS_LWH2F_BRIDGE_NAME)))
		error(1, 0, "bad LWH2F bridge name");

	/* validate the LWH2F bridge state */
	sysfs_fd = open(SYSFS_LWH2F_BRIDGE_STATE_PATH, O_RDONLY);
	if(sysfs_fd < 0)
		error(1, errno, "open sysfs LWH2F bridge state");
	result = read(sysfs_fd, sysfs_str, strlen(SYSFS_LWH2F_BRIDGE_STATE));
	if(result < 0)
		error(1, errno, "read sysfs LWH2F bridge state");
	close(sysfs_fd);
	if(strncmp(SYSFS_LWH2F_BRIDGE_STATE, sysfs_str,
				strlen(SYSFS_LWH2F_BRIDGE_STATE)))
		error(1, 0, "LWH2F bridge not enabled");

	/* validate the H2F bridge name */
	sysfs_fd = open(SYSFS_H2F_BRIDGE_NAME_PATH, O_RDONLY);
	if(sysfs_fd < 0)
		error(1, errno, "open sysfs H2F bridge name");
	result = read(sysfs_fd, sysfs_str, strlen(SYSFS_H2F_BRIDGE_NAME));
	if(result < 0)
		error(1, errno, "read sysfs H2F bridge name");
	close(sysfs_fd);
	if(strncmp(SYSFS_H2F_BRIDGE_NAME, sysfs_str,
				strlen(SYSFS_H2F_BRIDGE_NAME)))
		error(1, 0, "bad H2F bridge name");

	/* validate the H2F bridge state */
	sysfs_fd = open(SYSFS_H2F_BRIDGE_STATE_PATH, O_RDONLY);
	if(sysfs_fd < 0)
		error(1, errno, "open sysfs H2F bridge state");
	result = read(sysfs_fd, sysfs_str, strlen(SYSFS_H2F_BRIDGE_STATE));
	if(result < 0)
		error(1, errno, "read sysfs H2F bridge state");
	close(sysfs_fd);
	if(strncmp(SYSFS_H2F_BRIDGE_STATE, sysfs_str,
				strlen(SYSFS_H2F_BRIDGE_STATE)))
		error(1, 0, "H2F bridge not enabled");

	printf("FPGA appears to be configured and bridges are not in reset\n");
	printf("\n");
}

void demo_sysid(void) {

	int result;
	int dev_mem_fd;
	void *mmap_addr;
	size_t mmap_length;
	int mmap_prot;
	int mmap_flags;
	int mmap_fd;
	off_t mmap_offset;
	void *mmap_ptr;
	volatile u_int *system_id_ptr;
	u_int32_t system_ID_value;
	u_int32_t system_TS_value;

	/* map the peripheral span through /dev/mem */
	dev_mem_fd = open("/dev/mem", O_RDWR | O_SYNC);
	if(dev_mem_fd < 0)
		error(1, errno, "open /dev/mem");

	mmap_addr = NULL;
	mmap_length = SYSTEM_ID_SPAN;
	mmap_prot = PROT_READ;
	mmap_flags = MAP_SHARED;
	mmap_fd = dev_mem_fd;
	mmap_offset = SYSTEM_ID_BASE & ~(sysconf(_SC_PAGE_SIZE) - 1);
	mmap_ptr = mmap(mmap_addr, mmap_length, mmap_prot, mmap_flags,
							mmap_fd, mmap_offset);
	if(mmap_ptr == MAP_FAILED)
		error(1, errno, "mmap /dev/mem");

	system_id_ptr = (u_int*)((u_int)mmap_ptr +
			(SYSTEM_ID_BASE & (sysconf(_SC_PAGE_SIZE) - 1)));

	/* read the system ID values */
	system_ID_value = system_id_ptr[0];
	system_TS_value = system_id_ptr[1];

	printf("System ID ID: 0x%08X\n", system_ID_value);
	printf("System ID TS: 0x%08X\n", system_TS_value);

	/* unmap /dev/mem mappings */
	result = munmap(mmap_ptr, mmap_length);
	if(result < 0)
		error(1, errno, "munmap /dev/mem");

	close(dev_mem_fd);
	printf("\n");
}

void demo_ocram(void) {

	int result;
	int dev_mem_fd;
	void *mmap_addr;
	size_t mmap_length;
	int mmap_prot;
	int mmap_flags;
	int mmap_fd;
	off_t mmap_offset;
	void *mmap_ptr;
	volatile u_int *ocram_64k_ptr;
	void *original_ocram_64k_content_ptr;
	int i;

	/* map the peripheral span through /dev/mem */
	dev_mem_fd = open("/dev/mem", O_RDWR | O_SYNC);
	if(dev_mem_fd < 0)
		error(1, errno, "open /dev/mem");

	mmap_addr = NULL;
	mmap_length = OCRAM_64K_SPAN;
	mmap_prot = PROT_READ | PROT_WRITE;
	mmap_flags = MAP_SHARED;
	mmap_fd = dev_mem_fd;
	mmap_offset = OCRAM_64K_BASE & ~(sysconf(_SC_PAGE_SIZE) - 1);
	mmap_ptr = mmap(mmap_addr, mmap_length, mmap_prot, mmap_flags,
							mmap_fd, mmap_offset);
	if(mmap_ptr == MAP_FAILED)
		error(1, errno, "mmap /dev/mem");

	ocram_64k_ptr = (u_int*)((u_int)mmap_ptr +
			(OCRAM_64K_BASE & (sysconf(_SC_PAGE_SIZE) - 1)));

	/* save current ocram 64k contents */
	original_ocram_64k_content_ptr = malloc(OCRAM_64K_SPAN);
	if(original_ocram_64k_content_ptr == NULL)
		error(1, errno, "malloc");

	memcpy(original_ocram_64k_content_ptr, (const void *)ocram_64k_ptr,
								OCRAM_64K_SPAN);
	result = memcmp(original_ocram_64k_content_ptr,
				(const void *)ocram_64k_ptr, OCRAM_64K_SPAN);
	if(result != 0)
		error(1, errno, "memcmp original copy");

	printf("Saved initial ocram 64k values\n");

	/* test ocram 64k */
	printf("Writing sequential word values to ocram 64k\n");
	for(i = 0 ; i < (OCRAM_64K_SPAN / 4) ; i++) {
		ocram_64k_ptr[i] = i;
	}

	printf("Verifying sequential word values in ocram 64k\n");
	for(i = 0 ; i < (OCRAM_64K_SPAN / 4) ; i++) {
		if(ocram_64k_ptr[i] != (u_int)(i)) {
			printf("mismatch at word %d\n", i);
			printf("expected 0x%08X\n", i);
			printf("got 0x%08X\n", ocram_64k_ptr[i]);
		}
	}

	printf("Writing complimented sequential word values to ocram 64k\n");
	for(i = 0 ; i < (OCRAM_64K_SPAN / 4) ; i++) {
		ocram_64k_ptr[i] = ~i;
	}

	printf("Verifying complimented sequential word values in ocram 64k\n");
	for(i = 0 ; i < (OCRAM_64K_SPAN / 4) ; i++) {
		if(ocram_64k_ptr[i] != ~(u_int)(i)) {
			printf("mismatch at word %d\n", i);
			printf("expected 0x%08X\n", ~i);
			printf("got 0x%08X\n", ocram_64k_ptr[i]);
		}
	}

	/* restore ocram 64k contents */
	memcpy((void *)ocram_64k_ptr, original_ocram_64k_content_ptr,
								OCRAM_64K_SPAN);
	result = memcmp((const void *)ocram_64k_ptr,
				original_ocram_64k_content_ptr, OCRAM_64K_SPAN);
	if(result != 0)
		error(1, errno, "memcmp restore copy");

	printf("Restored initial ocram 64k values\n");

	/* free the malloc buffer */
	free(original_ocram_64k_content_ptr);

	/* unmap /dev/mem mappings */
	result = munmap(mmap_ptr, mmap_length);
	if(result < 0)
		error(1, errno, "munmap /dev/mem");

	close(dev_mem_fd);
	printf("\n");
}

void demo_leds(void) {

	int result;
	int dev_mem_fd;
	void *mmap_addr;
	size_t mmap_length;
	int mmap_prot;
	int mmap_flags;
	int mmap_fd;
	off_t mmap_offset;
	void *mmap_ptr;
	volatile u_int *led_pio_ptr;
	u_int32_t led_pio_value;
	u_int32_t led = 0;

	/* map the peripheral span through /dev/mem */
	dev_mem_fd = open("/dev/mem", O_RDWR | O_SYNC);
	if(dev_mem_fd < 0)
		error(1, errno, "open /dev/mem");

	mmap_addr = NULL;
	mmap_length = LED_PIO_SPAN;
	mmap_prot = PROT_READ | PROT_WRITE;
	mmap_flags = MAP_SHARED;
	mmap_fd = dev_mem_fd;
	mmap_offset = LED_PIO_BASE & ~(sysconf(_SC_PAGE_SIZE) - 1);
	mmap_ptr = mmap(mmap_addr, mmap_length, mmap_prot, mmap_flags,
							mmap_fd, mmap_offset);
	if(mmap_ptr == MAP_FAILED)
		error(1, errno, "mmap /dev/mem");

	led_pio_ptr = (u_int*)((u_int)mmap_ptr +
			(LED_PIO_BASE & (sysconf(_SC_PAGE_SIZE) - 1)));

	/* read the LED PIO value */
	led_pio_value = led_pio_ptr[0];

	/* Blink the LEDs until any key is pressed */
	printf("Blinking LEDs\n");
	printf("Press any key to stop LED lightshow\n");

	while(getc(stdin) == -1) {

		/* Turn on LED */
		led_pio_ptr[0] = 1L << led;

		/* Delay */
		usleep(LED_DELAY_US);

		/* Advance to next LED */
		led = (led + 1) % LED_PIO_DATA_WIDTH;
	}

	/* restore the LED PIO value */
	led_pio_ptr[0] = led_pio_value;

	/* unmap /dev/mem mappings */
	result = munmap(mmap_ptr, mmap_length);
	if(result < 0)
		error(1, errno, "munmap /dev/mem");

	close(dev_mem_fd);
	printf("\n");
}

void demo_switches(void) {

	int result;
	int dev_mem_fd;
	void *mmap_addr;
	size_t mmap_length;
	int mmap_prot;
	int mmap_flags;
	int mmap_fd;
	off_t mmap_offset;
	void *mmap_ptr;
	volatile u_int *switch_pio_ptr;
	u_int32_t prev_switches;
	u_int32_t switches;

	/* map the peripheral span through /dev/mem */
	dev_mem_fd = open("/dev/mem", O_RDWR | O_SYNC);
	if(dev_mem_fd < 0)
		error(1, errno, "open /dev/mem");

	mmap_addr = NULL;
	mmap_length = SWITCH_PIO_SPAN;
	mmap_prot = PROT_READ;
	mmap_flags = MAP_SHARED;
	mmap_fd = dev_mem_fd;
	mmap_offset = SWITCH_PIO_BASE & ~(sysconf(_SC_PAGE_SIZE) - 1);
	mmap_ptr = mmap(mmap_addr, mmap_length, mmap_prot, mmap_flags,
							mmap_fd, mmap_offset);
	if(mmap_ptr == MAP_FAILED)
		error(1, errno, "mmap /dev/mem");

	switch_pio_ptr = (u_int*)((u_int)mmap_ptr +
			(SWITCH_PIO_BASE & (sysconf(_SC_PAGE_SIZE) - 1)));

	/* Display state of switches */
	printf("Displaying the state of switches\n");
	printf("Slide switch SW0, Sw1, SW2 or SW3 to observe updates\n");
	printf("Press any key to stop displaying the switches\n");
	prev_switches = ~switch_pio_ptr[0];

	while(getc(stdin) == -1) {
		switches = switch_pio_ptr[0];
		if(switches != prev_switches) {
			printf("Switch value: 0x%01x\n", (int)switches);
			prev_switches = switches;
		}
	}

	/* unmap /dev/mem mappings */
	result = munmap(mmap_ptr, mmap_length);
	if(result < 0)
		error(1, errno, "munmap /dev/mem");

	close(dev_mem_fd);
	printf("\n");
}

void demo_buttons(void) {

	int result;
	int dev_mem_fd;
	void *mmap_addr;
	size_t mmap_length;
	int mmap_prot;
	int mmap_flags;
	int mmap_fd;
	off_t mmap_offset;
	void *mmap_ptr;
	volatile u_int *button_pio_ptr;
	u_int32_t prev_buttons;
	u_int32_t buttons;

	/* map the peripheral span through /dev/mem */
	dev_mem_fd = open("/dev/mem", O_RDWR | O_SYNC);
	if(dev_mem_fd < 0)
		error(1, errno, "open /dev/mem");

	mmap_addr = NULL;
	mmap_length = BUTTON_PIO_SPAN;
	mmap_prot = PROT_READ;
	mmap_flags = MAP_SHARED;
	mmap_fd = dev_mem_fd;
	mmap_offset = BUTTON_PIO_BASE & ~(sysconf(_SC_PAGE_SIZE) - 1);
	mmap_ptr = mmap(mmap_addr, mmap_length, mmap_prot, mmap_flags,
							mmap_fd, mmap_offset);
	if(mmap_ptr == MAP_FAILED)
		error(1, errno, "mmap /dev/mem");

	button_pio_ptr = (u_int*)((u_int)mmap_ptr +
			(BUTTON_PIO_BASE & (sysconf(_SC_PAGE_SIZE) - 1)));

	/* Display state of buttons */
	printf("Displaying the state of buttons\n");
	printf("Press push button KEY1 to observe updates\n");
	printf("Press any key to stop displaying the buttons\n");
	prev_buttons = ~button_pio_ptr[0];

	while(getc(stdin) == -1) {
		buttons = button_pio_ptr[0];
		if(buttons != prev_buttons) {
			printf("Button value: 0x%01x\n", (int)buttons);
			prev_buttons = buttons;
		}
	}

	/* unmap /dev/mem mappings */
	result = munmap(mmap_ptr, mmap_length);
	if(result < 0)
		error(1, errno, "munmap /dev/mem");

	close(dev_mem_fd);
	printf("\n");
}

