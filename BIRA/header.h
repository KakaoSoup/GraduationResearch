#ifndef HEADER_H
#define HEADER_H
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <random>
#define LEN 3				// length of row, col address bits
#define SIZE 1 << LEN		// length of bits along the row or col side of memory
#define FAULT 10			// # of total faults
#define BANK 2				// # of banks
using namespace std;

struct Fault {
	int bank;			// bank address
	int row_addr;		// row address
	int col_addr;		// col address
};

// global variables
extern bool mem[BANK][SIZE][SIZE];		// memory fault map
//extern int mem[BANK][SIZE][SIZE];		
extern Fault faults[FAULT];				// actual memory fault address
// early tearm_singal
extern bool early_term;					// early terminate signal

// function
extern void generate_fault();
extern void show_faults();
extern void read_fault_file();
extern void print_mem();
extern void BIRA();
extern void show_pcam();
extern void show_npcam();

#endif // !HEADER_H
