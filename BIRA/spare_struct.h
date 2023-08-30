#ifndef __SPARESTRUCT_H
#define __SPARESTRUCT_H
#include <iostream>
#include "header.h"

struct Spare {
	int addr;
	int bnk;
	bool rc;
	bool alloc;
};

struct Bank {
	int spares[4];
	Bank() {
		memset(spares, 0, sizeof(spares));
	}
};

/*
	struct1 : Sslc 2 + 2 / Sslr 2 + 2
	struct2 : Sslc 1 + 1 / Sslr 2 + 2 / Sscc 2
	struct3 : Sslc 1 + 1 / Sslr 1 + 1 / Sscc 2 / Sdgr 1

	Sslr	2	2	1
	Sslc	2	1	1
	Sscc		1	1
	Sdgr			1
*/


// spare : range, direction
struct SpareStruct {
	Bank bank[2];
	SpareStruct(int x) {
		switch (x) {
		case 01 :
			// bank1
			bank[0].spares[0] = 2;	// Sslr
			bank[0].spares[1] = 2;	// Sslc
			// bank2
			bank[1].spares[0] = 2;	// Sslr
			bank[1].spares[1] = 2;	// Sslc
			break;

		case 02:
			// bank1
			bank[0].spares[0] = 2;	// Sslr
			bank[0].spares[1] = 1;	// Sslc
			bank[0].spares[2] = 2;	// Sscc
			// bank2
			bank[1].spares[0] = 2;	// Sslr
			bank[1].spares[1] = 1;	// Sslc
			bank[1].spares[2] = 0;	// Sscc
			break;

		case 03:
			// bank1
			bank[0].spares[0] = 1;	// Sslr
			bank[0].spares[1] = 1;	// Sslc
			bank[0].spares[2] = 2;	// Sscc
			bank[0].spares[3] = 1;	// Sdgr
			// bank2
			bank[1].spares[0] = 1;	// Sslr
			bank[1].spares[1] = 1;	// Sslc
			bank[1].spares[2] = 1;	// Sscc
			bank[1].spares[3] = 0;	// Sdgr
			break;
		}
	};
};


#endif // !STRUCT_TYPE