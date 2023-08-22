#ifndef __SIGNALVALIDITYCHECKER_H
#define __SIGNALVALIDITYCHECKER_H
#include "spare_struct.h"
#include "singal_generator.h"
#include "CAM.h"

extern int pivot_block[PCAM_SIZE];
extern int must_repair[PCAM_SIZE];
extern bool uncover_must_pivot[PCAM_SIZE];
extern bool unused_spare[R_SPARE + C_SPARE];

bool signal_valid() {
	int cnt_col[2] = { 0 };
	int cnt_row[2] = { 0 };
	int rlss_idx = 0;
	int wide_idx = -1;
	bool temp = 0;
	bool fail = 0;
	memset(unused_spare, true, sizeof(unused_spare));
	memset(uncover_must_pivot, false, sizeof(uncover_must_pivot));

	for (int i = 0; i < pcamCnt; i++) {
		if (DSSS[i] == ROW) {
			if (RLSS[rlss_idx++])
				wide_idx = i;
		}
		if (must_repair[i]) {
			switch (must_repair[i]) {
			case 0x4:	// row must
				if (DSSS[i] != ROW) {
					uncover_must_pivot[i] = true;
					fail = true;
					continue;
				}
				//cnt_row[pivot_block[i] - 1]++;
				switch (struct_type) {
				case S1 :
					if (pivot_block[i] == 1) {
						if (unused_spare[0])
							unused_spare[0] = false;
						else if (unused_spare[2])
							unused_spare[2] = false;
						else
							fail = true;
					}
					else if (pivot_block[i] == 2) {
						if (unused_spare[1])
							unused_spare[1] = false;
						else if (unused_spare[3])
							unused_spare[3] = false;
						else
							fail = true;
					}
					break;
				case S2 :
					if (pivot_block[i] == 1) {
						if (unused_spare[0])
							unused_spare[0] = false;
						else if (unused_spare[2])
							unused_spare[2] = false;
						else
							fail = true;
					}
					else if (pivot_block[i] == 2) {
						if (unused_spare[1])
							unused_spare[1] = false;
						else if (unused_spare[3])
							unused_spare[3] = false;
						else
							fail = true;
					}
					break;
				case S3 :
					if (pivot_block[i] == 1) {
						if (unused_spare[0])			// Sslr : block1
							unused_spare[0] = false;
						else if (unused_spare[2])		// Sdgr
							unused_spare[2] = false;
						else
							fail = true;
					}
					else if (pivot_block[i] == 2) {
						if (unused_spare[1])			// Sslr : block2
							unused_spare[1] = false;
						else if (unused_spare[2])		// Sdgr
							unused_spare[2] = false;
						else
							fail = true;
					}
					break;
				default:
					break;
				}
				break;
			case 0x2:	// col must
				if (DSSS[i] != COL) {
					uncover_must_pivot[i] = true;
					fail = true;
					continue;
				}
				//cnt_col[pivot_block[i] - 1]++;
				switch (struct_type) {
				case S1:
					if (pivot_block[i] == 1) {
						if (unused_spare[4])
							unused_spare[4] = false;
						else if (unused_spare[6])
							unused_spare[6] = false;
						else
							fail = true;
					}
					else if (pivot_block[i] == 2) {
						if (unused_spare[5])
							unused_spare[5] = false;
						else if (unused_spare[7])
							unused_spare[7] = false;
						else
							fail = true;
					}
					break;
				case S2:
					if (pivot_block[i] == 1) {
						if (unused_spare[4])
							unused_spare[4] = false;
						else if (unused_spare[6])
							unused_spare[6] = false;
						else if (unused_spare[7])
							unused_spare[7] = false;
						else
							fail = true;
					}
					else if (pivot_block[i] == 2) {
						if (unused_spare[5])
							unused_spare[5] = false;
						else if (unused_spare[6])
							unused_spare[6] = false;
						else if (unused_spare[7])
							unused_spare[7] = false;
						else
							fail = true;
					}
					break;
				case S3:
					if (pivot_block[i] == 1) {
						if (unused_spare[4])
							unused_spare[4] = false;
						else if (unused_spare[6])
							unused_spare[6] = false;
						else if (unused_spare[7])
							unused_spare[7] = false;
						else
							fail = true;
					}
					else if (pivot_block[i] == 2) {
						if (unused_spare[5])
							unused_spare[5] = false;
						else if (unused_spare[6])
							unused_spare[6] = false;
						else if (unused_spare[7])
							unused_spare[7] = false;
						else
							fail = true;
					}
					break;
				default:
					break;
				}
				break;
			case 0x1:	// adj must			adjacent must일 경우, RLSS는 무조건 1이 되어야 하는가?
				/*
				if (DSSS[i] != ROW && i != wide_idx) {
					uncover_must_pivot[i] = true;
					fail = true;
				}
				*/
				//temp = pivot_block[i] - 1;
				//cnt_row[!temp]++;
				if (DSSS[i] != ROW) {
					uncover_must_pivot[i] = true;
					fail = true;
					continue;
				}
				switch (struct_type) {
				case S1:
					if (pivot_block[i] == 1) {
						if (unused_spare[1])		
							unused_spare[1] = false;
						else if (unused_spare[3])	
							unused_spare[3] = false;
						else						
							fail = true;
					}
					else if (pivot_block[i] == 2) {
						if (unused_spare[0])
							unused_spare[0] = false;
						else if (unused_spare[2])
							unused_spare[2] = false;
						else
							fail = true;
					}
					break;
				case S2:
					if (pivot_block[i] == 1) {
						if (unused_spare[1])
							unused_spare[1] = false;
						else if (unused_spare[3])
							unused_spare[3] = false;
						else
							fail = true;
					}
					else if (pivot_block[i] == 2) {
						if (unused_spare[0])
							unused_spare[0] = false;
						else if (unused_spare[2])
							unused_spare[2] = false;
						else
							fail = true;
					}
				case S3:
					if (pivot_block[i] == 1) {
						if (unused_spare[2])
							unused_spare[2] = false;
						if (unused_spare[1])
							unused_spare[1] = false;
						else
							fail = true;
					}
					else if (pivot_block[i] == 2) {
						if (unused_spare[2])
							unused_spare[2] = false;
						else if (unused_spare[0])
							unused_spare[0] = false;
						else
							fail = true;
					}
					break;
				default:
					break;
				}
				break;
			default:
				break;
			}
		}
	}
	if (fail)
		return false;
	return true;
}


