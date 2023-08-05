#ifndef CAM_H
#define CAM_H
#include "header.h"
#include "spare_struct.h"
#include <cmath>
#define MUST_FLAG 4
#define PCAM (R_SPARE + C_SPARE)
#define NPCAM (R_SPARE + C_SPARE)

class Cam {
protected:
	int row_len;
	int col_len;
	int bnk_len;
	unsigned int mask = (1 << LEN) - 1;
	unsigned int bnk_mask = (1 << BANK) - 1;
public:
	Cam() {
		row_len = LEN;
		col_len = LEN;
		bnk_len = BANK;
	}
};

/*
	<PCAM>
	enable flag :	1 bit
	row addr :		10 bits
	col addr :		10 bits
	bank addr :		2 bits
	row must :		1 bit
	col must :		1 bit
	adj row must :	1 bit
	adj col must :	1 bit
	1 + 10 + 10 + 2 + 1 + 1 + 1 + 1 = 27 bits
*/
class Pcam : private Cam {
private:
	int cnt;
	int bnk_offset;
	int col_offset;
	int row_offset;
	int en_offset;
	unsigned cam[PCAM];
public:
	void reset() {
		cnt = 0;
		memset(cam, '\0', sizeof(cam));
		bnk_offset = MUST_FLAG;
		col_offset = bnk_offset + bnk_len;
		row_offset = col_offset + col_len;
		en_offset = row_offset + row_len;
	}
	Pcam() {
		reset();
	}
	bool en(int i) {
		return (cam[i] >> en_offset);
	}
	void set_pcam(int r, int c, int b) {
		if (cnt < PCAM && !en(cnt)) {
			cam[cnt] |= 1 << en_offset;
			cam[cnt] |= (r & mask) << row_offset;
			cam[cnt] |= (c & mask) << col_offset;
			cam[cnt++] |= (b & bnk_mask) << bnk_offset;
		}
		else if (cnt >= PCAM) {	// if #faults > #total_spares
			early_term = true;		// generate early_term signal
			cout << "Alert : early term signal generated!\n";
		}
	}
	void set_must_flag(int i, int f) {
		switch (f) {
		case 0:	// row must
			cam[i] |= 0x8;
		case 1:	// col must
			cam[i] |= 0x4;
		case 2: // adj row must
			cam[i] |= 0x2;
		case 3: // adj col must
			cam[i] |= 0x1;
		}
	}
	int pcam_cnt() {
		return cnt;
	}
	int r_addr(int i) {
		return (cam[i] >> row_offset) & mask;
	}
	int c_addr(int i) {
		return (cam[i] >> col_offset) & mask;
	}
	int b_addr(int i) {
		return (cam[i] >> bnk_offset) & bnk_mask;
	}
	int must_flag(int i) {
		return (cam[i] & 0xF);
	}
	unsigned show_ith_cam(int i) {
		return cam[i];
	}
	void binary_exp(uint64_t bin) {
		int cnt = 64;
		int first = 0;
		bool check = 0;
		uint64_t pivot = 1ULL << 63;

		while (cnt) {
			// skip 0's over the highest bit of binary number
			if (!check) {
				if (!(bin & pivot))
					pivot >>= 1;
				else {
					check = true;
					first = cnt;
					cnt++;
				}
			}
			// express binary number
			else {
				if ((cnt == en_offset || cnt == row_offset || cnt == col_offset || cnt == bnk_offset) && cnt != first)
					cout << "_";
				if (bin & pivot)
					cout << 1;
				else
					cout << 0;
				pivot >>= 1;
			}
			cnt--;
		}
		cout << "(2)" << endl;
	}
	void show_pcam() {
		cout << endl;
		int cnt = pcam_cnt();
		for (int i = 0; i < cnt; i++) {
			cout << '#' << i << " pcam : ";
			binary_exp(show_ith_cam(i));
		}
		cout << endl;
	}
};

/*
	<NPCAM>
	enable flag :	1 bit
	Pivot CAM ptr :	#row + col spares
	R/C descripotr:	1 bit
	r or c addr :	10 bits
	bank addr :	2 bits
	1 + 4 + 1 + 10 + 2 = 18 bits
*/

