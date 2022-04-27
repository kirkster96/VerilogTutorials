//
// Copyright (c) 2017 Intel Corporation
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//

// create module
module blink(
        input   clk,                    // 50MHz FPGA input clock

        input   [1:0] push_button,      // KEY[1:0]
        input   [3:0] switch,           // SW[3:0]

        output  [7:0] leds              // LED[7:0]
);

// Create a power on reset pulse for clean system reset on entry into user mode
// We create this with the altera_std_synchronizer core
wire sync_dout;
altera_std_synchronizer #(
        .depth (20)
) power_on_reset_std_sync_inst (
        .clk     (clk),
        .reset_n (1'b1),
        .din     (1'b1),
        .dout    (sync_dout)
);

// Create a qsys system reset signal that is the logical AND of the power on
// reset pulse and the KEY[0] push button
wire qsys_system_reset;
assign qsys_system_reset = sync_dout & push_button[0];

/*

Qsys system instantiation template from soc_system/soc_system_inst.v:

soc_system u0 (
    .button_pio_export (<connected-to-button_pio_export>), // button_pio.export
    .clk_clk           (<connected-to-clk_clk>),           //        clk.clk
    .led_pio_export    (<connected-to-led_pio_export>),    //    led_pio.export
    .reset_reset_n     (<connected-to-reset_reset_n>),     //      reset.reset_n
    .switch_pio_export (<connected-to-switch_pio_export>)  // switch_pio.export
);

 */

soc_system u0 (
        .button_pio_export (push_button[1]),     // button_pio.export
        .clk_clk           (clk),                //        clk.clk
        .led_pio_export    (leds),               //    led_pio.export
        .reset_reset_n     (qsys_system_reset),  //      reset.reset_n
        .switch_pio_export (switch)              // switch_pio.export
);

endmodule
