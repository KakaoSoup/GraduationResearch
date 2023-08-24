#ifndef __SIGNALGENERATOR_H
#define __SIGNALGENERATOR_H
#include "header.h"
#include "spare_struct.h"

extern bool DSSS[R_SPARE + C_SPARE];
extern bool RLSS[R_SPARE-1];
extern void singal_generate();


// Module Signal Generator
class SignalGenerator {
private:
	// True if RLSS is generating
	bool rlss_run;
public:
	SignalGenerator() {
		rlss_run = 0;
	}
	// set 1 to DSSS with indexes 
	void set_dsss(int i, int j, int k, int p) {
		DSSS[i] = true;
		DSSS[j] = true;
		DSSS[k] = true;
		if(STRUCT_TYPE != S3)
			DSSS[p] = true;
	}
	// set 1 to RLSS with index
	void set_rlss(int i) {
		RLSS[i] = true;
	}
	// generate RLSS signal by one clk
	void RLSS_generator() {
		static int ri = 0;
		memset(RLSS, 0, sizeof(RLSS));
		set_rlss(ri);
		if (ri < R_SPARE - 2) {
			ri++;
			this->rlss_run = true;
		}
		else {
			ri = 0;
			this->rlss_run = false;
		}
	}
	// genrate DSSS signal with one clk
	void DSSS_genearator(bool hold) {
		static int i = 0;
		static int j = 1;
		static int k = 2;
		static int p = 3;
		memset(DSSS, 0, sizeof(DSSS));
		set_dsss(i, j, k, p);

		// hold -> true : RLSS is run and DSSS is updated with 3 clk
		if (!hold) {
			if (p < R_SPARE + C_SPARE - 1 && STRUCT_TYPE != S3)
				p++;
			else if (k < R_SPARE + C_SPARE - 2) {
				k++;
				p = k + 1;
			}
			else if (j < R_SPARE + C_SPARE - 3) {
				j++;
				k = j + 1;
				p = k + 1;
			}
			else if (i < R_SPARE + C_SPARE - 4) {
				i++;
				j = i + 1;
				k = j + 1;
				p = k + 1;
			}
			else {
				i = 0;
				j = 1;
				k = 2;
				p = 3;
			}
		}
	}
	// show RLSS signal
	void show_rlss() {
		cout << "RLSS=";
		const int len = (STRUCT_TYPE != S3) ? R_SPARE : R_SPARE - 1;
		for (int i = 0; i < len; i++) {
			if (RLSS[i])
				cout << '1';
			else
				cout << '0';
		}
	}
	// show DSSS signal
	void show_dsss() {
		cout << "DSSS=";
		for (int i = 0; i < sig_len; i++) {
			if (DSSS[i])
				cout << '1';
			else
				cout << '0';
		}
	}
	// generate DSSS and RLSS with Spare Structure
	void signal_generate() {
		switch (STRUCT_TYPE) {
		case S3:
			RLSS_generator();
		case S1:
		case S2:
			DSSS_genearator(rlss_run);
			break;
		default:
			break;
		}
	}
};

#endif // !__SIGNALGENERATOR_H