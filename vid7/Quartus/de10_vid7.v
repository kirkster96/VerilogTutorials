module f1(x,y,sw1,sw2,sw3);
	input sw1, sw2, sw3; output x, y;
	wire t1, t2;
	xor x1(t1, sw1,sw2); // (output, input(s))
	not n1(x,t1);
	and a1(t2, x, sw3);
	or o1(y,t2,sw2);
endmodule