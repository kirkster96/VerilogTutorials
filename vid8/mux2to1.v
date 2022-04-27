module mux2to1(c, s, a,b);
    output c;
    input s,a,b;
    assign c = s ? a : b; // continous assignment
endmodule