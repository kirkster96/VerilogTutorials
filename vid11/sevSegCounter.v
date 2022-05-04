module sevSegCounter(clk, counter, sevenseg0);
    input clk;
    wire specialClock;
    output [6:0] sevenseg0;
    output reg [3:0] counter;

    slowClock U2 (clk,specialClock );

    always @(posedge specialClock) begin
        if(counter == 4'b1001)
            counter = 4'b0000;
        else
            counter = counter + 1;
    end


    sevSegDecoder U1 (sevenseg0, counter);

endmodule

module sevSegDecoder (
    output reg [6:0] x,
    input [3:0] a
);
    always @(*) begin
        case(a)
            4'H0: x = 7'b1110111; //0
            4'H1: x = 7'b1000001; //1
            4'H2: x = 7'b1001101;
            4'H3: x = 7'b1101110;
            4'H4: x = 7'b1011011;
            4'H5: x = 7'b0000000;
            4'H6: x = 7'b0000000;
            4'H7: x = 7'b0000000;
            4'H8: x = 7'b0000000;
            4'H9: x = 7'b0001000;

        endcase
    end
    
endmodule

module slowClock(clk_50MHz, clk);
    input clk_50MHz;
    output clk;
    reg clk;
    reg [24:0] counter;

    always @(posedge clk_50MHz) begin
        if(counter == 24999999) begin

            counter <= 0;
            clk <= ~clk;
        end
        else begin
            counter <= counter + 1;
        end

    end

endmodule