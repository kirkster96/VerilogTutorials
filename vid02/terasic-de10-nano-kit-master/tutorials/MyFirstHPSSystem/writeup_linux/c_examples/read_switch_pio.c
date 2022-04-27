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
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <error.h>
#include <errno.h>
#include <string.h>
#include <sys/mman.h>

/* Expected System Environment */
#define SYSFS_FPGA0_STATE_PATH "/sys/class/fpga_manager/fpga0/state"
#define SYSFS_FPGA0_STATE "operating"

#define SYSFS_LWH2F_BRIDGE_NAME_PATH "/sys/class/fpga_bridge/br0/name"
#define SYSFS_LWH2F_BRIDGE_NAME "lwhps2fpga"

#define SYSFS_LWH2F_BRIDGE_STATE_PATH "/sys/class/fpga_bridge/br0/state"
#define SYSFS_LWH2F_BRIDGE_STATE "enabled"

#define SWITCH_PIO_BASE 0xff210020
#define SWITCH_PIO_SPAN 16

int main(void) {

	int result;
	int sysfs_fd;
	char sysfs_str[256];
	int dev_mem_fd;
	void *mmap_addr;
	size_t mmap_length;
	int mmap_prot;
	int mmap_flags;
	int mmap_fd;
	off_t mmap_offset;
	void *mmap_ptr;
	volatile u_int *switch_pio_ptr;
	u_int32_t switch_pio_value;

	printf("\nStart of program...\n\n");

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

	/* read the switch PIO value */
	switch_pio_value = switch_pio_ptr[0];

	printf("Switch PIO: 0x%08X\n", switch_pio_value);

	/* unmap /dev/mem mappings */
	result = munmap(mmap_ptr, mmap_length);
	if(result < 0)
		error(1, errno, "munmap /dev/mem");

	close(dev_mem_fd);

	printf("\nEnd of program...\n\n");
}

