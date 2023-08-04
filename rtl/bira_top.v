`timescale 1ns / 1ps

module bira_top(
    input clk,						// 100MHz clk signal from top module
    input rst, 						// reset signal from top module
	input [1:0] spare_struct,		// types of spare structure from top module
    input test_end,					// Test End signal from BIST
    input fault_detect,				// fault alertion from BIST
    input [9:0] row_add_in,			// fault row address from BIST
    input [9:0] col_add_in,			// fault col address from BIST
	input [7:0] col_flag,			// fault col flag from BIST
	input [1:0] bank_in,			// fault bank address from BIST
    output early_term,				// if # of pivot fault > total # of spares, stop BIST
    output repair,					// repair possible?
    output [15:0] solution			// 16 bits = 3 bits(spare types) + 1 bit(row:0 or col:1 flag) + 2 bits(bank addr) + 10 bits(row or col solution)
);

//                  BIRA DESIGN                         //















//////////////////////////////////////////////////////////
endmodule
