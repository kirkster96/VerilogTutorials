module byte_accum(btn,led);
    input btn;
    output [7:0]led;
    reg [7:0] count;

    always @(posedge btn) count <= count + 1'b1;

    assign led[7] = count[7];
    assign led[6] = count[6];
    assign led[5] = count[5];
    assign led[4] = count[4];
    assign led[3] = count[3];
    assign led[2] = count[2];
    assign led[1] = count[1];
    assign led[0] = count[0];
endmodule