#include "CAM.h"
#include "analyzer.h"

Pcam pcam;
Npcam npcam(pcam);

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

void fault_analysis() {

}

void BIRA() {
	store_CAM();			// Fault collection
	pcam.show_pcam();
	npcam.show_npcam();

	//fault_analysis();
}