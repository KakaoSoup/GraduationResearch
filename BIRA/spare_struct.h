#ifndef SPARE_STRUCT
#define SPARE_STRUCT
#include <iostream>
#define R_SPARE 4
#define C_SPARE 4

class Spare {
protected:
	bool dbl;
	bool rc;
};

class Local_spare : private Spare {


};

class Common_spare : private Spare {


};

class Global_spare : private Spare {


};

class spare_struct {
protected:
	int type[2][3][2]; // len, range, direct
public:
	spare_struct() {
		memset(type, 0, sizeof(type));
	}
};

class Struct1 : private spare_struct {
private:

public:
	Struct1() {
		type[0][0][0] = 2; // Sslr
		type[0][0][1] = 2; // Sslc
		type[1][2][0] = 2; // Sdgr
		type[0][1][1] = 2; // Sscc
	}
};

class Struct2 : private spare_struct {
private:

public:
	Struct2() {
		type[0][0][1] = 4; // Sslc
		type[1][2][0] = 4; // Sdgr
	}
};

class Struct3 : private spare_struct {
private:
public:
	Struct3() {
		type[0][0][1] = 2; // Sslc
		type[0][1][1] = 2; // Sscc
		type[0][2][0] = 4; // Ssgr
	}
};

#endif // !SPARE_STRUCT