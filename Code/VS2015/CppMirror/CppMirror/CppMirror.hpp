#include <boost/preprocessor/stringize.hpp>
#include <boost/preprocessor/variadic/size.hpp>
#include <boost/preprocessor/seq/for_each_i.hpp>
#include <boost/preprocessor/variadic/to_seq.hpp>
#include <boost/type_traits.hpp>
#include <boost/mpl/range_c.hpp>
#include <boost/mpl/for_each.hpp>
#include <boost/bind.hpp>
#include <boost/mpl/string.hpp>  
#include <boost/functional/hash.hpp>
#include <iostream>

#include <boost/mpl/string.hpp>
#include <boost/mpl/fold.hpp>
#include <boost/mpl/size_t.hpp>
#include <cstddef>

#include <map>

#define REM(...) __VA_ARGS__
#define EAT(...)

#define DETAIL_TYPEOF_INT2(tuple) DETAIL_TYPEOF_HEAD tuple 
#define DETAIL_TYPEOF_INT(...) DETAIL_TYPEOF_INT2((__VA_ARGS__))

// Retrieve the type
#define TYPEOF(x) DETAIL_TYPEOF_INT(DETAIL_TYPEOF_PROBE x,)
#define DETAIL_TYPEOF(...) DETAIL_TYPEOF_HEAD(__VA_ARGS__)
#define DETAIL_TYPEOF_HEAD(x, ...) REM x
#define DETAIL_TYPEOF_PROBE(...) (__VA_ARGS__),
// Strip off the type
#define STRIP(x) EAT x
// Show the type without parenthesis
#define PAIR(x) REM x

// A helper metafunction for adding const to a type
template<class M, class T>
struct make_const
{
	typedef T type;
};

template<class M, class T>
struct make_const<const M, T>
{
	typedef typename boost::add_const<T>::type type;
};

#define REFLECT(x)	\
std::map<std::string, int> * x::_attrIndex = nullptr; \
bool x::_attrIndexInit = false;	

#define REFLECTABLE(...) \
static const int _lenIndex = BOOST_PP_VARIADIC_SIZE(__VA_ARGS__); \
static std::map<std::string, int> * _attrIndex; \
static bool _attrIndexInit;	\
static void GenAttrIndex(){	\
	_attrIndex = new std::map<std::string, int>(); \
	BOOST_PP_SEQ_FOR_EACH_I(REFLECT_EACH_GEN_GEN, data, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__)) \
	_attrIndexInit = true; \
	return; \
}	\
int SelAttr(const char* propName) \
{	\
	if(_attrIndexInit == false) \
		this->GenAttrIndex(); \
	std::map<std::string, int>::iterator it = _attrIndex->find(propName); \
	if (it != _attrIndex->end()) \
		return (*_attrIndex)[propName]; \
	else \
		return -1;	\
}	\
magicVar GetAttr(const char* propName) {	\
	int index = this->SelAttr(propName);	\
	magicVar mv;	\
	switch (index)	\
	{	\
		BOOST_PP_SEQ_FOR_EACH_I(REFLECT_EACH_GEN_GET, data, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__)) \
	}	\
	return mv; \
}	\
template<class T>	\
void SetAttr(const char* propName, T var) {	\
	int index = this->SelAttr(propName);	\
	switch (index)	\
	{	\
		BOOST_PP_SEQ_FOR_EACH_I(REFLECT_EACH_GEN_SET, data, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__)) \
	}\
}

#define REFLECT_EACH_GEN_GEN(r, data, i, x) \
(*_attrIndex)[BOOST_PP_STRINGIZE(STRIP(x))] = i;

#define REFLECT_EACH_GEN_GET(r, data, i, x) \
case i:	\
	mv = Transform(this->STRIP(x));\
	break;\

#define REFLECT_EACH_GEN_SET(r, data, i, x) \
case i:	\
	this->STRIP(x)=var;\
	break;\

#define REFLECT_EACH(r, data, i, x) \
PAIR(x);
