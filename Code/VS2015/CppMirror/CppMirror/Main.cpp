#include <boost/preprocessor/stringize.hpp>
#include <boost/preprocessor/variadic/size.hpp>
#include <boost/preprocessor/seq/for_each_i.hpp>
#include <boost/preprocessor/variadic/to_seq.hpp>
#include <boost/type_traits.hpp>
#include <boost/mpl/range_c.hpp>
#include <boost/mpl/for_each.hpp>
#include <boost/bind.hpp>
#include <iostream>

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

class TestObj {
public:
	const char* mProp0;
	int mProp1;

	template<class Self, class T>
	struct _GetAttr{
		Self &self;
		_GetAttr(Self & self) : self(self) {} 
		T operator()(const char* propName)
		{
			return self.GetProp0();
		}
	};

	typename _GetAttr<TestObj, const char*> GetAttr;
	TestObj(const char* prop0, int prop1) :mProp0(prop0), mProp1(prop1), GetAttr(*this){}

private:

	const char* GetProp0() { return mProp0; }
	int GetProp1() { return mProp1; }
};

int main() {
	//Person p("Tom", 82);
	//print_fields(p);
	auto obj = new TestObj("stringProp", 110);
	auto a = (obj->GetAttr("mProp0"));
	std::cout<< a << std::endl;
	//obj->SetAttr("mProp1", 1);
	//std::count << obj->GetAttr("mProp1") << std::endl;
	return 0;
}