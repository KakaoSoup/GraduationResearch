#include "CAM.h"
#include "analyzer.h"

Pcam pcam;
Npcam npcam(pcam);
Block row_part[NPCAM];
Block col_part[NPCAM];

int in_pcam(int r, int c) {
	for (int i = 0; i < pcam.pcam_cnt(); i++) {
		if (pcam.r_addr(i) == r || pcam.c_addr(i) == c)
			return i+1;
	}
	return 0;
}

void set_npcam(int ptr, int r, int c, int b) {
	unsigned flag = 0;
	if (flag = pcam.must_flag(ptr)) {
		cout << "must flag is set" << endl;
		if (b == pcam.b_addr(ptr)) {
			if (flag == 0x8 && r == pcam.r_addr(ptr))
				return;
			else if (flag = 0x4 && c == pcam.c_addr(ptr))
				return;
		}
		else {
			if (flag == 0x2 && r == pcam.r_addr(ptr))
				return;
			else if (flag = 0x1 && c == pcam.c_addr(ptr))
				return;
		}
	}
	// share row
	if (r == pcam.r_addr(ptr)) {
		npcam.set_npcam(ptr, 0, c, b, &pcam);
	}
	// share col
	else if (c == pcam.c_addr(ptr)) {
		npcam.set_npcam(ptr, 1, r, b, &pcam);
	}
}

void set_pcam(int r, int c, int b) {
	pcam.set_pcam(r, c, b);
}

void store_CAM() {
	int pcam_ptr = 0;
	if (!early_term) {
		for (int k = 0; k < BANK; k++) {
			for (int i = 0; i < SIZE; i++) {
				for (int j = 0; j < SIZE; j++) {
					if (mem[k][i][j]) {
						if ((pcam_ptr = in_pcam(i, j))) { // parallel access to PCAM
							set_npcam(pcam_ptr - 1, i, j, k+1);
						}
						else
							set_pcam(i, j, k+1);
					}
					else
						continue;
				}
			}
		}
	}
}

void set_analyzer(bool* dsss, bool* rlss, bool* clss, bool* nonpivot_cover_info) {
	for (int i = 0; i < NPCAM; i++) {
		if (npcam.dsrpt(npcam.show_ith_cam(i)) == ROW)
			row_part[i].set(ROW, dsss, rlss, npcam.show_ith_cam(i), &pcam, &npcam, true);
		else
			col_part[i].set(COL, dsss, clss, npcam.show_ith_cam(i), &pcam, &npcam, true);
		nonpivot_cover_info[i] = row_part[i].result() || col_part[i].result();
		cout << "nonpivot_cover_info[" << i << "] : " << nonpivot_cover_info[i] << endl;
	}
}


void fault_analysis() {
	bool DSSS[8][R_SPARE + C_SPARE] = {
		{0, 0, 0, 1, 1},
		{0, 0, 0, 1, 1},
		{0, 0, 1, 1, 0},
		{0, 0, 1, 1, 0},
		{1, 0, 1, 0, 1},
		{1, 0, 1, 0, 1},
		{1, 1, 0, 0, 0},
		{1, 1, 0, 0, 0}
	};
	bool RLSS[R_SPARE];
	bool CLSS[C_SPARE];
	bool nonpivot_cover_info[NPCAM];

	set_analyzer(DSSS[0], RLSS, CLSS, nonpivot_cover_info);
}

void BIRA() {
	store_CAM();			// Fault collection
	pcam.show_pcam();
	npcam.show_npcam();
	fault_analysis();
}