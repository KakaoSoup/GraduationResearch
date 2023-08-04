#include "header.h"

void fault_arrange() {
	for (int i = 0; i < FAULT; i++) {
		// bank0
		if (faults[i].bank & 0x1) {
			mem[0][faults[i].row_addr][faults[i].col_addr] = true;
		}
		// bank1
		else {
			mem[1][faults[i].row_addr][faults[i].col_addr] = true;
		}
	}
}

void fault_addr_convert(int* fault_addr) {
	int msb = 1 << (LEN * BANK);
	int mask = SIZE;
	mask--;
	for (int i = 0; i < FAULT; i++) {
		// bank1
		if (msb & fault_addr[i]) {
			faults[i].bank = 0x2;
			faults[i].col_addr = fault_addr[i] & mask;
			faults[i].row_addr = (fault_addr[i] >> LEN) & mask;
		}
		// bank0
		else {
			faults[i].bank = 0x1;
			faults[i].col_addr = fault_addr[i] & mask;
			faults[i].row_addr = (fault_addr[i] >> LEN) & mask;
		}
	}
}

void print_mem() {
	cout << "\tbank1:" << "\t\t\t\t\t" << "bank2:" << endl;
	for (int i = 0; i < SIZE; i++) {
		cout << 'r' << i << '\t';
		for (int j = 0; j < SIZE; j++) {
			cout << mem[0][i][j] << ' ';
		}
		cout << '\t';
		for (int j = 0; j < SIZE; j++) {
			cout << mem[1][i][j] << ' ';
		}
		cout << endl;
	}
	cout << endl;
}

void show_faults() {
	cout << "fault address of " << int(SIZE) << " X " << int(SIZE) << ", " << BANK << " memory bank" << endl << endl << "\tbank\trow\tcol" << endl;
	for (int i = 0; i < FAULT; i++) {
		cout << '#' << i + 1 << " :" << '\t' << faults[i].bank << ",\t" << faults[i].row_addr << ",\t" << faults[i].col_addr << endl;
	}
	cout << endl;
}

void generate_fault() {
	int fault_addr[FAULT];
	int cnt = 0;
	int randnum = 0;
	int i = 0;

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int> dis(0, (SIZE * SIZE * BANK) - 1);
	memset(fault_addr, 0, sizeof(fault_addr));

	while (cnt < FAULT) {
		randnum = dis(gen);
		for (i = 0; i <= cnt; i++) {
			if (fault_addr[i] == randnum)
				break;
			if (i == cnt)
				fault_addr[cnt] = randnum;
		}
		cnt++;
	}
	fault_addr_convert(fault_addr);
	fault_arrange();
}