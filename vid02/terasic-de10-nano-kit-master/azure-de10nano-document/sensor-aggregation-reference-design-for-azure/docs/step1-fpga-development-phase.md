# Step 1: FPGA Development Phase

## Goals

The goals for this step are :

- Validate FPGA Design
- Validate Nios® II Firmware
    - Reading data from RFS card
    - Reading expected thresholds value from HPS

## Step 1.1: Preparation

### Step 1.1.1: Summary of Tools and Resources

List of tools and required resources used in this step

- Installed on Development PC
  - Intel® Quartus® Prime Software Lite/Standard Edition version 20.1.1
  - Nios® II Embedded Design Suite(EDS)
    Follow [Getting Started](https://fpgasoftware.intel.com/static/quick_start_guide/quick_start_guide_20.1_en.pdf)
  - Git client (GUI or CLI)

- Files
  - Quartus® Project
  - Binary for Nios II Application (.elf, needed only in the case that FPGA design has Nios II IP)
  - FPGA bitstream(.sof/.rbf)
  - Preloader Binary(Optional)

### Step 1.1.2: Clone the Reference Application

Clone the reference source code from the GitHub repo.

```bash
  git clone https://github.com/intel-iot-devkit/terasic-de10-nano-kit.git
```

The reference FPGA design is in `<working directory root>/terasic-de10-nano-kit/azure-de10nano-document/sensor-aggregation-reference-design-for-azure/hw` folder.

## Step 1.2: Build Nios® II software

Within your terminal, launch the Nios® II Command Shell.
Change directories to \<Nios II EDS install path\>, and type the command nios2_command_shell.sh.

```
cd <Nios II EDS install path>
nios2_command_shell.sh
```
Output:
```
------------------------------------------------
Altera Nios2 Command Shell

Version 20.1, Build 720
------------------------------------------------

```
*Note: Nios® II EDS install path is default to within the Intel® Quartus Prime Lite Edition*

First, generate the top_qsys system under the Quartus® project folder.
This generates RTL files from the .qsys file in top_qsys folder and the .sopcinfo file which is needed for Nios® II software generation.

```
cd <working directory root>/hw/Module5_Sample_HW/
qsys-generate -syn top_qsys.qsys
```

Next, go to the Nios® II application software project folder.
Execute the compile commands below:

```
cd software_nios2/RFS_SENSOR
./create-this-app
make mem_init_generate
```
Output of running `./create-this-app`:
```
[RFS_SENSOR build complete]

To download and run the application:
    1. Make sure the board is connected to the system.
    2. Run 'nios2-configure-sof <SOF_FILE_PATH>' to configure the FPGA with the hardware design.
    3. If you have a stdio device, run 'nios2-terminal' in a different shell.
    4. Run 'make download-elf' from the application directory.

To debug the application:
    Import the project into Nios II Software Build Tools for Eclipse.
    Refer to Nios II Software Build Tools for Eclipse Documentation for more information.
```

Output of running `make mem_init_generate`:
```
Info: Building ../RFS_SENSOR_bsp/
make --no-print-directory -C ../RFS_SENSOR_bsp/
[BSP build complete]
Post-processing to create mem_init/top_qsys_nios_system_onchip_memory2.hex...
elf2hex RFS_SENSOR.elf 0x00020000 0x0003ffff --width=32 --little-endian-mem --create-lanes=0 mem_init/top_qsys_nios_system_onchip_memory2.hex
Post-processing to create mem_init/top_qsys_nios_system_shared_memory.hex...
elf2hex RFS_SENSOR.elf 0x00002000 0x00002fff --width=32 --little-endian-mem --create-lanes=0 mem_init/top_qsys_nios_system_shared_memory.hex
Post-processing to create mem_init/hdl_sim/top_qsys_nios_system_onchip_memory2.dat...
elf2dat --infile=RFS_SENSOR.elf --outfile=mem_init/hdl_sim/top_qsys_nios_system_onchip_memory2.dat \
        --base=0x00020000 --end=0x0003ffff --width=32 \
        --little-endian-mem --create-lanes=0 
Post-processing to create mem_init/hdl_sim/top_qsys_nios_system_shared_memory.dat...
elf2dat --infile=RFS_SENSOR.elf --outfile=mem_init/hdl_sim/top_qsys_nios_system_shared_memory.dat \
        --base=0x00002000 --end=0x00002fff --width=32 \
        --little-endian-mem --create-lanes=0 
Post-processing to create mem_init/hdl_sim/top_qsys_nios_system_onchip_memory2.sym...
nios2-elf-nm -n RFS_SENSOR.elf > mem_init/hdl_sim/top_qsys_nios_system_onchip_memory2.sym
Post-processing to create mem_init/hdl_sim/top_qsys_nios_system_shared_memory.sym...
nios2-elf-nm -n RFS_SENSOR.elf > mem_init/hdl_sim/top_qsys_nios_system_shared_memory.sym
Post-processing to create mem_init/meminit.spd...
Post-processing to create mem_init/meminit.qip...
```

By now, you should have RFS_SENSOR.elf file, which is a Nios® II binary, and a memory initialization file in mem_init folder, to be used later with Quartus® project compilation.  
Please ensure that you have these files with `ls *.elf` and `ls mem_init/*.hex` commands:

```
$ ls *.elf
RFS_SENSOR.elf
$ ls mem_init/*.hex
mem_init/top_qsys_nios_system_onchip_memory2.hex
mem_init/top_qsys_nios_system_shared_memory.hex
```

## Step 1.3: Compile FPGA design

To compile the FPGA project

```
cd <working directory root>/phase1-rfs/hw/Module5_Sample_HW/
quartus_sh --flow compile Module5_Sample_HW
```
*Note: This may take some time*

Once the complilation is done, you will have a SOF file in output_files folder. Please check with `ls output_files/*.sof`.

```
$ ls output_files/*.sof
output_files/Module5_Sample_HW.sof
```

## Step 1.4: Test FPGA design

To test the generated .sof file, we will load the .sof file and check Nios® II printf strings via JTAG(USB Blaster).

- Connect your Development PC with J13(Labeled as "USB BLASTER II") on DE10-Nano board with a USB-mini cable.

- Remove SD card in the SD card slot on DE10-Nano

- Connect RFS card with GPIO0 on DE10-Nano

- Power On DE10-Nano

- Type the JTAG command `jtagconfig` to check if JTAG connection is live and active. You shall see the following:

  ```
  $ jtagconfig
  1) DE-SoC [USB-1]
    4BA00477   SOCVHPS
    02D020DD   5CSEBA6(.|ES)/5CSEMA6/..
  ```

    *Possible **error**:*

    1. **Error\: jtagconfig\: command not found**<br>
        - Possible root cause: missing environment variable.<br>

        Solution
        ```
        export PATH=$PATH:~/intelFPGA_lite/20.1/quartus/bin
        ```
    2. **Error\: No JTAG hardware available**<br>
        - Possible root cause: Missing access right to JTAG hardware.<br>

        Solution

            Create the file /etc/udev/rules.d/51-usbblaster.rules with the following contents.
            ```
            #USB Blaster
            SUBSYSTEM=="usb", ENV{DEVTYPE}=="usb_device",ATTR{idVendor}=="09fb", ATTR{idProduct}=="6001",MODE="0666",NAME="bus/usb/$env{BUSNUM}/$env{DEVNUM}", RUN+="/bin/chmod 0666 %c"
            SUBSYSTEM=="usb", ENV{DEVTYPE}=="usb_device", ATTR{idVendor}=="09fb", ATTR{idProduct}=="6002",MODE="0666", NAME="bus/usb/$env{BUSNUM}/$env{DEVNUM}", RUN+="/bin/chmod 0666 %c"
            SUBSYSTEM=="usb", ENV{DEVTYPE}=="usb_device", ATTR{idVendor}=="09fb", ATTR{idProduct}=="6003",MODE="0666", NAME="bus/usb/$env{BUSNUM}/$env{DEVNUM}", RUN+="/bin/chmod 0666 %c"

            #USB Blaster II
            SUBSYSTEM=="usb", ENV{DEVTYPE}=="usb_device", ATTR{idVendor}=="09fb", ATTR{idProduct}=="6010",MODE="0666", NAME="bus/usb/$env{BUSNUM}/$env{DEVNUM}", RUN+="/bin/chmod 0666 %c"
            SUBSYSTEM=="usb", ENV{DEVTYPE}=="usb_device", ATTR{idVendor}=="09fb", ATTR{idProduct}=="6810",MODE="0666", NAME="bus/usb/$env{BUSNUM}/$env{DEVNUM}", RUN+="/bin/chmod 0666 %c"
            ```
            Run the following command to reload and trigger the new rules
            ```
            sudo udevadm control --reload-rules && udevadm trigger
            ```
- Type the .sof download command `nios2-configure-sof`, then you will see the "successful" message from Quartus® Programmer as below:

  ```
  $ cd <working directory root>/phase1-rfs/hw/Module5_Sample_HW/
  $ nios2-configure-sof -d 2 output_files/Module5_Sample_HW.sof
   ```
    *Possible **error**:*

    1. **Error\: nios2-configure-sof\: command not found**<br>
        - Possible root cause: missing environment variable.<br>

        Solution
        ```
        export PATH=$PATH:~/intelFPGA_lite/20.1/nios2eds/bin
        ```

  Output:
   ```
  Info: Quartus Prime Programmer was successful. 0 errors, 0 warnings
      Info: Peak virtual memory: 4442 megabytes
      ...
  ```

- Type the JTAG terminal command `nios2-terminal` to see output strings from running Nios® II software:

  ```
    $ nios2-terminal 
    nios2-terminal: connected to hardware target using JTAG UART on cable
    nios2-terminal: "DE-SoC [1-2]", device 2, instance 0
    nios2-terminal: (Use the IDE stop button or Ctrl-C to terminate)

    Welcome to DE10_NANO RFS_SENSOR DEMO!

    light0 = 62, light1 = 8, lux = 1.666
    Temperature: 27.384*C
    Humidity: 53.223%
    9-axis info:
    ax = 0.129, ay = 9.783, az = 0.378
    gx = -0.027, gy = 0.016, gz = 0.028
    mx = -21.445, my = 19.723, mz = -31.459
    TH:light 0 0 0 0 
    TH:Lux 0.000 0.000 
    TH:Temp 0.000 100.000 
    TH:HM 0.000 0.000 
    TH:a* 0.000 0.000 0.000 0.000 0.000 0.000 
    TH:g* 0.000 0.000 0.000 0.000 0.000 0.000 
    TH:m* 0.000 0.000 0.000 0.000 0.000 0.000 
    ...
    ...
  ```

You will see a set of output in about every 2 seconds. The output shows values of all sensors from RFS, two Light sensors, Temperature sensor, Humidity sensor, and 9-axis sensors. Six lines starting with "TH:" are threshold values which is defined by HPS. They should be all ZERO now since HPS is not operational at this point.

## Step 1.5: Generate RBF file

Generate .rbf file from .sof file.

```
cd <working directory root>/phase1-rfs/hw/Module5_Sample_HW/output_files
quartus_cpf -c -o bitstream_compression=on Module5_Sample_HW.sof Module5_Sample_HW.rbf
```

Module5_Sample_HW.rbf should be generated in output_files folder now.

## Step 1.6: Generate Preloader

If HPS settings in the new RBF are different from the one in the SD card image, we will need to update the Preloader on the SD card. But since this reference design uses the same HPS settings with FPGA design in DE10-Nano-Cloud-Native.img on SD card, you can skip this step.

You can refer to the [Update Preloader/U-boot on DE10-Nano](https://rocketboards.org/foswiki/Documentation/UpdatePreloaderUBootIn201OrLaterForCycloneV) if you want to update the Preloader.

---
Next Step : [Step 2: Driver Development Phase](step2-driver-development-phase.md)

Tool Setup: [Prerequisites Guide](prerequisites.md)  
Learn this Design: [Sensor Aggregation Design Architecture](reference-design.md)  
Move to [Top](../top.md)