#ifndef HEADER_H
#define HEADER_H
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <random>
#include <cstring>
#define SIZE			8										// length of bank
#define LEN				static_cast<int>(ceil(log2(SIZE)))		// length of row, col address bits
#define FAULT			16										// # of total faults
#define BNK				2										// # of banks
#define ROW				1
#define COL				0
#define S1				1
#define S2				2
#define S3				3
#define R_SPARE			4
#define C_SPARE			4
#define PCAM_SIZE		(R_SPARE + C_SPARE)
#define NPCAM_SIZE		30
#define STRUCT_TYPE		S3

using namespace std;

// global variables
extern bool mem[BNK][SIZE][SIZE];		// memory fault map
extern bool early_term;					// early terminate signal
extern int sig_len;

// function
extern void BIRA();
extern void fault_generation();

#endif // !HEADER_H
