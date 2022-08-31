module simple_alu(result,a,b,cntl
);
  output reg [8:0] result;
  reg Cout;
    
  input [7:0] a;
  input [7:0] b;
  input [7:0] cntl;
  //input Cin; cntl[7]
  //input [2:0] Control; cntl[2:0]
  
  	always @(*)
		begin
			case(cntl[2:0])
			3'b000: result =  a && b;
			3'b001: result =  a || b;
			3'b010: result =  {a[0],a[7:1]};
			3'b011: result =  a + b + cntl[7];
			3'b100: result =  a - b - cntl[7];
			3'b101: result =  {a[6:0],a[7]};
			3'b110: result =  a >> 1;
			3'b111: result =  a << 1;			
			default: result =  0;
			endcase
		end  
endmodule