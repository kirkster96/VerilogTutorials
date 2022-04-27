#
# Copyright (c) 2017 Intel Corporation
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to
# deal in the Software without restriction, including without limitation the
# rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
# sell copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
# IN THE SOFTWARE.
#

# make the pin location assignments for the HPS, we only need the DRAM interface
# pins, not the HPS peripheral IO pins
set_location_assignment PIN_C28  -to hps_memory_mem_a[0]
set_location_assignment PIN_B28  -to hps_memory_mem_a[1]
set_location_assignment PIN_E26  -to hps_memory_mem_a[2]
set_location_assignment PIN_D26  -to hps_memory_mem_a[3]
set_location_assignment PIN_J21  -to hps_memory_mem_a[4]
set_location_assignment PIN_J20  -to hps_memory_mem_a[5]
set_location_assignment PIN_C26  -to hps_memory_mem_a[6]
set_location_assignment PIN_B26  -to hps_memory_mem_a[7]
set_location_assignment PIN_F26  -to hps_memory_mem_a[8]
set_location_assignment PIN_F25  -to hps_memory_mem_a[9]
set_location_assignment PIN_A24  -to hps_memory_mem_a[10]
set_location_assignment PIN_B24  -to hps_memory_mem_a[11]
set_location_assignment PIN_D24  -to hps_memory_mem_a[12]
set_location_assignment PIN_C24  -to hps_memory_mem_a[13]
set_location_assignment PIN_G23  -to hps_memory_mem_a[14]
set_location_assignment PIN_A27  -to hps_memory_mem_ba[0]
set_location_assignment PIN_H25  -to hps_memory_mem_ba[1]
set_location_assignment PIN_G25  -to hps_memory_mem_ba[2]
set_location_assignment PIN_A26  -to hps_memory_mem_cas_n
set_location_assignment PIN_N21  -to hps_memory_mem_ck
set_location_assignment PIN_N20  -to hps_memory_mem_ck_n
set_location_assignment PIN_L28  -to hps_memory_mem_cke
set_location_assignment PIN_L21  -to hps_memory_mem_cs_n
set_location_assignment PIN_G28  -to hps_memory_mem_dm[0]
set_location_assignment PIN_P28  -to hps_memory_mem_dm[1]
set_location_assignment PIN_W28  -to hps_memory_mem_dm[2]
set_location_assignment PIN_AB28 -to hps_memory_mem_dm[3]
set_location_assignment PIN_J25  -to hps_memory_mem_dq[0]
set_location_assignment PIN_J24  -to hps_memory_mem_dq[1]
set_location_assignment PIN_E28  -to hps_memory_mem_dq[2]
set_location_assignment PIN_D27  -to hps_memory_mem_dq[3]
set_location_assignment PIN_J26  -to hps_memory_mem_dq[4]
set_location_assignment PIN_K26  -to hps_memory_mem_dq[5]
set_location_assignment PIN_G27  -to hps_memory_mem_dq[6]
set_location_assignment PIN_F28  -to hps_memory_mem_dq[7]
set_location_assignment PIN_K25  -to hps_memory_mem_dq[8]
set_location_assignment PIN_L25  -to hps_memory_mem_dq[9]
set_location_assignment PIN_J27  -to hps_memory_mem_dq[10]
set_location_assignment PIN_J28  -to hps_memory_mem_dq[11]
set_location_assignment PIN_M27  -to hps_memory_mem_dq[12]
set_location_assignment PIN_M26  -to hps_memory_mem_dq[13]
set_location_assignment PIN_M28  -to hps_memory_mem_dq[14]
set_location_assignment PIN_N28  -to hps_memory_mem_dq[15]
set_location_assignment PIN_N24  -to hps_memory_mem_dq[16]
set_location_assignment PIN_N25  -to hps_memory_mem_dq[17]
set_location_assignment PIN_T28  -to hps_memory_mem_dq[18]
set_location_assignment PIN_U28  -to hps_memory_mem_dq[19]
set_location_assignment PIN_N26  -to hps_memory_mem_dq[20]
set_location_assignment PIN_N27  -to hps_memory_mem_dq[21]
set_location_assignment PIN_R27  -to hps_memory_mem_dq[22]
set_location_assignment PIN_V27  -to hps_memory_mem_dq[23]
set_location_assignment PIN_R26  -to hps_memory_mem_dq[24]
set_location_assignment PIN_R25  -to hps_memory_mem_dq[25]
set_location_assignment PIN_AA28 -to hps_memory_mem_dq[26]
set_location_assignment PIN_W26  -to hps_memory_mem_dq[27]
set_location_assignment PIN_R24  -to hps_memory_mem_dq[28]
set_location_assignment PIN_T24  -to hps_memory_mem_dq[29]
set_location_assignment PIN_Y27  -to hps_memory_mem_dq[30]
set_location_assignment PIN_AA27 -to hps_memory_mem_dq[31]
set_location_assignment PIN_R17  -to hps_memory_mem_dqs[0]
set_location_assignment PIN_R19  -to hps_memory_mem_dqs[1]
set_location_assignment PIN_T19  -to hps_memory_mem_dqs[2]
set_location_assignment PIN_U19  -to hps_memory_mem_dqs[3]
set_location_assignment PIN_R16  -to hps_memory_mem_dqs_n[0]
set_location_assignment PIN_R18  -to hps_memory_mem_dqs_n[1]
set_location_assignment PIN_T18  -to hps_memory_mem_dqs_n[2]
set_location_assignment PIN_T20  -to hps_memory_mem_dqs_n[3]
set_location_assignment PIN_D28  -to hps_memory_mem_odt
set_location_assignment PIN_A25  -to hps_memory_mem_ras_n
set_location_assignment PIN_V28  -to hps_memory_mem_reset_n
set_location_assignment PIN_E25  -to hps_memory_mem_we_n
set_location_assignment PIN_D25  -to hps_memory_oct_rzqin

