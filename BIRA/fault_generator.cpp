#include "header.h"

struct Fault {
	int bank;			// bank address
	int row_addr;		// row address
	int col_addr;		// col address
};

Fault faults[FAULT];

// reset signals and variables
static void init() {
	early_term = false;
	memset(mem, 0, sizeof(mem));
	memset(faults, 0, sizeof(faults));
}

// 1D fault address -> 2D fault address
static void fault_arrange() {
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

// to mark on Memory with the 2D fault address
static void fault_addr_convert(int* fault_addr) {
	int msb = 1 << (LEN * BNK);			// msb whether bank 1 (01) or bank 2 (10)
	int mask = SIZE;						
	mask--;									// mask = 111...111 with legth of LEN
	for (int i = 0; i < FAULT; i++) {
		// bank1
		if (msb & fault_addr[i]) {								// if msb is 1 = bank 2
			faults[i].bank = 0x2;								// extract bank address
			faults[i].col_addr = fault_addr[i] & mask;			// extract col address
			faults[i].row_addr = (fault_addr[i] >> LEN) & mask;	// extract row address
		}
		// bank0
		else {													// if msb is 1 = bank 2
			faults[i].bank = 0x1;								// extract bank address
			faults[i].col_addr = fault_addr[i] & mask;			// extract col address
			faults[i].row_addr = (fault_addr[i] >> LEN) & mask;	// extract row address
		}
	}
}

static void print_mem() {
	cout << "\tbank1:" << "\t\t\t" << "bank2:" << endl;
	for (int i = 0; i < SIZE; i++) {
		// print bank 1
		cout << 'r' << i << '\t';
		for (int j = 0; j < SIZE; j++) {
			cout << mem[0][i][j] << ' ';
		}
		cout << '\t';
		// print bank 2
		for (int j = 0; j < SIZE; j++) {
			cout << mem[1][i][j] << ' ';
		}
		cout << endl;
	}
	cout << endl;
}

static void show_faults() {
	cout << "fault address of " << int(SIZE) << " X " << int(SIZE) << ", " << BNK << " memory bank" << endl << endl << "\tbank\trow\tcol" << endl;
	for (int i = 0; i < FAULT; i++) {
		cout << '#' << i + 1 << " :" << '\t' << faults[i].bank << ",\t" << faults[i].row_addr << ",\t" << faults[i].col_addr << endl;
	}
	cout << endl;
}

static void generate_fault() {
	int fault_addr[FAULT];
	int cnt = 0;
	int randnum = 0;
	int i = 0;

	// generate random number with 0 ~ 1111....1111 = bank row * bank col * #bank : (LEN + LEN + 1) 1's
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int> dis(0, (SIZE * SIZE * BNK) - 1);
	memset(fault_addr, 0, sizeof(fault_addr));

	// generate random number (#randnum = FAULT(# total fault))
	while (cnt < FAULT) {
		// extract random number
		randnum = dis(gen);
		// check if the random number is already used
		for (i = 0; i <= cnt; i++) {
			if (fault_addr[i] == randnum) {
				cnt--;
				break;
			}
			if (i == cnt)
				fault_addr[cnt] = randnum;
		}
		cnt++;
	}

	fault_addr_convert(fault_addr);
	fault_arrange();
}


static void read_fault_file() {
	for (int i = 0; i < SIZE; i++) {
		for (int k = 0; k < BNK; k++) {
			for (int j = 0; j < SIZE; j++) {
				cin >> mem[k][i][j];
			}
		}
	}
}

extern void fault_generation() {
	init();
	//freopen("memory.txt", "r", stdin);		// read 'input.txt' file
	//read_fault_file();
	generate_fault();
	show_faults();

	if(SIZE < 50)
		print_mem();
}