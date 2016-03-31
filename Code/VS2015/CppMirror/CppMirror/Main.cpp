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

struct magicVar{
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

class TestObj {
public:
	std::string mProp0;
	int mProp1;
	TestObj(const char* prop0, int prop1) :mProp0(prop0), mProp1(prop1){}
public:
	REFLECTABLE(
		(std::string) mProp0,
		(int)mProp1
		)
};

REFLECT(TestObj)
const char* const TestObj::_index[] = { "mProp0", "mProp1","mProp3"};

int main() {
	auto obj = new TestObj("stringProp", 110);
	obj->SetAttr("mProp1", 100);
	printVar(obj->GetAttr("mProp1"));
	//obj->SetAttr("mProp1", 1);
	//std::count << obj->GetAttr("mProp1") << std::endl;
	//std::cout << boost::hash_value("mProp1") << std::endl;
	boost::hash<const char*> func;
	//std::cout << meta::hash_cstring<mpl::c_str<"mProp1">::value>::value << std::endl;
	return 0;

}
