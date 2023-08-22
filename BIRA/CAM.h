#ifndef CAM_H
#define CAM_H
#include "header.h"
#include "spare_struct.h"
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

	<NPCAM>
	enable flag :	1 bit
	Pivot CAM ptr :	#row + col spares
	R/C descripotr:	1 bit
	r or c addr :	3 bits
	bank addr :		2 bits
	1 + 4 + 1 + 10 + 2 = 18 bits
*/

extern int pcamCnt;
extern int npcamCnt;
extern void store_CAM();

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
public:
	void init() {
		row_len = LEN;
		col_len = LEN;
		bnk_len = BNK;
		this->pcam_cnt = 0;
	}
	CamStruct() {
		init();
	}
	void set_flag(const int idx, const int type) {
		switch (type) {
		case 1:
			pcam[idx].must_repair |= 0x4;
			break;
		case 2:
			pcam[idx].must_repair |= 0x2;
			break;
		case 3:
			pcam[idx].must_repair |= 0x1;
			break;
		}
	}
	void setNpcam(const int ptr, const bool rc, const int addr, const int bnk) {
		int cnt = 0;
		bool find = false;
		Npcam* nptr = nullptr;
	
		for (int i = 0; i < NPCAM_SIZE; i++) {
			if (npcam[i].en) {
				if (npcam[i].pcam_ptr == ptr && npcam[i].rc == rc) {
					cnt++;
				}
			}
			else if (!find) {
				nptr = &npcam[i];
				find = true;
			}
		}
		if (cnt > C_SPARE - 1 && rc == COL) {
			if (pcam[ptr].bnk_addr == bnk)
				set_flag(ptr, 1);
			else
				set_flag(ptr, 3);
			return;
		}
		if (cnt > R_SPARE - 1 && rc == ROW) {
			set_flag(ptr, 2);
			return;
		}

		if (nptr != nullptr) {
			nptr->en = true;
			nptr->pcam_ptr = ptr;
			nptr->rc = rc;
			nptr->addr = addr;
			nptr->bnk = bnk;
		}
	}
	void setCam(const int row, const int col, const int bnk) {
		for (int idx = 0; idx < this->pcam_cnt; idx++) {
			if (pcam[idx].row_addr == row) {
				setNpcam(idx, COL, col, bnk);
				return;
			}
			else if (pcam[idx].col_addr == col) {
				setNpcam(idx, ROW, row, bnk);
				return;
			}
		}
		pcam[this->pcam_cnt].en = true;
		pcam[this->pcam_cnt].row_addr = row;
		pcam[this->pcam_cnt].col_addr = col;
		pcam[this->pcam_cnt++].bnk_addr = bnk;
		pcamCnt = this->pcam_cnt;
	}
	void showPcam() {
		cout << "PCAM(en/row/col/bnk/must) : " << endl;
		for (int i = 0; i < this->pcam_cnt; i++) {
			if (pcam[i].en) {
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
	void showNpcam() {
		cout << "NPCAM(en/ptr/rc/addr/bnk) : " << endl;;
		for (int i = 0; i < NPCAM_SIZE; i++) {
			if (npcam[i].en) {
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
	int rtn_Pvblock(int idx) {
		return pcam[idx].bnk_addr;
	}
	int rtn_must(int idx) {
		return pcam[idx].must_repair;
	}
};

#endif // !CAM_H

