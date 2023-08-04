`timescale 1ns/1ps
module test_bench();
    reg clk, rst, test, repair, early_term;
	reg [15:0] solution;
   
   
top_module dut(clk, rst, test, repair, early_term, solution);
		
	initial 
    begin
    clk = 1'b0;
    repeat(1000000) #10 clk = ~clk;
    end

    initial begin
	    rst = 1'b1;
		#20
	    rst = 1'b0;
        test = 1'b1;
		#1000000
		test = 1'b0;				
        #100 $stop;
    end
endmodule