#include <boost/preprocessor.hpp>
#include <iostream>
#include <string>

#include "BottledType.hpp"

struct Bottle;


template<class T>
struct BottleLabel {};


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

#define MAKE_BOTTLE_LABEL_FOR_EACH(r, data, i, x) \
MAKE_BOTTLE_LABEL(x, i)

BOOST_PP_SEQ_FOR_EACH_I(MAKE_BOTTLE_LABEL_FOR_EACH, data, BOOST_PP_TUPLE_TO_SEQ(BOTTLED_TYPE))

#define MAKE_VALUE(x) \
x x##V;

#define MAKE_LABEL(x) \
label_##x = BottleLabel<x>::label,

#define MAKE_VALUE_FOR_EACH(r, data, i, x) \
MAKE_VALUE(x)

#define MAKE_LABEL_FOR_EACH(r, data, i, x) \
MAKE_LABEL(x)

struct Bottle {
	union {
		BOOST_PP_SEQ_FOR_EACH_I(MAKE_VALUE_FOR_EACH, data, BOOST_PP_TUPLE_TO_SEQ(BOTTLED_TYPE))
	} value;

	enum
	{
		BOOST_PP_SEQ_FOR_EACH_I(MAKE_LABEL_FOR_EACH, data, BOOST_PP_TUPLE_TO_SEQ(BOTTLED_TYPE))
	} label;
};

#define MAKE_STD_COUT_OPERATOR(x) \
case BottleLabel<x>::label: \
	std::cout<<bottle.value.x##V; \
	break; \

#define MAKE_STD_COUT_OPERATOR_FOR_EACH(r, data, i, x) \
MAKE_STD_COUT_OPERATOR(x)

std::ostream& operator<<(std::ostream& out, const Bottle& bottle)
{
	switch (bottle.label)
	{
		BOOST_PP_SEQ_FOR_EACH_I(MAKE_STD_COUT_OPERATOR_FOR_EACH, data, BOOST_PP_TUPLE_TO_SEQ(BOTTLED_TYPE))
	}
	return out;
}

#define MAKE_FUNNEL_FOR_EACH(r, data, i, x) \
MAKE_FUNNEL(x, i)

#define MAKE_FUNNEMAKE_BOTTLE_UP_FOR_EACH(r, data, i, x) \
MAKE_BOTTLE_UP(x, i)

BOOST_PP_SEQ_FOR_EACH_I(MAKE_FUNNEL_FOR_EACH, data, BOOST_PP_TUPLE_TO_SEQ(BOTTLED_TYPE))

BOOST_PP_SEQ_FOR_EACH_I(MAKE_FUNNEMAKE_BOTTLE_UP_FOR_EACH, data, BOOST_PP_TUPLE_TO_SEQ(BOTTLED_TYPE))
