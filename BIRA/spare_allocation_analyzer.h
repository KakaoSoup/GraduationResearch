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
public:
	void init() {
		memset(nonpivot_cover_info, false, sizeof(nonpivot_cover_info));
	}
	SpareAllocationAnalyzer() {
		init();
	}
	void set_repair_cand() {
		int cidx = 0, ridx = 0;
		for (int i = 0; i < R_SPARE + C_SPARE; i++) {
			if (pivot_cover[i].alloc) {
				if (pivot_cover[i].rc == ROW) {
					RRx[ridx].addr = pivot_cover[i].addr;
					RRx[ridx++].bnk = pivot_cover[i].bnk;
				}
				else if (pivot_cover[i].rc == COL) {
					RCx[cidx].addr = pivot_cover[i].addr;
					RCx[cidx++].bnk = pivot_cover[i].bnk;
				}
			}
		}
	}
	bool RAC(int RRx_addr, int NPr_addr, int RRx_bnk, int NPr_bnk, bool RLSS) {
		bool result = 0;
		result = (RRx_addr != NPr_addr) ? false : (RLSS || (RRx_bnk == NPr_bnk));
		return result;
	}
	bool CAC(int RRx_addr, int NPr_addr, int RRx_bnk, int NPr_bnk) {
		bool result = 0;
		result = (RRx_addr != NPr_addr) ? false : (RRx_bnk != NPr_bnk) ? false : true;
		return result;
	}
	void comapare_row(const Npcam npcam[NPCAM_SIZE]) {
		for (int i = 0; i < NPCAM_SIZE; i++) {
			if (npcam[i].rc == ROW)
				for (int j = 0; j < R_SPARE; j++)
					nonpivot_cover_info[i] |= RAC(RRx[j].addr, npcam[i].addr, RRx[j].bnk, npcam[i].bnk, RLSS[j]);
		}
	}
	void comapare_col(const Npcam npcam[NPCAM_SIZE]) {
		for (int i = 0; i < NPCAM_SIZE; i++) {
			if (npcam[i].rc == COL)
				for (int j = 0; j < C_SPARE; j++)
					nonpivot_cover_info[i] |= CAC(RCx[j].addr, npcam[i].addr, RCx[j].bnk, npcam[i].bnk);
		}
	}
};

static void init() {
	memset(pivot_cover, -1, sizeof(pivot_cover));
	memset(nonpivot_cover_info, false, sizeof(nonpivot_cover_info));
}

extern void show_nonpivot_cover() {
	cout << "nonpivot_cover_info : ";
	for (int i = 0; i < NPCAM_SIZE; i++) {
		if(npcam[i].en)
			cout << nonpivot_cover_info[i];
	}
}

void spare_allocation() {
	if (!signal_valid())
		return;
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
	analyzer.comapare_row(npcam);
	analyzer.comapare_col(npcam);
}

#endif // !__SPAREALLOCATIONANALYZER_H
