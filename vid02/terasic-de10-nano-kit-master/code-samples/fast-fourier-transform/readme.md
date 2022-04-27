# Example Programs for Fast Fourier Transform (FFT) Application

There are two example programs for the FFT application:

* Example Program 1: Uses the ARM\* CPU & NEON\* co-processor to perform the FFT calculation
* Example Program 2: Uses the FFT hardware in the FPGA.

There are many variants of these examples, using different data sizes, and data sources (FFT RAM vs. CPU cache) in the src directory. There are also scripts for generating waveform sample data, and plotting the FFT results if you wish to experiment further.

The program listed below below performs the FFT operation using the ARM CPU and NEON co-processor using data stored on the SD Card.
 
```  
neon32_256.c
``` 
 
The program listed below performs the FFT operation using the hardware accelerator and DMAs inside the FPGA using data stored on the SD Card. The input and output data pass through the Acceleration Coherency Port (ACP) of the processor's L2 cache.

``` 
ffftdma_256.c
``` 
 
*Please see the README files in the sandbox directory for more information.*
 
 
# Tutorials
 
The README_SANDBOX.txt file contains detailed instructions, scripts and functions for interacting with the board.

### SSH

If you are using SSH to access the board, it is handy to have two terminals open to the same example directory:

1. Display the README file in one terminal (e.g. cat readme_sandbox.txt)
1. Copy the example scripts from this terminal and paste into the second terminal as you progress through the tutorial.

### Linux Desktop

If you are accessing the board's Linux desktop, we recommend the following:

1. Open the README file with gedit.
1. Open a terminal window and navigate to the example directory.
1. Copy scripts from gedit and paste into the terminal window.
 
 
# Build Scripts and Related C code

Scripts are provided to build the applications, generate sample data, and prepare results to be displayed using tools like GNU Plot.
 
### Building the applications
```
sandbox_build_all.sh
target_build_app.sh
target_build_lib.sh
target_build_ne10.sh
clean_all.sh

``` 
### Generate Sample Data
```
sandbox_create_input_waveforms.sh
create_real_short_sine32.c
create_real_short_square32.c
create_real_short_triangle32.c
duplicate_x8.sh
real_short_to_ne10cpx_long.c
real_short_to_ne10cpx_short.c
```
### Prepare Results For Display
```
ne10cpx_long_to_text.c
```
 
 
# Examples Directory

The FFT examples directory is located on the SD Card that shipped with the DE10-Nano kit. If your SD Card has been damaged, erased, or misplaced, you can get the latest image from the SD Card downloads site <need link for SDCard git>. This directory contains source code, archives, binaries, and a sub-directory called the sandbox.  

The sandbox directory contains source files, scripts and documentation you can review, run, modify, and recompile as you wish. It also contains a tar ball which is a copy of the entire sandbox contents so they can be restored if needed.
 
 
/examples/fft

```
├── bin
│   ├── fft.tgz
│   ├── README_TARGET.TXT
│   └── setup_target_fft_env.sh
├── sandbox
│   ├── clean_all.sh
│   ├── COPYING
│   ├── create_real_short_sine32.c
│   ├── create_real_short_square32.c
│   ├── create_real_short_triangle32.c
│   ├── duplicate_x8.sh
│   ├── fftdma_256.c
│   ├── ne10cpx_long_to_text.c
│   ├── Ne10-master.zip
│   ├── neon32_256.c
│   ├── overhead.c
│   ├── README_SANDBOX_BUILD.TXT
│   ├── README_SANDBOX.TXT
│   ├── real_short_to_ne10cpx_long.c
│   ├── real_short_to_ne10cpx_short.c
│   ├── sandbox_build_all.sh
│   ├── sandbox_create_input_waveforms.sh
│   ├── target_build_app.sh
│   ├── target_build_lib.sh
│   └── target_build_ne10.sh
├── src
│   ├── images
│   │   ├── create_gnuplot_sine_256.gnu
│   │   ├── create_gnuplot_sine_256x32.gnu
│   │   ├── create_gnuplot_sine_4096.gnu
│   │   ├── create_gnuplot_squares_256.gnu
│   │   ├── create_gnuplot_squares_256x32.gnu
│   │   ├── create_gnuplot_squares_4096.gnu
│   │   ├── create_gnuplot_triangle_256.gnu
│   │   ├── create_gnuplot_triangle_256x32.gnu
│   │   ├── create_gnuplot_triangle_4096.gnu
│   │   ├── create_montage_sine_256.sh
│   │   ├── create_montage_sine_256x32.sh
│   │   ├── create_montage_sine_4096.sh
│   │   ├── create_montage_squares_256.sh
│   │   ├── create_montage_squares_256x32.sh
│   │   ├── create_montage_squares_4096.sh
│   │   ├── create_montage_triangle_256.sh
│   │   ├── create_montage_triangle_256x32.sh
│   │   └── create_montage_triangle_4096.sh
│   ├── archive_for_target.sh
│   ├── c16_256.c
│   ├── c16_256x32.c
│   ├── c16_256x32x128.c
│   ├── c16_4096.c
│   ├── c32_256.c
│   ├── c32_256x32.c
│   ├── c32_256x32x128.c
│   ├── c32_4096.c
│   ├── clean_all.sh
│   ├── COPYING
│   ├── create_input_waveforms.sh
│   ├── create_real_short_sine32.c
│   ├── create_real_short_square32.c
│   ├── create_real_short_triangle32.c
│   ├── duplicate_x128.sh
│   ├── duplicate_x32.sh
│   ├── duplicate_x8.sh
│   ├── fft_256.c
│   ├── fft_256x32.c
│   ├── fft_256x32x128.c
│   ├── fft_4096.c
│   ├── fftdma_256.c
│   ├── fftdma_256x32.c
│   ├── fftdma_256x32x128.c
│   ├── fftdma_4096.c
│   ├── ne10cpx_long_to_text.c
│   ├── ne10cpx_short_to_text.c
│   ├── Ne10-master.zip
│   ├── neon16_256.c
│   ├── neon16_256x32.c
│   ├── neon16_256x32x128.c
│   ├── neon16_4096.c
│   ├── neon32_256.c
│   ├── neon32_256x32.c
│   ├── neon32_256x32x128.c
│   ├── neon32_4096.c
│   ├── overhead.c
│   ├── README_TARGET_BUILD.TXT
│   ├── README_TARGET.TXT
│   ├── real_short_to_ne10cpx_long.c
│   ├── real_short_to_ne10cpx_short.c
│   ├── run_all.sh
│   ├── run_fft_256.sh
│   ├── run_fft_256x32.sh
│   ├── run_fft_256x32x128.sh
│   ├── run_fft_4096.sh
│   ├── run_stream_256x32x128.sh
│   ├── setup_target_fft_env.sh
│   ├── stream_fpga_256x32x128.c
│   ├── stream_neon32_256x32x128.c
│   ├── stream_raw_256x32x128.c
│   ├── target_build_all.sh
│   ├── target_build_app.sh
│   ├── target_build_lib.sh
│   └── target_build_ne10.sh
├── fft_sandbox.tgz
└── fft_src.tgz
 ```
