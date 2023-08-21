#ifndef ANALYZER_H
#define ANALYZER_H
#include "CAM.h"
#include "spare_struct.h"

#endif


/*
	bool comparator(int idx, bool sig) {
		unsigned rc_addr;
		unsigned np_addr;
		unsigned rc_bnk;
		unsigned np_bnk;

		bool xnor1, xnor2, xnor3;
		xnor1 = (rc_addr == np_addr) ? true : false;

		// CAC
		if (descr) {
			xnor2 = (rc_addr & np_addr & 0x1) ? true : false;
			xnor3 = sig ? true : (rc_addr & np_addr & 0x2) ? true : false;
		}
		// RAC
		else {
			xnor2 = (rc_addr & np_addr & 0x2) ? true : false;
			xnor3 = sig ? true : (rc_addr & np_addr & 0x1) ? true : false;
		}

		return xnor1 && xnor2 && xnor3;
	}
	bool result() {
		if (!enable)
			return false;
		bool r = false;
		for (int i = 0; i < offset; i++) {
			r |= comparator(i, RLSS[i]);
		}
		return r;
	}
	*/