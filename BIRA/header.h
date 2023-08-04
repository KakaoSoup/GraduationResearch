#ifndef HEADER_H
#define HEADER_H

#include <iostream>
#include <random>
#define LEN 4
#define SIZE 1 << LEN
#define FAULT 20
#define BANK 2
using namespace std;

struct Fault {
	int bank;
	int row_addr;
	int col_addr;
};

// global variables
extern bool mem[BANK][SIZE][SIZE];
//extern int mem[BANK][SIZE][SIZE];
extern Fault faults[FAULT];
// early tearm_singal
extern bool early_term;

// function
extern void generate_fault();
extern void show_faults();
extern void print_mem();
extern void BIRA();
extern void show_pcam();
extern void show_npcam();

#endif // !HEADER_H
