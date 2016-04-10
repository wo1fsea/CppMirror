typedef signed char signed_char;
typedef unsigned char unsigned_char;
typedef unsigned short unsigned_short;
typedef unsigned long unsigned_long;
typedef long double long_double;

#define BOTTLED_TYPE (\
	bool, \
	char, signed_char, unsigned_char, \
	wchar_t, \
	short, unsigned_short, \
	int, unsigned, \
	long, unsigned_long, \
	float, \
	double, \
	long_double \
)