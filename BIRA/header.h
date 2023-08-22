#ifndef HEADER_H
#define HEADER_H
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <random>
#define SIZE 6				// length of bank
#define LEN	static_cast<int>(ceil(log2(SIZE)))		// length of row, col address bits
#define FAULT 10			// # of total faults
#define BNK 2				// # of banks
#define ROW 1
#define COL 0
#define R_SPARE	4
#define C_SPARE 4
#define PCAM_SIZE (R_SPARE + C_SPARE)
#define NPCAM_SIZE 30
using namespace std;


extern bool mem[BNK][SIZE][SIZE];		// memory fault map
extern bool early_term;					// early terminate signal

// function
extern void BIRA();
extern void fault_generation();

#endif // !HEADER_H
