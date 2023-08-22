#include "header.h"

bool early_term;
bool mem[BNK][SIZE][SIZE];


int main(int argc, char** argv) {	
	fault_generation();
	BIRA();
	return 0;
}