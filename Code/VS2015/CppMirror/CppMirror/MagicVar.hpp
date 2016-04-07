#include <string>
#include <iostream>

template<class T>
struct MagicTypeIndex {};

#define DEFINE_MAGIC_TYPE(x, i) \
template <>	\
struct MagicTypeIndex<x>	\
{						\
	static const int index = i;	\
};

DEFINE_MAGIC_TYPE(int, 0)
DEFINE_MAGIC_TYPE(float, 1)
DEFINE_MAGIC_TYPE(double, 2)
DEFINE_MAGIC_TYPE(std::string, 3)

struct magicVar {
	union {
		int intV;
		float floatV;
		double doubleV;
		std::string* stringV;
	} value;

	enum type
	{
		int_ = MagicTypeIndex<int>::index,
		float_ = MagicTypeIndex<float>::index,
		double_ = MagicTypeIndex<double>::index,
		string_ = MagicTypeIndex<std::string>::index,
	} type;
};

template <class T>
magicVar Transform(const T var)
{
	return magicVar();
};

template <>
magicVar Transform<std::string>(std::string var)
{
	magicVar mv;
	mv.type = mv.string_;
	mv.value.stringV = new std::string(var);
	return mv;
}

template <>
magicVar Transform<int>(int var)
{
	magicVar mv;
	mv.type = mv.int_;
	mv.value.intV = var;
	return mv;
}

template <class T>
struct TransformHelper {
	static T Transform(magicVar var) {
		return 0;
	}
};

template <>
struct TransformHelper<int>{
	static int Transform(magicVar var) {
		return var.value.intV;
	}
};

void printVar(magicVar var)
{
	switch (var.type)
	{
	case magicVar::int_:
		std::cout << var.value.intV << std::endl;
		break;
	case magicVar::float_:
		std::cout << var.value.floatV << std::endl;
		break;
	case magicVar::double_:
		std::cout << var.value.doubleV << std::endl;
		break;
	case magicVar::string_:
		std::cout << *var.value.stringV << std::endl;
		break;
	default:
		std::cout << "Error!" << std::endl;
		break;
	}
}