# define the IO standard for the HPS peripheral IO pins
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_io_hps_io_emac1_inst_MDC
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_io_hps_io_emac1_inst_MDIO
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_io_hps_io_emac1_inst_RXD0
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_io_hps_io_emac1_inst_RXD1
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_io_hps_io_emac1_inst_RXD2
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_io_hps_io_emac1_inst_RXD3
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_io_hps_io_emac1_inst_RX_CLK
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_io_hps_io_emac1_inst_RX_CTL
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_io_hps_io_emac1_inst_TXD0
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_io_hps_io_emac1_inst_TXD3
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_io_hps_io_emac1_inst_TXD1
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_io_hps_io_emac1_inst_TXD2
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_io_hps_io_emac1_inst_TX_CLK
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_io_hps_io_emac1_inst_TX_CTL
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_io_hps_io_sdio_inst_CLK
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_io_hps_io_sdio_inst_CMD
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_io_hps_io_sdio_inst_D0
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_io_hps_io_sdio_inst_D1
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_io_hps_io_sdio_inst_D2
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_io_hps_io_sdio_inst_D3
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_io_hps_io_uart0_inst_RX
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_io_hps_io_uart0_inst_TX
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_io_hps_io_usb1_inst_CLK
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_io_hps_io_usb1_inst_D0
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_io_hps_io_usb1_inst_D1
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_io_hps_io_usb1_inst_D2
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_io_hps_io_usb1_inst_D3
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_io_hps_io_usb1_inst_D4
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_io_hps_io_usb1_inst_D5
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_io_hps_io_usb1_inst_D6
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_io_hps_io_usb1_inst_D7
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_io_hps_io_usb1_inst_DIR
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_io_hps_io_usb1_inst_NXT
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_io_hps_io_usb1_inst_STP
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_io_hps_io_spim1_inst_CLK
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_io_hps_io_spim1_inst_MOSI
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_io_hps_io_spim1_inst_MISO
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_io_hps_io_spim1_inst_SS0
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_io_hps_io_i2c0_inst_SDA
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_io_hps_io_i2c0_inst_SCL
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_io_hps_io_i2c1_inst_SDA
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_io_hps_io_i2c1_inst_SCL
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_io_hps_io_gpio_inst_GPIO09
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_io_hps_io_gpio_inst_GPIO35
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_io_hps_io_gpio_inst_GPIO40
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_io_hps_io_gpio_inst_GPIO53
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_io_hps_io_gpio_inst_GPIO54
set_instance_assignment -name IO_STANDARD "3.3-V LVTTL" -to hps_io_hps_io_gpio_inst_GPIO61

