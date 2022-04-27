# Step 2: Driver Development Phase

## Goals

The goals for this step are :

- Validate Linux Kernel Drivers
- Validate Device Tree files
    - Reconfiguring FPGA images
    - Loading Linux Kernel drivers
    - Accessing FPGA resources from HPS through kernel drivers

## Step 2.1: Preparation

### Step 2.1.1: Summary of Tools and Resources

List of tools and required resources used in this step 

- Development PC
  - Intel® SoC FPGA Embedded Development Suite (SoC EDS) Standard Edition version 20.1
    Follow Section **2. Installing the Tools** in [User Guide](https://www.intel.com/content/dam/www/programmable/us/en/pdfs/literature/ug/ug_soc_eds.pdf)
    
- Files
  - FPGA bitstream(.rbf) from [Step 1](step1-fpga-development-phase.md)
  - Linux Kernel Driver(.ko)
  - Device Tree Files(.dts/.dtb/.dtso/.dtbo)
  
### Step 2.1.2: Disable services accessing FPGA

We need to disable services that are accessing FPGA as we will reconfigure the FPGA in this step.

These are some typical services:
   
+ Azure IoT Edge Module from previous tutorials
+ fpgaoverlay.service
   
The image loaded by `fpgaoverlay.service` may not be compatible with the new design, thus you must ensure no Device Tree Overlay is being carried out.

For Azure IoT Edge, you can disable and stop iotedge service and daemon.
```
systemctl disable aziot-edged
iotedge system stop
```
As for `fpgaoverlay.service`, modify the script(`/overlay/fpgaoverlay.sh`), which is launched as Systemd start up service. Comment out the portion of the script to not load images and the framebuffer driver. This will stop display manager service as well.
```
Before:
    echo "Doing Device Tree Overlay"
    echo overlay.dtbo > $overlay_dir/path

    echo "Successfully Device Tree Overlay Done"

    echo "Loading altvipfb"
    modprobe altvipfb
    echo "Successfully altvipfb is loaded"

After:
    #echo "Doing Device Tree Overlay"
    #echo overlay.dtbo > $overlay_dir/path

    #echo "Successfully Device Tree Overlay Done"

    #echo "Loading altvipfb"
    #modprobe altvipfb
    #echo "Successfully altvipfb is loaded"
        
```
Once that is done, reboot De10-Nano.

## Step2.2: Linux Kernel Driver Development
For this reference design, we don't need to develop any Linux Kernel driver.
But if you are interested in developing Linux Kernel drivers, please refer to below documents.

+ [Altera SoC Workshop Series][LINK_Altera_SoC_Workshop_Series]
+ [WS3 Developing Drivers for Altera SoC Linux][LINK_W3S_Developing_Drivers_for_Altera_SoC_Linux]

## Step2.3: Recompile Device Tree Files
To compile the Device Tree Blob(dtb) and Device Tree Blob for Overlay(dtbo), you will use the `dtc` installed with SoC EDS.
For this tutorial, you can reuse `soc_system.dtb` which is the base device tree describing the HPS on DE10-Nano component settings. Since we don't use any unique Linux Kernel driver for the FPGA components, dtso will just describe which FPGA image to use for reconfiguration.
This step is similar to [Module 4][LINK_Develop_and_Deploy_Container_Reconfiguring_FPGA_Circuit_of_DE10-Nano_from_Azure_Cloud] and only the FPGA image name has changed.

Open a terminal in development PC.
```
<SoC EDS Installation Root>/embedded/embedded_command_shell.sh
cd <working directory root>/phase1-rfs/hw/dtb/base/
dtc -@ -I dts -O dtb -o new-overlay.dtbo new-overlay.dtso
```

## Step2.4: Test Online FPGA Reconfiguration

This step is same in [Module4][LINK_Develop_and_Deploy_Container_Reconfiguring_FPGA_Circuit_of_DE10-Nano_from_Azure_Cloud], where we will copy the dtbo and rbf to DE10-Nano and use the overlay script to reconfigure the FPGA.

1. Modify `sw/software-code/overlay/run.sh` to reconfigure FPGA.
    Please check around `FIXME` and ensure you have modified the values accordingly.
    ```bash
    #FIXME
    overlay_dir="/sys/kernel/config/device-tree/overlays/socfpga"
    overlay_dtbo="rfs-overlay.dtbo"
    overlay_rbf="Module5_Sample_HW.rbf"
    ```

2. Copy run.sh/dtbo/rbf files from development PC to DE10-Nano.
   
    ```
    scp <working directory root>/phase1-rfs/sw/software-code/overlay/run.sh root@<DE10-Nano IP address>:~/Downloads/
    scp <working directory root>/phase1-rfs/hw/dtb/rfs-overlay.dtbo root@<DE10-Nano IP address>:~/Downloads/
    scp <working directory root>/phase1-rfs/hw/Module5_Sample_HW/Module5_Sample_HW.rbf root@<DE10-Nano IP address>:~/Downloads/
    ```
   
3. Test Device Tree Overlay.

    ```
    cd ~/Downloads/
    chmod 777 run.sh
    ./run.sh
	```

    Output:
    ```
    root@de10nano:~/Downloads# chmod 777 run.sh 
    root@de10nano:~/Downloads# ./run.sh 
    Deleting /sys/kernel/config/device-tree/overlays/socfpga
    Copy DTBO and RBF
    creating /sys/kernel/config/device-tree/overlays/socfpga
    Doing Device Tree Overlay
    Successfully Device Tree Overlay Done.
    Message : Application Start
    python3: can't open file './main.py': [Errno 2] No such file or directory
    ```
    Once completed, You can check the message from `dmesg` command to write the new image successfully.
   
    ```bash
    dmesg
    # Last Line(timestamp may be different)
    [  158.924447] fpga_manager fpga0: writing Module5_Sample_HW.rbf to Altera SOCFPGA FPGA Manager
    ```
    *Note:If you cannot execute device tree overlay, you have to debug dtb or dtbo files from error messages in dmesg*
   
    Honestly, we don't have changed FPGA images and cannot see any changed functionality.
    If you want to confirm image change, A one of simplest is modifying sysid value in FPGA design, recompile and use it.

---
Next Step : [Step 3: Plug and Play Model Development Phase](step3-pnp-model-development-phase.md)  
Previous Step : [Step 1: FPGA Development Phase](step1-fpga-development-phase.md)  

Tool Setup: [Prerequisites Guide](prerequisites.md)  
Learn this Design: [Sensor Aggregation Design Architecture](reference-design.md)  
Move to [Top](../top.md)

[LINK_Altera_SoC_Workshop_Series]: https://rocketboards.org/foswiki/Documentation/AlteraSoCWorkshopSeries
[LINK_W3S_Developing_Drivers_for_Altera_SoC_Linux]: https://rocketboards.org/foswiki/Documentation/WS3DevelopingDriversForAlteraSoCLinux
[LINK_Develop_and_Deploy_Container_Reconfiguring_FPGA_Circuit_of_DE10-Nano_from_Azure_Cloud]: https://software.intel.com/content/www/us/en/develop/articles/reconfigure-an-fpga-from-the-cloud-with-containers.html
