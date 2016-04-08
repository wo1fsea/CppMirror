#include <string>
#include <iostream>

template<class T>
struct BottleLabel {};

#define MAKE_BOTTLE_LABEL(x, i) \
template <>	\
struct BottleLabel<x>	\
{						\
	static const int label = i;	\
};

MAKE_BOTTLE_LABEL(int, 0)
MAKE_BOTTLE_LABEL(float, 1)
MAKE_BOTTLE_LABEL(double, 2)
typedef std::string string;
MAKE_BOTTLE_LABEL(string, 3)

struct Bottle {
	union {
		// auto gen
		int intV;
		float floatV;
		double doubleV;
		std::string* stringV;
	} value;

	enum 
	{
		// auto gen
		label_int = BottleLabel<int>::label,
		label_float = BottleLabel<float>::label,
		label_double = BottleLabel<double>::label,
		label_string = BottleLabel<std::string>::label,
	} label;
};

template <class T>
Bottle BottleUp(const T var)
{
	return Bottle();
};

// auto gen
template <>
Bottle BottleUp<int>(int var)
{
	Bottle mv;
	mv.label = mv.label_int;
	mv.value.intV = var;
	return mv;
}

template <class T>
struct Funnel {
	static T Pour(Bottle var) {
		return 0;
	}
};

// auto gen
template <>
struct Funnel<int>{
	static int Pour(Bottle var) {
		return var.value.intV;
	}
};

void printVar(Bottle var)
{
	switch (var.label)
	{
	case Bottle::label_int:
		std::cout << var.value.intV << std::endl;
		break;
	case Bottle::label_float:
		std::cout << var.value.floatV << std::endl;
		break;
	case Bottle::label_double:
		std::cout << var.value.doubleV << std::endl;
		break;
	case Bottle::label_string:
		std::cout << *var.value.stringV << std::endl;
		break;
	default:
		std::cout << "Error!" << std::endl;
		break;
	}
}