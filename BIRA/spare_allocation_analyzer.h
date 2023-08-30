#ifndef __SPAREALLOCATIONANALYZER_H
#define __SPAREALLOCATIONANALYZER_H
#include "CAM.h"
#include "spare_struct.h"
#include "singal_generator.h"

extern int uncover_nonpivot_addr[NPCAM_SIZE];
extern Spare pivot_cover[R_SPARE + C_SPARE];
extern bool nonpivot_cover_info[NPCAM_SIZE];
extern void spare_allocation();
extern void show_nonpivot_cover();

class SpareAllocationAnalyzer {
private:
	Spare RRx[R_SPARE];
	Spare RCx[C_SPARE];
	int row_len;

public:
	// initialize repair candidate
	void init() {
		memset(RRx, 0, sizeof(RRx));
		memset(RCx, 0, sizeof(RCx));
	}

	SpareAllocationAnalyzer() {
		row_len = (STRUCT_TYPE != S3) ? R_SPARE : R_SPARE - 1;
		init();
	}

	// MUX : set repair candidate from PCAM by DSSS signal 
	void set_repair_cand() {
		int cidx = 0, ridx = 0;
		for (int i = 0; i < sig_len; i++) {
			if (DSSS[i] == ROW) {
				RRx[ridx].alloc = true;
				RRx[ridx].addr = pcam[i].row_addr;
				RRx[ridx++].bnk = pcam[i].bnk_addr;
			}
			else if (DSSS[i] == COL) {
				RCx[cidx].alloc = true;
				RCx[cidx].addr = pcam[i].col_addr;
				RCx[cidx++].bnk = pcam[i].bnk_addr;
			}
		}
	}

	// Row Address Comparator
	bool RAC(const int RRx_addr, const int NPr_addr, const int RRx_bnk, const int NPr_bnk, const bool RLSS) {
		return (RRx_addr != NPr_addr) ? false : (RLSS || (RRx_bnk == NPr_bnk));
	}

	// Col Address Comparator
	bool CAC(const int RCx_addr, const int NPc_addr, const int RCx_bnk, const int NPc_bnk) {
		return (RCx_addr != NPc_addr) ? false : (RCx_bnk != NPc_bnk) ? false : true;
	}

	// compare row part
	void comapare_row(const Npcam npcam[NPCAM_SIZE]) {
		for (int i = 0; i < NPCAM_SIZE; i++) {
			if (npcam[i].en) {
				if (npcam[i].rc == COL) {
					for (int j = 0; j < row_len; j++)
						nonpivot_cover_info[i] |= RAC(RRx[j].addr, pcam[npcam[i].pcam_ptr].row_addr, RRx[j].bnk, npcam[i].bnk, RLSS[j]);
				}
				else {
					for (int j = 0; j < row_len; j++)
						nonpivot_cover_info[i] |= RAC(RRx[j].addr, npcam[i].addr, RRx[j].bnk, npcam[i].bnk, RLSS[j]);
				}
			}
		}
	}

	// compare col part
	void comapare_col(const Npcam npcam[NPCAM_SIZE]) {
		for (int i = 0; i < NPCAM_SIZE; i++) {
			if (npcam[i].en) {
				if (npcam[i].rc == ROW) {
					for (int j = 0; j < C_SPARE; j++)
						nonpivot_cover_info[i] |= CAC(RCx[j].addr, pcam[npcam[i].pcam_ptr].col_addr, RCx[j].bnk, npcam[i].bnk);
				}
				else {
					for (int j = 0; j < C_SPARE; j++)
						nonpivot_cover_info[i] |= CAC(RCx[j].addr, npcam[i].addr, RCx[j].bnk, npcam[i].bnk);
				}
			}
		}
	}

	// show repair candidate
	void show_repaircand() {
		cout << "Row Repair Candidate :" << endl;
		for (int i = 0; i < R_SPARE; i++)
			printf("RRx[%d](addr, bnk) : %d, %d\n", i, RRx[i].addr, RRx[i].bnk);
		
		cout << "Col Repair Candidate :" << endl;
		for (int i = 0; i < C_SPARE; i++)
			printf("RCx[%d](addr, bnk) : %d, %d\n", i, RCx[i].addr, RCx[i].bnk);
	}

};

static void init() {
	memset(pivot_cover, -1, sizeof(pivot_cover));
	memset(nonpivot_cover_info, false, sizeof(nonpivot_cover_info));
}

extern void show_nonpivot_cover() {
	cout << "nonpivot_cover_info : ";
	for (int i = 0; i < NPCAM_SIZE; i++) {
		if (npcam[i].en) {
			cout << nonpivot_cover_info[i];
		}
	}
}

void spare_allocation() {
	//if (!signal_valid())
	//	return;
	init();
	SpareAllocationAnalyzer analyzer;

	for (int i = 0; i < pcamCnt; i++) {
		if (DSSS[i] == ROW) {
			pivot_cover[i].rc = ROW;
			pivot_cover[i].addr = pcam[i].row_addr;
		}
		else if (DSSS[i] == COL) {
			pivot_cover[i].rc = COL;
			pivot_cover[i].addr = pcam[i].col_addr;
		}
		pivot_cover[i].bnk = pcam[i].bnk_addr;
		pivot_cover[i].alloc = true;
	}

	analyzer.set_repair_cand();
	//analyzer.show_repaircand();
	analyzer.comapare_row(npcam);
	analyzer.comapare_col(npcam);
}

#endif // !__SPAREALLOCATIONANALYZER_H
