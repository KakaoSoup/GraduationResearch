#ifndef __SIGNALVALIDITYCHECKER_H
#define __SIGNALVALIDITYCHECKER_H
#include "spare_struct.h"
#include "singal_generator.h"
#include "CAM.h"

extern int pivot_block[PCAM_SIZE];
extern int must_repair[PCAM_SIZE];
extern bool uncover_must_pivot[PCAM_SIZE];
extern bool unused_spare[R_SPARE + C_SPARE];

// if DSSS and RLSS is valid -> True
// valid : 1) Signal covered all must repair fault line, 2) Spares at block doesn't exceed the allocated number of spares  
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
		fail = false;

		// find the double signal of row spare
		if (DSSS[i] == ROW) {
			if (RLSS[rlss_idx++]) {
				//wide_idx = i;
				if (STRUCT_TYPE == S3) {

				}
			}
		}

		if (must_repair[i]) {
			switch (must_repair[i]) {
			// row must repair
			case 0x4:
				if (DSSS[i] != ROW) {
					fail = true;
					uncover_must_pivot[i] = true;
					continue;
				}
				switch (STRUCT_TYPE) {
				// spare structure 1 with row must flag
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
				// spare structure 2 with row must flag
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
				// spare structure 3 with row must flag
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

			// col must repair
			case 0x2:
				if (DSSS[i] != COL) {
					fail = true;
					uncover_must_pivot[i] = true;
					continue;
				}

				switch (STRUCT_TYPE) {
				// spare structure 1 with col must flag
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

				// spare structure 2 with col must flag
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

				// spare structure 3 with col must flag
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

			// row adjacent must flag
			case 0x1:	// adj must, adjacent must일 경우, RLSS는 무조건 1이 되어야 하는가?
				if (DSSS[i] != ROW) {
					fail = true;
					uncover_must_pivot[i] = true;
					continue;
				}

				switch (STRUCT_TYPE) {
				// spare structure 1 with row adjacent must flag
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

				// spare structure 2 with row adjacent must flag
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
					break;

				// spare structure 3 with row adjacent must flag
				case S3:
					if (pivot_block[i] == 1) {
						if (unused_spare[1])
							unused_spare[1] = false;
						else if (unused_spare[2])
							unused_spare[2] = false;
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
				default:
					break;
				}
				break;
			default:
				break;
			}
		}

		if(fail)
			uncover_must_pivot[i] = true;
	}

	if (fail)
		return false;

	return true;
}


#endif // !__SIGNALVALIDITYCHECKER_H