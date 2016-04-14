#include <boost/preprocessor.hpp>
#include <iostream>
#include <string>

#include "BottledType.hpp"

struct Bottle;


template<class T>
struct BottleLabel {};


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


#define MAKE_FUNNEL(x, i)	\
template <>	\
struct Funnel<x> {	\
	static x Pour(Bottle bottle) {	\
		return *bottle.value.x##V;	\
	}	\
};	\

#define MAKE_BOTTLE_LABEL_FOR_EACH(r, data, i, x) \
MAKE_BOTTLE_LABEL(x, i)

BOOST_PP_SEQ_FOR_EACH_I(MAKE_BOTTLE_LABEL_FOR_EACH, data, BOOST_PP_TUPLE_TO_SEQ(BOTTLED_TYPE))

#define COPY_DATA(x) \
case label_##x: \
	this->value.x##V = new x(*bottle.value.x##V); \
	break;

#define MAKE_CONSTRUCTION(x) \
template<> \
Bottle(x value){ \
	this->value.x##V = new x(value); \
	this->label = label_##x; \
}

#define DELETE_DATA(x) \
case label_##x: \
	delete this->value.x##V; \
	break;

#define MAKE_VALUE(x) \
x* x##V;

#define MAKE_LABEL(x) \
label_##x = BottleLabel<x>::label,

#define COPY_DATA_FOR_EACH(r, data, i, x) \
COPY_DATA(x)

#define MAKE_CONSTRUCTION_FOR_EACH(r, data, i, x) \
MAKE_CONSTRUCTION(x) 

#define DELETE_DATA_FOR_EACH(r, data, i, x) \
DELETE_DATA(x) 

#define MAKE_VALUE_FOR_EACH(r, data, i, x) \
MAKE_VALUE(x)

#define MAKE_LABEL_FOR_EACH(r, data, i, x) \
MAKE_LABEL(x)

struct Bottle {
	Bottle(){
		this->value.intV = new int(0);
		this->label = label_int;
	}
	template<class T> 
	Bottle(T value) {}


	Bottle(const Bottle& bottle)
	{
		this->label = bottle.label;	
		switch (this->label)
		{
			BOOST_PP_SEQ_FOR_EACH_I(COPY_DATA_FOR_EACH, data, BOOST_PP_TUPLE_TO_SEQ(BOTTLED_TYPE))
		}
	}

	Bottle & operator = (const Bottle& bottle)
	{
		switch (this->label)
		{
			BOOST_PP_SEQ_FOR_EACH_I(DELETE_DATA_FOR_EACH, data, BOOST_PP_TUPLE_TO_SEQ(BOTTLED_TYPE))
		}

		this->label = bottle.label;
		switch (this->label)
		{
			BOOST_PP_SEQ_FOR_EACH_I(COPY_DATA_FOR_EACH, data, BOOST_PP_TUPLE_TO_SEQ(BOTTLED_TYPE))
		}
		return *this;
	}

	BOOST_PP_SEQ_FOR_EACH_I(MAKE_CONSTRUCTION_FOR_EACH, data, BOOST_PP_TUPLE_TO_SEQ(BOTTLED_TYPE))

	~Bottle() {
		switch (this->label)
		{
			BOOST_PP_SEQ_FOR_EACH_I(DELETE_DATA_FOR_EACH, data, BOOST_PP_TUPLE_TO_SEQ(BOTTLED_TYPE))
		}
	}
	
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
	std::cout<<*bottle.value.x##V; \
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

BOOST_PP_SEQ_FOR_EACH_I(MAKE_FUNNEL_FOR_EACH, data, BOOST_PP_TUPLE_TO_SEQ(BOTTLED_TYPE))

