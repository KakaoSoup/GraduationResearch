#include "header.h"
#include "spare_struct.h"

bool uncover_must_pivot[PCAM_SIZE] = { 0 };
bool unused_spare[PCAM_SIZE] = { 0 };

bool signal_valid(const int spare_struct, const int pivot_block[PCAM_SIZE], const int must_repair[PCAM_SIZE], const bool RLSS[R_SPARE], const bool DSSS[PCAM_SIZE]) {
	int cnt_col[2] = { 0 };
	int cnt_row[2] = { 0 };
	int rlss_idx = 0;
	int wide_idx = -1;
	bool temp = 0;

	for (int i = 0; i < PCAM_SIZE; i++) {
		if (DSSS[i] == ROW) {
			if (RLSS[rlss_idx++])
				wide_idx = i;
		}
		if (must_repair[i]) {
			switch (must_repair[i]) {
			case 0x4:	// row must
				if (DSSS[i] != ROW)
					uncover_must_pivot[i] = true;
				cnt_row[pivot_block[i] - 1]++;
				break;
			case 0x2:	// col must
				if (DSSS[i] != COL)
					uncover_must_pivot[i] = true;
				cnt_col[pivot_block[i] - 1]++;
				break;
			case 0x1:	// adj must			adjacent must일 경우, RLSS는 무조건 1이 되어야 하는가?
				if (DSSS[i] != ROW && i != wide_idx)
					uncover_must_pivot[i] = true;
				// temp = pivot_block[i] - 1;
				// cnt_row[!temp]++;
				break;
			default:
				break;
			}
		}
		else
			unused_spare[i] = true;			// must_cover_pivot이 아니어서 unused = 1
	}

	SpareStruct strct(spare_struct); 
	switch (spare_struct) {
	case S1 :
		if (cnt_row[0] > strct.bank[0].spares[0] || cnt_row[1] > strct.bank[1].spares[0])
			return false;
		if (cnt_col[0] > strct.bank[0].spares[1] || cnt_col[1] > strct.bank[1].spares[1])
			return false;
		break;
	case S2 :
		if (cnt_row[0] > strct.bank[0].spares[0])
			return false;
		if(cnt_row[1] > strct.bank[1].spares[0])
			return false;
		if (cnt_col[0] > strct.bank[0].spares[1]) {
			if (cnt_col[0] - strct.bank[0].spares[1] > strct.bank[0].spares[2])
				return false;
			else
				strct.bank[0].spares[2] -= (cnt_col[0] - strct.bank[0].spares[1]);
		}
		if(cnt_col[1] > strct.bank[1].spares[1]) {
			if (cnt_col[0] - strct.bank[1].spares[1] > strct.bank[0].spares[2])
				return false;
		}
		break;
	case S3 :
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

	return true;
}