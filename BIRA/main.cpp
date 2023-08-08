#include "header.h"

bool early_term;
bool mem[BANK][SIZE][SIZE];
Fault faults[FAULT];

int main(int argc, char** argv) {
	//freopen("input.txt", "r", stdin);		// read 'input.txt' file
	
	// reset signals and variables
	early_term = false;
	memset(mem, 0, sizeof(mem));
	memset(faults, 0, sizeof(faults));
	
	generate_fault();
	show_faults();

	//read_fault_file();
	
	print_mem();

	// run BIRA
	BIRA();
	return 0;
}