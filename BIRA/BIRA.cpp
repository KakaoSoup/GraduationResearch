#include "header.h"
#include "CAM.h"
#include "singal_generator.h"
#include "signal_validity_checker.h"
#include "spare_allocation_analyzer.h"
#include "redundant_analyzer.h"

// Enviroment variables
int pcamCnt = 0;
int sig_len = (STRUCT_TYPE != S3) ? (R_SPARE + C_SPARE) : (R_SPARE + C_SPARE - 1);

// Modules
CamStruct cam;
Pcam pcam[PCAM_SIZE];
Npcam npcam[NPCAM_SIZE];
SignalGenerator generator;
RedundantAnalyzer redundant_analyzer;

// Signal Generator variables
bool DSSS[R_SPARE + C_SPARE];
bool RLSS[R_SPARE-1];

// Signal Validity Checker variables
int pivot_block[PCAM_SIZE];
int must_repair[PCAM_SIZE];
bool uncover_must_pivot[PCAM_SIZE] = { 0 };
bool unused_spare[R_SPARE + C_SPARE] = { 1 };

// Spare Allocation Analyzer variables
Spare pivot_cover[R_SPARE + C_SPARE];
int uncover_nonpivot_addr[NPCAM_SIZE];
bool nonpivot_cover_info[NPCAM_SIZE];

// store fault to CAM structure
static void store_CAM() {
	int pcam_ptr = 0;
	
	for (int k = 0; k < BNK; k++) {
		for (int i = 0; i < SIZE; i++) {
			for (int j = 0; j < SIZE; j++) {
				if (mem[k][i][j])
					early_term = !cam.setCam(i, j, k + 1);
				if (early_term)
					return;
			}
		}
	}
	for (int i = 0; i < PCAM_SIZE; i++) {
		pivot_block[i] = cam.rtn_pvblock(i);
		must_repair[i] = cam.rtn_must(i);
	}
}

// run singal generator with 1 by 1
static void singal_generate() {
	generator.signal_generate();
}

// show DSSS and RLSS signals
static void show_signals() {
	generator.show_dsss();
	cout << ' ';
	generator.show_rlss();
	cout << " is valid? : " << signal_valid();
}

// show unused spares
static void show_unused_spare() {
	cout << "unused spare : ";
	for (int i = 0; i < PCAM_SIZE; i++)
		cout << unused_spare[i];
}

static void show_info() {
	show_signals();
	cout << '\t';
	show_nonpivot_cover();
	cout << '\n';
	show_unused_spare();
	cout << endl;
}

void BIRA() {
	// total clk : S1, S2 -> 8C4, S3 -> 7C3 * 3
	int testCnt = (STRUCT_TYPE != S3) ? 70 : 35 * 3;

	// Fault collection
	store_CAM();
	if (early_term) {
		cout << "Test was ealry terminated!!" << endl;
		return;
	}
	cam.showPcam();
	cam.showNpcam();

	// Fault Analysis with 1 clk
	for (int i = 0; i < testCnt; i++) {
		singal_generate();
		if (signal_valid() && DSSS[1] && DSSS[2] && DSSS[6]) {
		//if(signal_valid()) {
			spare_allocation();
			show_info();
			if (redundant_analyzer.show_final_result())
				cout << "***Faults are repaired!!***" << endl;
		}
	}
}