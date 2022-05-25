module  magcomparator(aLarger,bLarger,abEqual, a, b);
    output aLarger, bLarger, abEqual;
    input a,b;

    assign aLarger = a & ~b;
    assign bLarger = ~a & b;
    assign abEqual = ~a & ~b || a & b;

endmodule