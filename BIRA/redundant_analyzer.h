#ifndef __REDUNDANTANALYZER_H
#define __REDUNDANTANALYZER_H
#include "header.h"
#include "signal_validity_checker.h"
#include "spare_allocation_analyzer.h"
#include "CAM.h"

struct Solution {
	int spare_type;
	bool rc;
	int bnk;
	int addr;
};

class RedundantAnalyzer {
private :
	Solution solution[PCAM_SIZE];
	int ridx;
	bool final_result;

public:
	RedundantAnalyzer() {
		memset(solution, 0, sizeof(solution));
		ridx = 0;
		final_result = true;
	}

	bool show_final_result() {
		for (int i = 0; i < NPCAM_SIZE; i++) {
			if (npcam[i].en) {
				final_result = final_result && nonpivot_cover_info[i];
			}
		}
		for (int i = 0; i < pcamCnt; i++) {
			if (DSSS[i]) {
				solution[i].rc = ROW;
				solution[i].addr = pcam[i].row_addr;
				solution[i].bnk = pcam[i].bnk_addr;
			}
			else {
				solution[i].rc = COL;
				solution[i].addr = pcam[i].col_addr;
				solution[i].bnk = pcam[i].bnk_addr;
			}
		}
		// all nonpivot faults are covered
		if (final_result) {
			return true;
		}
		return false;
	}

	Solution* rtn_solution(int idx) {
		if(idx < PCAM_SIZE)
			return &solution[idx];
	}
};


#endif // !__REDUNDANTANALYZER_H
