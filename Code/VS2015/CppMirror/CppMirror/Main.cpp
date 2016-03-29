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


#define REFLECTABLE(...) \
static const int _lenIndex = BOOST_PP_VARIADIC_SIZE(__VA_ARGS__); \
friend struct reflector; \
template<int N, class Self> \
struct field_data {}; \
BOOST_PP_SEQ_FOR_EACH_I(REFLECT_EACH, data, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))	\
static const char* const _index[]; \
int FindAttrIndex(const char* propName) \
{	\
	for (int i = 0; i < _lenIndex; i++)	\
	{	\
		if (_index[i] == propName)	\
			return i;	\
	}	\
	return -1;	\
}	\
magicVar GetAttr(const char* propName) {	\
	int index = this->FindAttrIndex(propName);	\
	magicVar mv;	\
	switch (index)	\
	{	\
		BOOST_PP_SEQ_FOR_EACH_I(REFLECT_EACH_GEN_GET, data, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__)) \
	}	\
	return mv; \
}	\

//Auto Gen
//template<class T>
//void SetAttr(const char* propName, T var) {
//	int index = this->FindAttrIndex(propName);
//	switch (index)
//	{
//	case _index4mProp0:
//		this->_name4mProp0 = var;
//		break;
//	}
//}

#define REFLECT_EACH_GEN_GET(r, data, i, x) \
case i:	\
	mv = Transform(this->STRIP(x));\
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

#define _index4mProp0 0
#define _index4mProp1 1
#define _name4mProp0 mProp0
#define _name4mProp1 mProp1
#define _type4mProp0 std::string
#define _type4mProp1 int

	//Auto Gen
	static const char* const _index[];

	//Auto Gen
	int FindAttrIndex(const char* propName)
	{
		for (int i = 0; i < sizeof(*_index); i++)
		{
			if (_index[i] == propName)
				return i;
		}
		return -1;
	}

	//Auto Gen
	magicVar GetAttr(const char* propName) {
		int index = this->FindAttrIndex(propName);
		magicVar mv;
		switch (index)
		{
		case _index4mProp0:
			//_GetAttr<TestObj, _index4mProp0, _type4mProp0> GetAttr(this);
			mv = Transform(this->_name4mProp0);
			break;
		}
		return mv;
	}

	template<class T>
	void SetAttr(const char* propName, T var) {
		int index = this->FindAttrIndex(propName);
		switch (index)
		{
		case _index4mProp0:
			this->_name4mProp0 = var;
			break;
		}
	}

	template <class Self, class T>
	struct _SetAttr
	{
		Self &self;
		_SetAttr(Self &self) : self(self) {}
		void operator()(const char* propName, T)
		{
			return;
		}
	};

	TestObj(const char* prop0, int prop1) :mProp0(prop0), mProp1(prop1){}

private:

};

const char* const TestObj::_index[] = { "mProp0", "mProp1","mProp3"};

int main() {
	//Person p("Tom", 82);
	//print_fields(p);
	auto obj = new TestObj("stringProp", 110);
	printVar(obj->GetAttr("mProp0"));
	//obj->SetAttr("mProp1", 1);
	//std::count << obj->GetAttr("mProp1") << std::endl;
	//std::cout << boost::hash_value("mProp1") << std::endl;
	boost::hash<const char*> func;
	//std::cout << meta::hash_cstring<mpl::c_str<"mProp1">::value>::value << std::endl;
	return 0;

}