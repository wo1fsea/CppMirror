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

using namespace boost;

namespace meta
{
#pragma warning(push)
	// disable addition overflow warning
#pragma warning(disable:4307)

	template <typename Seed, typename Value>
	struct hash_combine
	{
		typedef mpl::size_t<
			Seed::value ^ (static_cast<std::size_t>(Value::value)
				+ 0x9e3779b9 + (Seed::value << 6) + (Seed::value >> 2))
		> type;
	};

#pragma warning(pop)

	// Hash any sequence of integral wrapper types
	template <typename Sequence>
	struct hash_sequence
		: mpl::fold<
		Sequence
		, mpl::size_t<0>
		, hash_combine<mpl::_1, mpl::_2>
		>::type
	{};

	// For hashing std::strings et al that don't include the zero-terminator
	template <typename String>
	struct hash_string
		: hash_sequence<String>
	{};

	// Hash including terminating zero for char arrays
	template <typename String>
	struct hash_cstring
		: hash_combine<
		hash_sequence<String>
		, mpl::size_t<0>
		>::type
	{};

} // namespace meta

template<class T>
struct MagicTypeIndex
{
	static const int index = -1;
};


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
static const int fields_n = BOOST_PP_VARIADIC_SIZE(__VA_ARGS__); \
friend struct reflector; \
template<int N, class Self> \
struct field_data {}; \
BOOST_PP_SEQ_FOR_EACH_I(REFLECT_EACH, data, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))

#define REFLECT_EACH(r, data, i, x) \
PAIR(x); \
template<class Self> \
struct field_data<i, Self> \
{ \
    Self & self; \
    field_data(Self & self) : self(self) {} \
    \
    typename make_const<Self, TYPEOF(x)>::type & get() \
    { \
        return self.STRIP(x); \
    }\
    typename boost::add_const<TYPEOF(x)>::type & get() const \
    { \
        return self.STRIP(x); \
    }\
    const char * name() const \
    {\
        return BOOST_PP_STRINGIZE(STRIP(x)); \
    } \
}; \


struct reflector
{
	//Get field_data at index N
	template<int N, class T>
	static typename T::template field_data<N, T> get_field_data(T& x)
	{
		return typename T::template field_data<N, T>(x);
	}

	// Get the number of fields
	template<class T>
	struct fields
	{
		static const int n = T::fields_n;
	};
};

struct field_visitor
{
	template<class C, class Visitor, class I>
	void operator()(C& c, Visitor v, I)
	{
		v(reflector::get_field_data<I::value>(c));
	}
};


template<class C, class Visitor>
void visit_each(C & c, Visitor v)
{
	typedef boost::mpl::range_c<int, 0, reflector::fields<C>::n> range;
	boost::mpl::for_each<range>(boost::bind<void>(field_visitor(), boost::ref(c), v, _1));
}

/* 
struct Person
{
	Person(const char *name, int age)
		:
		name(name),
		age(age)
	{
	}
private:
	REFLECTABLE
		(
			(const char *)name,
			(int)age
			)
};
*/

struct print_visitor
{
	template<class FieldData>
	void operator()(FieldData f)
	{
		std::cout << f.name() << "=" << f.get() << std::endl;
	}
};

template<class T>
void print_fields(T & x)
{
	visit_each(x, print_visitor());
}

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

void printVar(magicVar var)
{
	switch (var.type)
	{
	case magicVar::int_:
		std::cout << var.value.intV << std::endl;
	case magicVar::float_:
		std::cout << var.value.floatV << std::endl;
	case magicVar::double_:
		std::cout << var.value.doubleV << std::endl;
	case magicVar::string_:
		std::cout << var.value.stringV << std::endl;
	default:
		std::cout << "Error!" << std::endl;
		break;
	}
}

class TestObj {
public:
	const char* mProp0;
	int mProp1;
	
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
		return mv;
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

	const char* GetProp0() { return mProp0; }
	int GetProp1() { return mProp1; }
};

const char* const TestObj::_index[] = { "mProp0", "mProp1","mProp3"};

int main() {
	//Person p("Tom", 82);
	//print_fields(p);
	auto obj = new TestObj("stringProp", 110);
	std::cout<<obj->FindAttrIndex("mProp3");
	//obj->SetAttr("mProp1", 1);
	//std::count << obj->GetAttr("mProp1") << std::endl;
	//std::cout << boost::hash_value("mProp1") << std::endl;
	boost::hash<const char*> func;
	//std::cout << meta::hash_cstring<mpl::c_str<"mProp1">::value>::value << std::endl;
	return 0;

}