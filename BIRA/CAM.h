#ifndef __CAM_H
#define __CAM_H
#include "header.h"
#include "spare_struct.h"
#define GLOBAL	0x4
#define COMMON	0x2
#define LOCAL	0x1

extern int pcamCnt;
extern int npcamCnt;
extern void store_CAM();

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
struct Pcam {
	bool en;
	int row_addr;
	int col_addr;
	int bnk_addr;
	int must_repair;

	Pcam() {
		en = false;
		row_addr = 0;
		col_addr = 0;
		bnk_addr = 0;
		must_repair = 0;
	}
};

/*
	<NPCAM>
	enable flag :	1 bit
	Pivot CAM ptr :	#row + col spares
	R/C descripotr:	1 bit
	r or c addr :	3 bits
	bank addr :		2 bits
	1 + 4 + 1 + 10 + 2 = 18 bits
*/
struct Npcam {
	bool en;
	int pcam_ptr;
	bool rc;
	int addr;
	int bnk;

	Npcam() {
		en = false;
		pcam_ptr = 0;
		rc = false;
		addr = 0;
		bnk = 0;
	}
};

extern Pcam pcam[PCAM_SIZE];
extern Npcam npcam[NPCAM_SIZE];

class CamStruct {
private:
	int row_len;
	int col_len;
	int bnk_len;
	int pcam_cnt;
	int pcam_size;
	int cnt_size[3];

public:
	// initialize variables
	void init() {
		row_len = LEN;
		col_len = LEN;
		bnk_len = BNK;
		this->pcam_cnt = 0;
		this->pcam_size = (STRUCT_TYPE != S3) ? PCAM_SIZE : PCAM_SIZE - 1;		
		cnt_size[0] = (STRUCT_TYPE != S1) ? 3 : 2;	// row must repair in same block
		cnt_size[1] = (STRUCT_TYPE != S3) ? 2 : 2;	// col must repair in same block
		cnt_size[2] = (STRUCT_TYPE != S1) ? 3 : 2;	// adj row must repair in same block
	}

	CamStruct() {
		init();
	}

	// set must flag with spare structure
	void set_flag(const int idx, const int type) {
		switch (type) {
		// row must flag
		case 1:
			pcam[idx].must_repair |= 0x4;
			break;
		// col must flag
		case 2:
			pcam[idx].must_repair |= 0x2;
			break;
		// row adjacent must flag
		case 3:
			pcam[idx].must_repair |= 0x1;
			break;
		}
	}

	// set NPCAM
	void setNpcam(const int ptr, const bool rowcol, const int addr, const int bnk) {
		int cnt[3] = { 0 };
		int row = 0, col = 0;
		bool find = false;
		Npcam* nptr = nullptr;
	
		// checks the number of NPCAMs shared address and finds the unactive NPCAM index
		for (int i = 0; i < NPCAM_SIZE; i++) {
			if (npcam[i].en) {
				// non-pivot fault share col address with pivot fault
				if (rowcol == ROW) {
					// col address of non-pivot fault
					col = (npcam[i].rc == COL) ? npcam[i].addr : pcam[npcam[i].pcam_ptr].col_addr;
					if (col == pcam[ptr].col_addr && npcam[i].bnk == pcam[ptr].bnk_addr)
						cnt[1]++;
				}
				// non-pivot fault share row address with pivot fault
				else {
					// row address of non-pivot fault
					row = (npcam[i].rc == ROW) ? npcam[i].addr : pcam[npcam[i].pcam_ptr].row_addr;
					if (row == pcam[ptr].row_addr) {
						if (npcam[i].bnk == pcam[ptr].bnk_addr)
							cnt[0]++;
						else
							cnt[2]++;
					}
				}

				/*
				if (npcam[i].pcam_ptr == ptr && npcam[i].rc == rowcol) {
					// npcam share with row 
					if (rowcol == COL) {
						// with same bnk
						if (bnk == npcam[i].bnk)
							cnt[0]++;
						// with adj bnk
						else
							cnt[2]++;
					}
					// npcam share with col
					else {
						cnt[1]++;
					}
				}
				*/
			}
			else if (!find) {
				nptr = &npcam[i];
				nptr->en = true;
				nptr->pcam_ptr = ptr;
				nptr->rc = rowcol;
				nptr->addr = addr;
				nptr->bnk = bnk;
				find = true;
				i--;
			}
		}

		// # of NPCAM overs the allocated number
		if (cnt[0]+1 > cnt_size[0]) {
			set_flag(ptr, 1);
			nptr->en = false;
			return;
		}
		else if (cnt[1]+1 > cnt_size[1]) {
			set_flag(ptr, 2);
			nptr->en = false;
			return;
		}
		else if (cnt[2] > cnt_size[2]) {
			set_flag(ptr, 3);
			nptr->en = false;
			return;
		}

		// there is no empty NPCAM
		if(!find)
			cout << "NPCAM is full!" << endl;
	}