class Npcam : private Cam {
private :
	unsigned cam[NPCAM];
	unsigned* camptr;
	int cnt;
	int en_offset;
	int ptr_offset;
	int ptr_len;
	int ptr_mask;
	int dscrpt_offset;
	int addr_offset;
public:
	void reset() {
		cnt = 0;
		camptr = cam;
		memset(cam, '\0', sizeof(cam));
	}
	Npcam() {
		addr_offset = BANK;
		reset();
		dscrpt_offset = addr_offset + LEN;
		ptr_offset = dscrpt_offset + 1;
		ptr_len = static_cast<int>(round(log2(PCAM)));
		ptr_mask = (1 << ptr_len) - 1;
		en_offset = ptr_offset + ptr_len;
	}
	bool en(unsigned cam) {
		return (cam >> en_offset) & 0x1;
	}
	bool full() {
		if (cnt >= NPCAM)
			return true;
		else
			return false;
	}
	void set_npcam(int ptr, bool rc, int addr, int bnk) {
		camptr = &cam[0];
		int Cnt = 0;
		bool over = false;
		if (cnt < NPCAM) {
			for (int i = 0; i < cnt; i++) {
				if (cam_ptr(*camptr) == ptr) {
					if (dsrpt(*camptr) == rc && bnk_addr(*camptr) == bnk) {
						Cnt++;
					}
				}
				if (Cnt >= R_SPARE-1 || Cnt >= C_SPARE-1) {
					over = true;
					break;
				}
				camptr++;
			}
			if (over) {
				camptr = &cam[0];
				for (int i = 0; i < cnt; i++) {
					if (cam_ptr(*camptr) == ptr) {
						if (dsrpt(*camptr) == rc && bnk_addr(*camptr) == bnk) {
							cout << "npcam, ";
							binary_exp(*camptr);
							cout << " is deleted." << endl;
							*camptr = 0;
						}
					}
					camptr++;
				}
			}
			else {
				*camptr |= 1 << en_offset;
				*camptr |= (ptr & ptr_mask) << ptr_offset;
				*camptr |= rc << dscrpt_offset;
				*camptr |= (addr & mask) << addr_offset;
				*camptr |= bnk & bnk_mask;
				camptr++;
				cnt++;
			}
		}
	}
	int npcam_cnt() {
		return cnt;
	}
	int cam_ptr(unsigned cam) {
		return (cam >> ptr_offset) & ptr_mask;
	}
	bool dsrpt(unsigned cam) {
		return (cam >> dscrpt_offset) & 0x1;
	}
	int addr(unsigned cam) {
		return (cam >> addr_offset) & mask;
	}
	int bnk_addr(unsigned cam) {
		return cam & bnk_mask;
	}
	unsigned show_ith_cam(int i) {
		return cam[i];
	}
	void binary_exp(uint64_t bin) {
		int cnt = 64;
		int first = 0;
		bool check = 0;
		uint64_t pivot = 1ULL << 63;

		while (cnt) {
			// skip 0's over the highest bit of binary number
			if (!check) {
				if (!(bin & pivot))
					pivot >>= 1;
				else {
					check = true;
					first = cnt;
					cnt++;
				}
			}
			// express binary number
			else {
				if ((cnt == en_offset || cnt == ptr_offset || cnt == dscrpt_offset || cnt == ptr_offset || cnt == addr_offset) && cnt != first)
					cout << "_";
				if (bin & pivot)
					cout << 1;
				else
					cout << 0;
				pivot >>= 1;
			}
			cnt--;
		}
		cout << "(2)";
	}
	void show_npcam() {
		int cnt = npcam_cnt();
		for (int i = 0; i < cnt; i++) {
			cout << '#' << i << " npcam : ";
			binary_exp(show_ith_cam(i));
			cout << endl;
		}
		cout << endl;
	}
};

extern Pcam pcam;
extern Npcam npcam;

#endif // !CAM_H