# define the current strength for the HPS peripheral IO pins
set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to hps_io_hps_io_emac1_inst_MDC
set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to hps_io_hps_io_emac1_inst_MDIO
set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to hps_io_hps_io_emac1_inst_RXD0
set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to hps_io_hps_io_emac1_inst_RXD1
set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to hps_io_hps_io_emac1_inst_RXD2
set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to hps_io_hps_io_emac1_inst_RXD3
set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to hps_io_hps_io_emac1_inst_RX_CLK
set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to hps_io_hps_io_emac1_inst_RX_CTL
set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to hps_io_hps_io_emac1_inst_TXD0
set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to hps_io_hps_io_emac1_inst_TXD1
set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to hps_io_hps_io_emac1_inst_TXD2
set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to hps_io_hps_io_emac1_inst_TXD3
set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to hps_io_hps_io_emac1_inst_TX_CLK
set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to hps_io_hps_io_emac1_inst_TX_CTL
set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to hps_io_hps_io_usb1_inst_CLK
set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to hps_io_hps_io_usb1_inst_D0
set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to hps_io_hps_io_usb1_inst_D1
set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to hps_io_hps_io_usb1_inst_D2
set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to hps_io_hps_io_usb1_inst_D3
set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to hps_io_hps_io_usb1_inst_D4
set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to hps_io_hps_io_usb1_inst_D5
set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to hps_io_hps_io_usb1_inst_D6
set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to hps_io_hps_io_usb1_inst_D7
set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to hps_io_hps_io_usb1_inst_DIR
set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to hps_io_hps_io_usb1_inst_NXT
set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to hps_io_hps_io_usb1_inst_STP
set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to hps_io_hps_io_sdio_inst_CLK
set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to hps_io_hps_io_sdio_inst_CMD
set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to hps_io_hps_io_sdio_inst_D0
set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to hps_io_hps_io_sdio_inst_D1
set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to hps_io_hps_io_sdio_inst_D2
set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to hps_io_hps_io_sdio_inst_D3
set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to hps_io_hps_io_uart0_inst_RX
set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to hps_io_hps_io_uart0_inst_TX
set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to hps_io_hps_io_spim1_inst_CLK
set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to hps_io_hps_io_spim1_inst_MOSI
set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to hps_io_hps_io_spim1_inst_SS0
set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to hps_io_hps_io_i2c0_inst_SDA
set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to hps_io_hps_io_i2c0_inst_SCL
set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to hps_io_hps_io_i2c1_inst_SDA
set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to hps_io_hps_io_i2c1_inst_SCL
set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to hps_io_hps_io_gpio_inst_GPIO09
set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to hps_io_hps_io_gpio_inst_GPIO35
set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to hps_io_hps_io_gpio_inst_GPIO40
set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to hps_io_hps_io_gpio_inst_GPIO53
set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to hps_io_hps_io_gpio_inst_GPIO54
set_instance_assignment -name CURRENT_STRENGTH_NEW 4MA -to hps_io_hps_io_gpio_inst_GPIO61

