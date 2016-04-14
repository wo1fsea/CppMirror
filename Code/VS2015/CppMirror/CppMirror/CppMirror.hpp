#include <boost/preprocessor.hpp>
#include <boost/type_traits.hpp>

#include <map>
#include <vector>

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
bool x::_attrIndexInit = false;	\
std::map<std::string, int> * x::_methodIndex = nullptr; \
bool x::_methodIndexInit = false; 

#define REFLECT_PROP(...) \
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
Bottle GetAttr(const char* propName) {	\
	int index = this->SelAttr(propName);	\
	Bottle mv;	\
	switch (index)	\
	{	\
		BOOST_PP_SEQ_FOR_EACH_I(REFLECT_EACH_GEN_GET, data, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__)) \
	}	\
	return mv; \
}	\
template<class T>	\
void SetAttr(const char* propName, T var) {	\
	Bottle bottle(var);	\
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
	mv = Bottle(this->STRIP(x));\
	break;\

#define REFLECT_EACH_GEN_SET(r, data, i, x) \
case i:	\
	this->STRIP(x) = Funnel<TYPEOF(x)>::Pour(bottle);\
	break;\

#define REFLECT_EACH(r, data, i, x) \
PAIR(x);


#define TYPEOF_FUN(x)

#define REFLECT_FUN(...) \
static const int _lenIndexFun = BOOST_PP_VARIADIC_SIZE(__VA_ARGS__); \
static std::map<std::string, int> * _methodIndex; \
static bool _methodIndexInit;	\
static void GenMethodIndex() {	\
		_methodIndex = new std::map<std::string, int>(); \
		BOOST_PP_SEQ_FOR_EACH_I(REFLECT_EACH_GEN_METHOD_GEN, data, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__)) \
		_methodIndexInit = true; \
		return; \
}	\
int SelMethod(std::string methodName)	\
{	\
	if(_methodIndexInit == false) \
		this->GenMethodIndex(); \
	std::map<std::string, int>::iterator it = _methodIndex->find(methodName); \
	if(it != _methodIndex->end()) \
	return (*_methodIndex)[methodName]; \
	else \
		return -1; \
}	\
Bottle CallMethod(std::string methodName, std::vector<Bottle> args)	\
{	\
	int index = this->SelMethod(methodName);	\
	Bottle mv;	\
	switch (index)	\
	{	\
		BOOST_PP_SEQ_FOR_EACH_I(REFLECT_EACH_GEN_METHOD_CALL, data, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__)) \
	}	\
	return mv; \
}

#define STRING(x) \
BOOST_PP_STRINGIZE(x)

#define METHOD_NAME(x)	\
TYPEOF(STRIP(x))

#define ARGS(x)	\
STRIP(STRIP(x))

#define REFLECT_EACH_GEN_METHOD_GEN(r, data, i, x) \
(*_methodIndex)[STRING(METHOD_NAME(x))] = i;

#define REFLECT_EACH_GEN_METHOD_CALL(r, data, i, x) \
case i:	\
	mv = Bottle(this->METHOD_NAME(x)(GEN_METHOD_CALL_HELPER(ARGS(x)))); \
break;

#define GEN_METHOD_CALL_HELPER(...)	\
BOOST_PP_LIST_FOR_EACH_I(REFLECT_EACH_GEN_ARGS, data, BOOST_PP_TUPLE_TO_LIST(__VA_ARGS__))

#define REFLECT_EACH_GEN_ARGS(r, data, i, x)	\
BOOST_PP_COMMA_IF(i) Funnel<TYPEOF(x)>::Pour(args[i])