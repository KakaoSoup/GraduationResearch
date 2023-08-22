#ifndef __SPAREALLOCATIONANALYZER_H
#define __SPAREALLOCATIONANALYZER_H
#include "CAM.h"
#include "spare_struct.h"
#include "singal_generator.h"

extern int uncover_nonpivot_addr[NPCAM_SIZE];
extern Spare pivot_row_cover[R_SPARE];
extern Spare pivot_col_cover[C_SPARE];
extern bool nonpivot_cover_info[NPCAM_SIZE];
extern void spare_allocation();

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
		memcpy(RRx, pivot_row_cover, sizeof(pivot_row_cover));
		memcpy(RCx, pivot_col_cover, sizeof(pivot_col_cover));
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
	memset(pivot_row_cover, -1, sizeof(pivot_row_cover));
	memset(pivot_col_cover, -1, sizeof(pivot_col_cover));
	memset(nonpivot_cover_info, false, sizeof(nonpivot_cover_info));
}

static void show_nonpivot_cover() {
	for (int i = 0; i < NPCAM_SIZE; i++) {
		cout << nonpivot_cover_info[i];
	}
}

void spare_allocation() {
	init();
	SpareAllocationAnalyzer analyzer;
	int ridx = 0;
	int cidx = 0;
	for (int i = 0; i < pcamCnt; i++) {
		if (DSSS[i] == ROW && ridx < R_SPARE) {
			pivot_row_cover[ridx].rc = ROW;
			pivot_row_cover[ridx].addr = pcam[i].row_addr;
			pivot_row_cover[ridx++].bnk = pcam[i].bnk_addr;
		}
		else if (DSSS[i] == COL && cidx < C_SPARE) {
			pivot_col_cover[cidx].rc = COL;
			pivot_col_cover[cidx].addr = pcam[i].col_addr;
			pivot_col_cover[cidx++].bnk = pcam[i].bnk_addr;
		}
	}
	analyzer.set_repair_cand();
	analyzer.comapare_row(npcam);
	analyzer.comapare_col(npcam);
	cout << "nonpivot_cover_info : ";
	show_nonpivot_cover();
	cout << endl;
}

#endif // !__SPAREALLOCATIONANALYZER_H
