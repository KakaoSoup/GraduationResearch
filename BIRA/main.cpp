#include "header.h"

bool early_term;
bool mem[BNK][SIZE][SIZE];

int main(int argc, char** argv) {	
	// generate faults
	fault_generation();

	// run BIRA
	BIRA();
	
	return 0;
}