# define the slew rate for the HPS peripheral IO pins and DRAM interface pins
set_instance_assignment -name SLEW_RATE 1 -to hps_memory_mem_a[0]
set_instance_assignment -name SLEW_RATE 1 -to hps_memory_mem_a[1]
set_instance_assignment -name SLEW_RATE 1 -to hps_memory_mem_a[2]
set_instance_assignment -name SLEW_RATE 1 -to hps_memory_mem_a[3]
set_instance_assignment -name SLEW_RATE 1 -to hps_memory_mem_a[4]
set_instance_assignment -name SLEW_RATE 1 -to hps_memory_mem_a[5]
set_instance_assignment -name SLEW_RATE 1 -to hps_memory_mem_a[6]
set_instance_assignment -name SLEW_RATE 1 -to hps_memory_mem_a[7]
set_instance_assignment -name SLEW_RATE 1 -to hps_memory_mem_a[8]
set_instance_assignment -name SLEW_RATE 1 -to hps_memory_mem_a[9]
set_instance_assignment -name SLEW_RATE 1 -to hps_memory_mem_a[10]
set_instance_assignment -name SLEW_RATE 1 -to hps_memory_mem_a[11]
set_instance_assignment -name SLEW_RATE 1 -to hps_memory_mem_a[12]
set_instance_assignment -name SLEW_RATE 1 -to hps_memory_mem_a[13]
set_instance_assignment -name SLEW_RATE 1 -to hps_memory_mem_a[14]
set_instance_assignment -name SLEW_RATE 1 -to hps_memory_mem_ba[0]
set_instance_assignment -name SLEW_RATE 1 -to hps_memory_mem_ba[1]
set_instance_assignment -name SLEW_RATE 1 -to hps_memory_mem_ba[2]
set_instance_assignment -name SLEW_RATE 1 -to hps_memory_mem_cas_n
set_instance_assignment -name SLEW_RATE 1 -to hps_memory_mem_cke
set_instance_assignment -name SLEW_RATE 1 -to hps_memory_mem_cs_n
set_instance_assignment -name SLEW_RATE 1 -to hps_memory_mem_odt
set_instance_assignment -name SLEW_RATE 1 -to hps_memory_mem_ras_n
set_instance_assignment -name SLEW_RATE 1 -to hps_memory_mem_reset_n
set_instance_assignment -name SLEW_RATE 1 -to hps_memory_mem_we_n
set_instance_assignment -name SLEW_RATE 1 -to hps_io_hps_io_emac1_inst_MDC
set_instance_assignment -name SLEW_RATE 1 -to hps_io_hps_io_emac1_inst_MDIO
set_instance_assignment -name SLEW_RATE 1 -to hps_io_hps_io_emac1_inst_RXD0
set_instance_assignment -name SLEW_RATE 1 -to hps_io_hps_io_emac1_inst_RXD1
set_instance_assignment -name SLEW_RATE 1 -to hps_io_hps_io_emac1_inst_RXD2
set_instance_assignment -name SLEW_RATE 1 -to hps_io_hps_io_emac1_inst_RXD3
set_instance_assignment -name SLEW_RATE 1 -to hps_io_hps_io_emac1_inst_RX_CLK
set_instance_assignment -name SLEW_RATE 1 -to hps_io_hps_io_emac1_inst_RX_CTL
set_instance_assignment -name SLEW_RATE 1 -to hps_io_hps_io_emac1_inst_TXD0
set_instance_assignment -name SLEW_RATE 1 -to hps_io_hps_io_emac1_inst_TXD1
set_instance_assignment -name SLEW_RATE 1 -to hps_io_hps_io_emac1_inst_TXD2
set_instance_assignment -name SLEW_RATE 1 -to hps_io_hps_io_emac1_inst_TXD3
set_instance_assignment -name SLEW_RATE 1 -to hps_io_hps_io_emac1_inst_TX_CLK
set_instance_assignment -name SLEW_RATE 1 -to hps_io_hps_io_emac1_inst_TX_CTL
set_instance_assignment -name SLEW_RATE 1 -to hps_io_hps_io_usb1_inst_CLK
set_instance_assignment -name SLEW_RATE 1 -to hps_io_hps_io_usb1_inst_D0
set_instance_assignment -name SLEW_RATE 1 -to hps_io_hps_io_usb1_inst_D1
set_instance_assignment -name SLEW_RATE 1 -to hps_io_hps_io_usb1_inst_D2
set_instance_assignment -name SLEW_RATE 1 -to hps_io_hps_io_usb1_inst_D3
set_instance_assignment -name SLEW_RATE 1 -to hps_io_hps_io_usb1_inst_D4
set_instance_assignment -name SLEW_RATE 1 -to hps_io_hps_io_usb1_inst_D5
set_instance_assignment -name SLEW_RATE 1 -to hps_io_hps_io_usb1_inst_D6
set_instance_assignment -name SLEW_RATE 1 -to hps_io_hps_io_usb1_inst_D7
set_instance_assignment -name SLEW_RATE 1 -to hps_io_hps_io_usb1_inst_DIR
set_instance_assignment -name SLEW_RATE 1 -to hps_io_hps_io_usb1_inst_NXT
set_instance_assignment -name SLEW_RATE 1 -to hps_io_hps_io_usb1_inst_STP
set_instance_assignment -name SLEW_RATE 1 -to hps_io_hps_io_sdio_inst_CLK
set_instance_assignment -name SLEW_RATE 1 -to hps_io_hps_io_sdio_inst_CMD
set_instance_assignment -name SLEW_RATE 1 -to hps_io_hps_io_sdio_inst_D0
set_instance_assignment -name SLEW_RATE 1 -to hps_io_hps_io_sdio_inst_D1
set_instance_assignment -name SLEW_RATE 1 -to hps_io_hps_io_sdio_inst_D2
set_instance_assignment -name SLEW_RATE 1 -to hps_io_hps_io_sdio_inst_D3
set_instance_assignment -name SLEW_RATE 1 -to hps_io_hps_io_uart0_inst_RX
set_instance_assignment -name SLEW_RATE 1 -to hps_io_hps_io_uart0_inst_TX
set_instance_assignment -name SLEW_RATE 1 -to hps_io_hps_io_spim1_inst_CLK
set_instance_assignment -name SLEW_RATE 1 -to hps_io_hps_io_spim1_inst_MOSI
set_instance_assignment -name SLEW_RATE 1 -to hps_io_hps_io_spim1_inst_SS0
set_instance_assignment -name SLEW_RATE 1 -to hps_io_hps_io_i2c0_inst_SDA
set_instance_assignment -name SLEW_RATE 1 -to hps_io_hps_io_i2c0_inst_SCL
set_instance_assignment -name SLEW_RATE 1 -to hps_io_hps_io_i2c1_inst_SDA
set_instance_assignment -name SLEW_RATE 1 -to hps_io_hps_io_i2c1_inst_SCL
set_instance_assignment -name SLEW_RATE 1 -to hps_io_hps_io_gpio_inst_GPIO09
set_instance_assignment -name SLEW_RATE 1 -to hps_io_hps_io_gpio_inst_GPIO35
set_instance_assignment -name SLEW_RATE 1 -to hps_io_hps_io_gpio_inst_GPIO40
set_instance_assignment -name SLEW_RATE 1 -to hps_io_hps_io_gpio_inst_GPIO53
set_instance_assignment -name SLEW_RATE 1 -to hps_io_hps_io_gpio_inst_GPIO54
set_instance_assignment -name SLEW_RATE 1 -to hps_io_hps_io_gpio_inst_GPIO61

