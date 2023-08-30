#ifndef __SIGNALVALIDITYCHECKER_H
#define __SIGNALVALIDITYCHECKER_H
#include "header.h"
#include "spare_struct.h"

extern bool unused_spare[PCAM_SIZE];
extern bool uncover_must_pivot[PCAM_SIZE];
extern bool valid_signal;

static bool check_rlss(const int spr_struct) {
	int cnt = 0;
	for (int i = 0; i < R_SPARE; i++)
		if (RLSS[i])
			cnt++;

	switch (spr_struct) {
	case S1:
	case S2:
		if (cnt != 0)
			return false;
		break;
	case S3:
		if (cnt != 1)
			return false;
		break;
	default:
		break;
	}
	return true;
}

static bool check_dsss() {
	int cnt = 0;
	for (int i = 0; i < PCAM_SIZE; i++) {
		if (DSSS[i])
			cnt++;
	}
	if (cnt == R_SPARE)
		return true;
	else
		return false;
}

// 1. are the numbers of spares in block over?
// 2. are must-repair lines covered?
extern bool signal_validity_checker(const int spr_struct) {
	if (!check_dsss() || !check_rlss(spr_struct)) {
		cout << "singal is invalid\n";
		return false;
	}

	return true;
}


#endif // !__SIGNALVALIDITYCHECKER
