module byte_adder(c,b,a);
	output [7:0]c;
    input [7:0]a,b;

    assign c = a + b;
    

endmodule