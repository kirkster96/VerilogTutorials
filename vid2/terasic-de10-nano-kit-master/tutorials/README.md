# tutorials directory

This directory contains the source content used to create the following six tutorials that are designed to run on the Terasic DE10-Nano development kit by Terasic Technologies Inc.  The PDF files for these tutorials can be downloaded from the GitHub* release download page [here][latest-release-download-page].

---
**How to Program Your First FPGA Device**

This tutorial demonstrates how to create the hardware equivalent of "Hello World": a blinking LED. This is a simple exercise to get you started using the Intel&reg; Quartus&reg; software for FPGA development.

You'll learn to compile Verilog code, make pin assignments, create timing constraints, and then program the FPGA to blink one of the eight green user LEDs on the board. You'll use a 50 MHz clock input (from the on-board oscillator) to drive a counter, and assign an LED to one of the counter output bits.

Download the tutorial PDF file *writeup_MyFirstFPGA.pdf* [here][latest-release-download-page].

---
**My First Qsys System**

This tutorial demonstrates how to use the Qsys system integration tool to easily create an FPGA design using IP available in the Intel&reg Quartus&reg software IP catalog. Qsys speeds embedded system design by standardizing the interconnect between IP blocks and allowing users to create their own IP blocks for reuse in their systems.

Download the tutorial PDF file *writeup_MyFirstQsysSystem.pdf* [here][latest-release-download-page].

---
**Interacting with FPGA Designs using System Console**

This tutorial demonstrates how to use the System Console debugging tool to program a compiled FPGA design into an FPGA device, then access the hardware modules instantiated in that FPGA design. This System Console tutorial is based on the FPGA design created in the "My First Qsys System" tutorial.

In this tutorial we will demonstrate how you can interact with the Qsys system through a JTAG cable connected to the FPGA that can send read and write transactions through the JTAG master in the Qsys system to interact with the slave peripherals it is connected to.

Download the tutorial PDF file *writeup_InteractSystemConsole.pdf* [here][latest-release-download-page].

---
**My First HPS System**

This tutorial demonstrates how to instantiate and configure a Hard Processor System (HPS) component into a Qsys system. Unlike other components in the prior "My First Qsys System" tutorial, the HPS component does not define any soft logic to be configured in the FPGA, rather, it allows the designer to configure the interfaces between the existing processor hardware on the Intel&reg SoC FPGA chip. Configuring an entire processor system is a necessarily detailed process, but the level of customization available is precisely the advantage an Intel&reg SoC FPGA provides. This tutorial shows as closely as possible how to set up the HPS in a similar manner to the design that shipped with the Terasic DE10-Nano board, and integrate it with the existing Qsys system from the preceding "My First Qsys System" tutorial. This will allow us to integrate this example onto the existing SD card image that ships with the Terasic DE10-Nano board.

Download the tutorial PDF file *writeup_MyFirstHPSSystem.pdf* [here][latest-release-download-page].

---
**Interacting with FPGA Designs using U-Boot**

This tutorial demonstrates how to use the U-Boot boot loader to program a compiled FPGA design into an Intel&reg SoC FPGA device, then interact with the hardware modules instantiated in that FPGA design from the HPS processor using U-Boot.  This U-Boot tutorial is based on the FPGA design created in a prior tutorial "My First HPS System".

In this tutorial we will demonstrate how you can interact with the Qsys system in the FPGA through the HPS-to-FPGA bridges provided on the HPS core, reading and writing to the slave peripherals they are connected to. Both U-Boot console commands and a U-Boot standalone application are used to interact with the soft IP peripherals residing in the FPGA fabric.

Download the tutorial PDF file *writeup_u-boot.pdf* [here][latest-release-download-page].

---
**Interacting with FPGA Designs using Linux***

This tutorial demonstrates how to use various Linux* capabilities to interact with the hardware modules instantiated in an FPGA design from the HPS processor. This Linux tutorial is based on the FPGA design created in a prior tutorial "My First HPS System".

In this tutorial we will demonstrate how you can interact with the Qsys system in the FPGA through the HPS-to-FPGA bridges provided on the HPS core, reading and writing to the slave peripherals they are connected to. We demonstrate the *devmem* and *devmem2* programs that can peek and poke at the FPGA peripherals. We also demonstrate how to build a Linux application to interact with those same soft IP peripherals. Finally we demonstrate how to use a number of sysfs capabilities provided by existing device drivers for the FPGA based peripherals which we will enable with a *device tree overlay*.

Download the tutorial PDF file *writeup_linux.pdf* [here][latest-release-download-page].

---
Intel and Quartus are trademarks of Intel Corporation or its subsidiaries in the U.S. and/or other countries.

Other names and brands may be claimed as the property of others.

[latest-release-download-page]:https://github.com/intel-iot-devkit/terasic-de10-nano-kit/releases/latest