	bool setCam(const int row, const int col, const int bnk) {
		for (int idx = 0; idx < this->pcam_cnt; idx++) {
			// new fault shares the address with already stored PCAM
			if (pcam[idx].row_addr == row) {
				setNpcam(idx, COL, col, bnk);
				return true;
			}
			else if (pcam[idx].col_addr == col && pcam[idx].bnk_addr == bnk) {
				setNpcam(idx, ROW, row, bnk);
				return true;
			}
		}

		// set PCAM
		if (this->pcam_cnt < this->pcam_size) {
			pcam[this->pcam_cnt].en = true;
			pcam[this->pcam_cnt].row_addr = row;
			pcam[this->pcam_cnt].col_addr = col;
			pcam[this->pcam_cnt].bnk_addr = bnk;
			pcamCnt = this->pcam_cnt;

			int cnt[3] = { 0 };
			for (int i = 0; i < NPCAM_SIZE; i++) {
				if (npcam[i].en) {
					// share with row address
					if (npcam[i].rc == ROW && npcam[i].addr == row) {
						if (npcam[i].bnk == bnk)
							cnt[0]++;
						else
							cnt[2]++;
					}
					// share with col address
					else if (npcam[i].rc == COL && npcam[i].addr == col && npcam[i].bnk == bnk) {
						cnt[1]++;
					}

				}
			}

			// # of NPCAM overs the allocated number
			if (cnt[0] + 1 > cnt_size[0])
				set_flag(this->pcam_cnt, 1);
			else if (cnt[1] + 1 > cnt_size[1])
				set_flag(this->pcam_cnt, 2);
			else if (cnt[2] > cnt_size[2])
				set_flag(this->pcam_cnt, 3);

			this->pcam_cnt++;
			return true;
		}
		else {
			cout << "# of fault over the PCAM size!!" << endl;
			return false;
		}
	}

	// show PCAMs
	void showPcam() {
		int idx = 0;
		cout << "PCAM(en/row/col/bnk/must) : " << endl;
		for (int i = 0; i < this->pcam_cnt; i++) {
			if (pcam[i].en) {
				cout << '#' << idx++ << '\t';
				cout << pcam[i].en << '_';
				cout << pcam[i].row_addr << '_';
				cout << pcam[i].col_addr << '_';
				cout << pcam[i].bnk_addr << '_';
				cout << pcam[i].must_repair;
				cout << endl;
			}
		}
		cout << endl;
	}

	// show NPCAMs
	void showNpcam() {
		int idx = 0;
		cout << "NPCAM(en/ptr/rc/addr/bnk) : " << endl;;
		for (int i = 0; i < NPCAM_SIZE; i++) {
			if (npcam[i].en) {
				cout << '#' << idx++ << '\t';
				cout << npcam[i].en << '_';
				cout << npcam[i].pcam_ptr << '_';
				cout << npcam[i].rc << '_';
				cout << npcam[i].addr << '_';
				cout << npcam[i].bnk;
				cout << endl;
			}
		}
		cout << endl;
	}

	// return PCAM bank address
	int rtn_pvblock(int idx) {
		return pcam[idx].bnk_addr;
	}

	// return PCAM must flags
	int rtn_must(int idx) {
		return pcam[idx].must_repair;
	}
};

#endif // !CAM_H

