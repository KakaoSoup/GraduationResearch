#include "CAM.h"
#include "singal_generator.h"
#include "signal_validity_checker.h"
#include "spare_allocation_analyzer.h"

int pcamCnt = 0;
int struct_type = S3;
SignalGenerator generator;
Pcam pcam[PCAM_SIZE];
Npcam npcam[NPCAM_SIZE];
CamStruct cam;

// signal generator
bool DSSS[R_SPARE + C_SPARE];
bool RLSS[R_SPARE];

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
						cam.setCam(i, j, k);
				}
			}
		}
	}
	for (int i = 0; i < PCAM_SIZE; i++) {
		pivot_block[i] = cam.rtn_Pvblock(i);
		must_repair[i] = cam.rtn_must(i);
	}
}

extern void singal_generate() {
	generator.signal_generate();
	generator.show_dsss();
	cout << ' ';
	generator.show_rlss();
	cout << " is valid? : " << signal_valid() << endl;
}


void BIRA() {
	store_CAM();			// Fault collection
	cam.showPcam();
	cam.showNpcam();
	for (int i = 0; i < 70; i++) {
		singal_generate();
		spare_allocation();
	}
}