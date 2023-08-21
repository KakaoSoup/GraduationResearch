#include "header.h"
#include "spare_struct.h"

bool DSSS[PCAM_SIZE];
bool RLSS[R_SPARE];

class SignalGenerator {
private:

public:
	static void set_dsss(int i, int j, int k, int p) {
		DSSS[i] = true;
		DSSS[j] = true;
		DSSS[k] = true;
		DSSS[p] = true;
	}
	static void set_rlss(int i, int j) {
		RLSS[i] = true;
		RLSS[j] = true;
	}
	static void RLSS_generator() {
		static int ri = 0;
		static int rj = 1;

		memset(RLSS, 0, sizeof(RLSS));
		set_rlss(ri, rj);
		if (rj < R_SPARE - 1)
			rj++;
		else if (ri < R_SPARE - 2) {
			ri++;
			rj = ri + 1;
		}
		else {
			ri = 0;
			rj = 1;
		}
	}
	static void DSSS_genearator(bool hold) {
		static int i = 0;
		static int j = 1;
		static int k = 2;
		static int p = 3;

		memset(DSSS, 0, sizeof(DSSS));
		set_dsss(i, j, k, p);

		if (!hold) {
			if (p < PCAM_SIZE - 1)
				p++;
			else if (k < PCAM_SIZE - 2) {
				k++;
				p = k + 1;
			}
			else if (j < PCAM_SIZE - 3) {
				j++;
				k = j + 1;
				p = k + 1;
			}
			else if (i < PCAM_SIZE - 4) {
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
	static void show_rlss() {
		cout << "RLSS : ";
		for (int i = 0; i < R_SPARE; i++) {
			if (RLSS[i])
				cout << '1';
			else
				cout << '0';
		}
		cout << endl;
	}

	static void show_dsss() {
		cout << "DSSS : ";
		for (int i = 0; i < PCAM_SIZE; i++) {
			if (DSSS[i])
				cout << '1';
			else
				cout << '0';
		}
		cout << endl;
	}
	void signal_generator(const int struct_type) {
		bool hold = 0;
		switch (struct_type) {
		case S3:
			for (int i = 0; i < 4; i++) {
				RLSS_generator();
				//show_rlss();
			}
		case S1:
		case S2:
			DSSS_genearator(hold);
			//show_dsss();
			break;
		default:
			break;
		}
	}
};

SignalGenerator generator;

extern void singal_generate(const int struct_type) {
	for (int i = 0; i < 70; i++) {
		generator.signal_generator(struct_type);
	}
}