#endif // !__SIGNALVALIDITYCHECKER_H


/*

SpareStruct strct(struct_type);
	switch (struct_type) {
	case S1:
		if (cnt_row[0] > strct.bank[0].spares[0] || cnt_row[1] > strct.bank[1].spares[0])
			return false;
		if (cnt_col[0] > strct.bank[0].spares[1] || cnt_col[1] > strct.bank[1].spares[1])
			return false;
		break;
	case S2:
		if (cnt_row[0] > strct.bank[0].spares[0])
			return false;
		if (cnt_row[1] > strct.bank[1].spares[0])
			return false;
		if (cnt_col[0] > strct.bank[0].spares[1]) {
			if (cnt_col[0] - strct.bank[0].spares[1] > strct.bank[0].spares[2])
				return false;
			else
				strct.bank[0].spares[2] -= (cnt_col[0] - strct.bank[0].spares[1]);
		}
		if (cnt_col[1] > strct.bank[1].spares[1]) {
			if (cnt_col[0] - strct.bank[1].spares[1] > strct.bank[0].spares[2])
				return false;
		}
		break;
	case S3:
		if (wide_idx > 0) {
			if (cnt_row[0] > strct.bank[0].spares[0])
				return false;
			if (cnt_row[1] > strct.bank[1].spares[0])
				return false;
		}
		else {
			if (cnt_row[0] > strct.bank[0].spares[0] || cnt_row[1] > strct.bank[1].spares[0]) {
				if (cnt_row[0] + cnt_row[1] > (strct.bank[0].spares[0] + strct.bank[1].spares[0] + strct.bank[0].spares[3]))
					return false;
			}
		}
		if (cnt_col[0] > strct.bank[0].spares[1]) {
			if (cnt_col[0] - strct.bank[0].spares[1] > strct.bank[0].spares[2])
				return false;
			else
				strct.bank[0].spares[2] -= (cnt_col[0] - strct.bank[0].spares[1]);
		}
		if (cnt_col[1] > strct.bank[1].spares[1]) {
			if (cnt_col[0] - strct.bank[1].spares[1] > strct.bank[0].spares[2])
				return false;
		}
		break;
	default:
		break;
	}

*/