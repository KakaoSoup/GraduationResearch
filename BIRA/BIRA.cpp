#include "CAM.h"
Pcam pcam;
Npcam npcam;

int in_pcam(int r, int c) {
	for (int i = 0; i < pcam.pcam_cnt(); i++) {
		if (pcam.r_addr(i) == r || pcam.c_addr(i) == c)
			return i+1;
	}
	return 0;
}

void set_npcam(int ptr, int r, int c, int b) {
	// share row
	if (r == pcam.r_addr(ptr)) {
		npcam.set_npcam(ptr, 0, r, b);
	}
	// share col
	else if (c == pcam.c_addr(ptr)) {
		npcam.set_npcam(ptr, 1, c, b);
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
						if ((pcam_ptr = in_pcam(i, j)) && !npcam.full())	// parallel access to PCAM
							set_npcam(pcam_ptr - 1, i, j, k);
						else
							set_pcam(i, j, k);
					}
					else
						continue;
				}
			}
		}
	}
}

void BIRA() {
	store_CAM();
	pcam.show_pcam();
	npcam.show_npcam();
}