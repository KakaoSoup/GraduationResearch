`timescale 1ns / 1ps

module memory (                
    input   clk,				// 100MHz clk signal from top module
    input    ce,				// 1 : do write or read / 0 : nothing to run from BIST
    input    we,				// 1 : write operation / 0 : read operation from BIST
    input  [9:0] row_addr,		// 10 bits temporal row address from BIST
    input  [9:0] col_addr,		// 10 bits temporal col address	from BIST
    input  [1:0] bank_addr,		// 2 bits temporal bank address from BIST
    input  [7:0] data_i,		// 1 Byte word data 'data_gen' of mbist_top.v
    output [7:0] data_o			// 1 Byte word data outputs to BIST
);

    reg [1023:0] bank0[1023:0];	// 1024 X 1024 memory, 1st bank
    reg [1023:0] bank1[1023:0];	// 1024 X 1024 memory, 2nd bank
    
	reg [7:0] data_reg;			// 1 Byte word data for output to BIST
	
	//initial $readmemh("/data2/fault_data/bank0.data", bank0);
	//initial $readmemh("/data2/fault_data/bank1.data", bank1);
    
    //error insertion example
    always @ (*) begin
       if((row_addr == 10'b00000_00010) && (col_addr == 10'b00000_01000))
			bank0[row_addr][col_addr] <= 1;
       if((row_addr == 10'b00001_00010) && (col_addr == 10'b00000_01000))
            bank0[row_addr][col_addr + 3] <= 1;
       if((row_addr == 10'b00000_00110) && (col_addr == 10'b00000_11000))
            bank1[row_addr][col_addr + 5] <= 1;
       if((row_addr == 10'b00000_11111) && (col_addr == 10'b00000_00000))
            bank1[row_addr][col_addr + 7] <= 1;		   
    end
	
	// WRITE OPERATION
    always @ (posedge clk) begin
        if (ce && we) begin										// if state of BIST : w0
            $display("row_addr: %d, col_addr: %d, bank: %b, data into mem: %h",  row_addr, col_addr, bank_addr, data_i);			
            if (bank_addr[0]) begin 							// current bank : 1st
			   bank0[row_addr][col_addr] <= data_i[7];
               bank0[row_addr][col_addr + 1] <= data_i[6];
               bank0[row_addr][col_addr + 2] <= data_i[5];
               bank0[row_addr][col_addr + 3] <= data_i[4];
               bank0[row_addr][col_addr + 4] <= data_i[3];
               bank0[row_addr][col_addr + 5] <= data_i[2];
               bank0[row_addr][col_addr + 6] <= data_i[1];
               bank0[row_addr][col_addr + 7] <= data_i[0];
			end
            else if (bank_addr[1]) begin 						// current bank : 2nd
			   bank1[row_addr][col_addr] <= data_i[7];
               bank1[row_addr][col_addr + 1] <= data_i[6];
               bank1[row_addr][col_addr + 2] <= data_i[5];
               bank1[row_addr][col_addr + 3] <= data_i[4];
               bank1[row_addr][col_addr + 4] <= data_i[3];
               bank1[row_addr][col_addr + 5] <= data_i[2];
               bank1[row_addr][col_addr + 6] <= data_i[1];
               bank1[row_addr][col_addr + 7] <= data_i[0];
			end			
        end
    end
	
	// READ OPERATION
    always @ (posedge clk) begin
        if (!ce) begin
            data_reg <= 0;
        end 
		else if (!we) 
		begin		    
            if (bank_addr[0]) begin 
			   data_reg[7] <= bank0[row_addr][col_addr];
               data_reg[6] <= bank0[row_addr][col_addr + 1];
               data_reg[5] <= bank0[row_addr][col_addr + 2];
               data_reg[4] <= bank0[row_addr][col_addr + 3];
               data_reg[3] <= bank0[row_addr][col_addr + 4];
               data_reg[2] <= bank0[row_addr][col_addr + 5];
               data_reg[1] <= bank0[row_addr][col_addr + 6];
               data_reg[0] <= bank0[row_addr][col_addr + 7];
			end
            else if (bank_addr[1]) begin 
			   data_reg[7] <= bank1[row_addr][col_addr];
               data_reg[6] <= bank1[row_addr][col_addr + 1];
               data_reg[5] <= bank1[row_addr][col_addr + 2];
               data_reg[4] <= bank1[row_addr][col_addr + 3];
               data_reg[3] <= bank1[row_addr][col_addr + 4];
               data_reg[2] <= bank1[row_addr][col_addr + 5];
               data_reg[1] <= bank1[row_addr][col_addr + 6];
               data_reg[0] <= bank1[row_addr][col_addr + 7];
			end		
        $display("row_addr: %d, col_addr: %d, bank: %b, data out mem: %h",  row_addr, col_addr, bank_addr, data_o);		
		end		
		else begin
            data_reg <= 0;
        end
    end

assign data_o = data_reg;


endmodule 
