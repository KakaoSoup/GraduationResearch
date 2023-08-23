﻿#include "header.h"
#include "CAM.h"
#include "singal_generator.h"
#include "signal_validity_checker.h"
#include "spare_allocation_analyzer.h"

int pcamCnt = 0;
const int struct_type = S3;
const int sig_len = (struct_type != S3) ? R_SPARE + C_SPARE : R_SPARE + C_SPARE - 1;

SignalGenerator generator;
Pcam pcam[PCAM_SIZE];
Npcam npcam[NPCAM_SIZE];
CamStruct cam;

// signal generator
bool DSSS[R_SPARE + C_SPARE];
bool RLSS[R_SPARE-1];

// signal validity checker
int pivot_block[PCAM_SIZE];
int must_repair[PCAM_SIZE];
bool uncover_must_pivot[PCAM_SIZE] = { 0 };
bool unused_spare[R_SPARE + C_SPARE] = { 1 };

// spare allocation analyzer
int uncover_nonpivot_addr[NPCAM_SIZE];
Spare pivot_cover[R_SPARE + C_SPARE];
bool nonpivot_cover_info[NPCAM_SIZE];

void store_CAM() {
	int pcam_ptr = 0;
	if (!early_term) {
		for (int k = 0; k < BNK; k++) {
			for (int i = 0; i < SIZE; i++) {
				for (int j = 0; j < SIZE; j++) {
					if (mem[k][i][j])
						cam.setCam(i, j, k+1);
				}
			}
		}
	}
	for (int i = 0; i < PCAM_SIZE; i++) {
		pivot_block[i] = cam.rtn_Pvblock(i);
		must_repair[i] = cam.rtn_must(i);
	}
}

void singal_generate() {
	generator.signal_generate();
}

void show_signals() {
	generator.show_dsss();
	cout << ' ';
	generator.show_rlss();
	cout << " is valid? : " << signal_valid();
}

void show_unused_spare() {
	cout << "unused spare : ";
	for (int i = 0; i < PCAM_SIZE; i++)
		cout << unused_spare[i];
}

void BIRA() {
	int testCnt = (struct_type != S3) ? 70 : 35 * 3;
	store_CAM();			// Fault collection
	cam.showPcam();
	cam.showNpcam();
	for (int i = 0; i < testCnt; i++) {
		singal_generate();
		if (signal_valid() && DSSS[1] && DSSS[2] && DSSS[6]) {
			spare_allocation();
			show_signals();
			cout << '\t';
			show_nonpivot_cover();
			cout << '\n';
			show_unused_spare();
			cout << endl;
		}
	}
}