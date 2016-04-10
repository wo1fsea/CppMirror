#include <boost/preprocessor.hpp>
#include <iostream>
#include <string>

#include "BottledType.hpp"

template<class T>
struct BottleLabel {};


struct Bottle;


template <class T>
Bottle BottleUp(const T var)
{
	return Bottle();
};


template <class T>
struct Funnel {
	static T Pour(Bottle var) {
		return 0;
	}
};


#define MAKE_BOTTLE_LABEL(x, i) \
template <> \
struct BottleLabel<x> \
{ \
	static const int label = i; \
}; \


#define MAKE_BOTTLE_UP(x, i) \
template <> \
Bottle BottleUp<x>(x var) \
{ \
	Bottle bottle; \
	bottle.label = bottle.label_##x; \
	bottle.value.x##V = var; \
	return bottle; \
} \


#define MAKE_FUNNEL(x, i)	\
template <>	\
struct Funnel<x> {	\
	static x Pour(Bottle bottle) {	\
		return bottle.value.x##V;	\
	}	\
};	\

#define MAKE_BOTTLE_LABEL_EACH(r, data, i, x) \
MAKE_BOTTLE_LABEL(x, i)

BOOST_PP_SEQ_FOR_EACH_I(MAKE_BOTTLE_LABEL_EACH, data, BOOST_PP_VARIADIC_TO_SEQ BOTTLED_TYPE)
//MAKE_BOTTLE_LABEL(int, 0)

#define MAKE1(x) \
x x##V;

#define MAKE1_EACH(r, data, i, x) \
MAKE1(x)

#define MAKE2_EACH(r, data, i, x) \
MAKE2(x)

#define MAKE2(x) \
label_##x = BottleLabel<x>::label,

struct Bottle {
	union {
		BOOST_PP_SEQ_FOR_EACH_I(MAKE1_EACH, data, BOOST_PP_VARIADIC_TO_SEQ BOTTLED_TYPE)
	} value;

	enum
	{
		BOOST_PP_SEQ_FOR_EACH_I(MAKE2_EACH, data, BOOST_PP_VARIADIC_TO_SEQ BOTTLED_TYPE)
	} label;
};


std::ostream& operator<<(std::ostream& out, const Bottle& var)
{
	out << "ERROR";
	return out;
}

#define MAKE_FUNNEL_EACH(r, data, i, x) \
MAKE_FUNNEL(x, i)

#define MAKE_FUNNEMAKE_BOTTLE_UP_EACH(r, data, i, x) \
MAKE_BOTTLE_UP(x, i)

BOOST_PP_SEQ_FOR_EACH_I(MAKE_FUNNEL_EACH, data, BOOST_PP_VARIADIC_TO_SEQ BOTTLED_TYPE)


BOOST_PP_SEQ_FOR_EACH_I(MAKE_FUNNEMAKE_BOTTLE_UP_EACH, data, BOOST_PP_VARIADIC_TO_SEQ BOTTLED_TYPE)


