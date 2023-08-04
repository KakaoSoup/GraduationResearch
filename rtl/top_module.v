`timescale 1ns/1ps

module top_module (
	input clk,	// clk signal | memory, mbist_top, bira_top에 연결
	input rst,	// reset, clk & rst = 1 : active high
    input test,	// test mode 시작, MBIST가 동작 -> 메모리 고장 주소 검출
	input [1:0] spare_struct,	// 3개의 spare structure 중에 1개를 선택
	output repair,	// repair 가능? 불가능? 알려줌
	output early_term,	// repair 불가능 할 시 Test가 종료됨을 나타냄
	output [15:0] solution	// row solution, column solution, bank_addr, spare_type에 대한 정보를 담은 16 bits
);

// memory.v로 연결
wire ce;	// control enable
wire we;	// write enable
wire [9:0] row_addr;	// row 주소
wire [9:0] col_addr;	// col 주소
wire [1:0] bank_addr;	// 01 : 1st bank, 10 : 2nd bank
wire [7:0] data_w;	// 1Byte word data to write
wire [7:0] data_r;	// 1Byte word data to read

// bira_top.v로 연결
wire fault_detect;	// 고장 검출 여부를 알려줌
wire [9:0] fault_row;	// fault의 row 주소
wire [9:0] fault_col;	// fault의 col 주소
wire [7:0] fault_col_flag;	// col에서 fault가 발생하는 정확한 위치
wire [1:0] fault_bank;	// fault bank => 01 : 1st bank, 10 : 2nd bank
				  
memory mem(
     // input
    .clk(clk),
    .ce(ce),
    .we(we),
    .row_addr(row_addr),
    .col_addr(col_addr),
    .bank_addr(bank_addr),
    .data_i(data_w),
    // output
    .data_o(data_r)
	);

mbist_top mbist(
    //input
    .clk(clk),
    .rst(rst),
    .test(test),
	.early_term(early_term),
	.data_r(data_r),
	//output
    .ce(ce),
    .we(we),
    .row_addr(row_addr),
    .col_addr(col_addr),
    .bank_addr(bank_addr),
    .data_w(data_w),
	.test_end(test_end),
	.fault_detect(fault_detect),
	.fault_row(fault_row),
	.fault_col(fault_col),
	.fault_col_flag(fault_col_flag),
	.fault_bank(fault_bank)
    );	
	
bira_top bira(
    //input
    .clk (clk),
    .rst (rst),
	.spare_struct (spare_struct),
    .test_end(test_end),
    .fault_detect(fault_detect),
    .row_add_in(fault_row),
    .col_add_in(fault_col),
	.col_flag(fault_col_flag),
	.bank_in(fault_bank),
	//output
    .early_term(early_term),
    .repair(repair),
    .solution(solution)
    );		  



endmodule
