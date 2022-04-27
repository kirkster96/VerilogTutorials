module f1_module(x,y,out);
	input x, y; output out;
	wire not_x, not_y, t3, t4;
	not n1(not_x, x);
	not n2(not_y, y);
	and a1(t3, x, not_y);
	and a2(t4, y, not_x);
	or o1(out,t3,t4);	
endmodule