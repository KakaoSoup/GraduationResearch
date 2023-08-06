`timescale 1ns / 1ps

module mbist_top (              
    input clk,						// 100MHz clk signal
    input rst,
    input test,						// 1 : test mode, find the fault memory address
    input early_term,				// alert early termination to BIST
    input  [7:0] data_r,			// 1 Byte word read data from Memory
    output ce, 						// 1 : do write or read / 0 : nothing to run
    output we,						// 1 : write operation / 0 : read operation
    output [9:0] row_addr,			// 10 bits temporal row address : 0 ~ 1023
    output [9:0] col_addr,			// 10 bits temporal col address : 0 ~ 1023
    output [1:0] bank_addr,			// 2 bits temporal bank address : 01 : 1st bank, 10 : 2nd bank
    output [7:0] data_w,			// 1 Byte word write data
    output test_end,				// alert BIST is running(0) or terminated(1)
    output fault_detect,			// alert fault detection
    output [9:0] fault_row,			// 10 bits fault row address
	output [9:0] fault_col,			// 10 bits fault col address
    output [7:0] fault_col_flag,	// 8 bits fault position along row : index 0 ~ 7
    output [1:0] fault_bank			// 2 bits fault bank address : 0 ~ 3
);

//MBIST
reg ce_gen;							// connected with wire 'ce' (1 : do write or read / 0 : nothing to run)
reg we_gen;							// connected with wire 'we' (1 : write operation / 0 : read operation)
reg [8:0] row_addr_gen;				// 9 bits temporal block row address : 0 ~ 511
reg [8:0] col_addr_gen;				// 9 bits temporal block col address : 0 ~ 511
reg [1:0] bank_addr_gen = 2'b0;		// 2 bits temporal block address / 01 : 1st, 10 : 2nd
reg [7:0] data_gen;					// 8 bits temporal write data

//inner reg
reg check;							// alert the loop below whether write operation is before(0) or after(1) execution 	
reg pattern;						
reg first;							// indicate whether read operation was executed for first(1) or not yet(0)
reg [3:0] state;					// 4 bits state
reg [8:0] row_addr_buf;				// buf
reg [8:0] col_addr_buf;
reg [1:0] bank_addr_buf = 2'b0;
reg [8:0] row_addr_buf2;			// buf2
reg [8:0] col_addr_buf2;
reg [1:0] bank_addr_buf2 = 2'b0;


//BIRA
reg test_end_reg;					// alert to Top module that test was terminated
reg fault_detect_reg;
reg [1:0] fault_bank_reg;
reg [8:0] fault_row_reg;
reg [8:0] fault_col_reg;
reg [7:0] fault_col_flag_reg;		// find the position of fault in 8 bits word data, index : 0 ~ 7

// state types
localparam 
    IDLE = 'd0,				// IDLE : 0
    state0_w0 = 'd1,		// w0 : 1
    state1_r0 = 'd2,		// r0 : 2
	STOP = 'd3;				// STOP : 3
		
always @ (posedge clk) 

begin
    if((rst) || (early_term)) begin		// if signal 'rst' or 'early_term' is received, resets the Test by set 0 to all param
	ce_gen <= 0;					
        we_gen <= 0;					
        row_addr_gen <= 0;				
        col_addr_gen <= 0;				
        bank_addr_gen <= 0;
        row_addr_buf <= 0;
        col_addr_buf <= 0;
        bank_addr_buf <= 0;		
	data_gen <= 0;		
        check <= 0;
        pattern <= 0;
	first <=0;
	state <= IDLE;				
        test_end_reg <= 0;        		
	end
    else begin
        case(state)
	// state == IDLE
        IDLE: begin
        	//$display("State = IDLE");
        	if(test) begin				// test == 1 : test mode
        		state <= state0_w0;		// state => w0
			bank_addr_gen <= 2'b01;	// bank address => 1st bank
        	end
        	else begin					// test == 0 : test mode X
                	state <= IDLE;			// hold the state IDLE
        	end
	end
		
	// state == w0
        state0_w0: begin
   	        we_gen <= 1;								// do write operation
            	ce_gen <= 1;								// activate read / wirte operation
	        check <= 0;									// check = 0
           	row_addr_gen <= row_addr_gen + 1; 			// move row address 1 by 1
            	data_gen <=  8'b0000_0000;					// set temporal write data = 0
            	if(row_addr_gen == 'd511) begin				// temporal row meet the half of bank
          		if(col_addr_gen == 'd504) begin    		// col address meet the half of bank
				bank_addr_gen <= 2'b10;				// bank : 1st -> 2nd
                   		col_addr_gen <= 0;					// col address = 0
				if(bank_addr_gen == 2'b10) begin		// if bank address is already in 2nd
	   	               		first <= 0;						// reset first
                       			ce_gen <= 0;				   		// reset ce_gen stop write/read operation
				       state <= state1_r0;				// state => r0, terminate write operation
				end				   
			end
			else begin							
				col_addr_gen <= col_addr_gen + 8; 	// col address != 504(last col)
				row_addr_gen <= 0;					// move next col line
			end
            	end
	end

	// state == r0
        state1_r0: begin
        	if(first == 0) begin    						// with first of read operation
			row_addr_gen <= 0;							// reset temporal row address
			col_addr_gen <= 0;							// reset temporal col address
	            	first <= 1;									// set first 1 to skip current loop
			bank_addr_gen <= 2'b01;						// set temporal bank : 1st
		end
            	else begin
   	        	we_gen <= 0;								// do read operaion
                	ce_gen <= 1;								// activate read / wirte operaion
	            	check <= 1;									// activate another loop below
			pattern <= 0;							
			row_addr_buf <= row_addr_gen;				// dupplicate temporal address -> buffer
			col_addr_buf <= col_addr_gen;			
			bank_addr_buf <= bank_addr_gen;
                	row_addr_gen <= row_addr_gen + 1;			// move row address 1 by 1
			if(row_addr_gen == 'd511) begin				// temporal row meet the half of bank
          	       		if(col_addr_gen == 'd504) begin     		// temporal col address meet the half of bank
			    	   	bank_addr_gen <= 2'b10;				// bank : 1st -> 2nd
					col_addr_gen <= 0;					// col address = 0
			    	   	if(bank_addr_gen == 2'b10) begin		// if bank address is already 2nd, reset all variables
	   	                   		first <= 0;					
                           			ce_gen <= 0;
						test_end_reg <= 1;				// terminate test			   
			    	       		state <= STOP;					// state => STOP, terminate read operation
			    	   	end				   
			    	end
			    	else begin
			    		col_addr_gen <= col_addr_gen + 8;  	// col address != 504(last col)
			    		row_addr_gen <= 0;					// move next col line
			    	end
                	end
            	end
	end

	// state == STOP
        STOP:
        	if(test == 0) begin		// test == 0, test mode X
        		state <= IDLE;		// state => IDLE
            	end       
		else begin				// test == 1, test mode
			state <= STOP;		// hold the state STOP
		end
        endcase
    end
end

//assign wire
assign ce = ce_gen;
assign we = we_gen;
assign row_addr = row_addr_gen;
assign col_addr = col_addr_gen;
assign bank_addr = bank_addr_gen;
assign data_w = data_gen;
assign test_end = test_end_reg;

assign fault_detect = (|fault_col_flag_reg)? 1 : 0;					// is any fault in 8bit word data?
assign fault_bank = (|fault_col_flag_reg)? bank_addr_buf2 : 0;		// outputs current address which has fault 
assign fault_row =  (|fault_col_flag_reg)? row_addr_buf2 : 0;		
assign fault_col =  (|fault_col_flag_reg)? col_addr_buf2 : 0;
assign fault_col_flag = fault_col_flag_reg;							// outputs the fault positon of current 8bits word data


//bist fault check
always @ (posedge clk) begin
    if(rst) begin					// reset param about fault information
        fault_bank_reg <= 0;
        fault_row_reg <= 0;
        fault_col_reg <= 0;
        fault_col_flag_reg <= 0;
        fault_detect_reg <= 0;
    end
    else begin
        if(check) begin							// after write operation
            bank_addr_buf2 <= bank_addr_buf;	// dupplicate buf -> buf2
		row_addr_buf2 <= row_addr_buf;	
		col_addr_buf2 <= col_addr_buf;	
			
		if(pattern == (~data_r[7])) begin	// MSB(1st) of read data from Memory == pattern(0)?
                	fault_col_flag_reg[7] <= 1;		// if fault on 1st bit, set 1 to 1st bit of fault_col_flag_reg
		end
		else begin
			fault_col_flag_reg[7] <= 0;		
		end
		
		if(pattern == (~data_r[6])) begin	// 2nd bit of read data from Memory == pattern(0)?	
                	fault_col_flag_reg[6] <= 1;		// if fault on 2nd bit, set 1 to 2nd bit of fault_col_flag_reg			
		end
		else begin
			fault_col_flag_reg[6] <= 0;		
		end
		
		if(pattern == (~data_r[5])) begin	// 3rd bit of read data from Memory == pattern(0)?
                	fault_col_flag_reg[5] <= 1;		// if fault on 3rd bit, set 1 to 3rd bit of fault_col_flag_reg
		end
		else begin
			fault_col_flag_reg[5] <= 0;
		end
		
		if(pattern == (~data_r[4])) begin	// 4th bit of read data from Memory == pattern(0)?
                	fault_col_flag_reg[4] <= 1;		// if fault on 4th bit, set 1 to 4th bit of fault_col_flag_reg
		end
		else begin
			fault_col_flag_reg[4] <= 0;
		end

		if(pattern == (~data_r[3])) begin	// 5th bit of read data from Memory == pattern(0)?	
                	fault_col_flag_reg[3] <= 1;		// if fault on 5th bit, set 1 to 5th bit of fault_col_flag_reg
		end
		else begin
			fault_col_flag_reg[3] <= 0;
		end

		if(pattern == (~data_r[2])) begin	// 6th bit of read data from Memory == pattern(0)?
                	fault_col_flag_reg[2] <= 1;		// if fault on 6th bit, set 1 to 6th bit of fault_col_flag_reg
		end
		else begin
			fault_col_flag_reg[2] <= 0;
		end

		if(pattern == (~data_r[1])) begin	// 7th bit of read data from Memory == pattern(0)?
                	fault_col_flag_reg[1] <= 1;		// if fault on 7th bit, set 1 to 7th bit of fault_col_flag_reg
		end
		else begin
			fault_col_flag_reg[1] <= 0;
		end
			
		if(pattern == (~data_r[0])) begin	//  LSB(8th) of read data from Memory == pattern(0)?
                	fault_col_flag_reg[0] <= 1;		// if fault on 8th bit, set 1 to 8th bit of fault_col_flag_reg
		end
		else begin
			fault_col_flag_reg[0] <= 0;
		end
        end
    end        
end
endmodule                    


