#include "CppMirror.hpp"
#include "MagicVar.hpp"

class TestObj {
public:
	std::string mProp0;
	int mProp1;
	TestObj(const char* prop0, int prop1) :mProp0(prop0), mProp1(prop1){
		a = [](int i) {
			std::cout << i << std::endl;
		};
	}
public:
	REFLECTABLE(
		(std::string) mProp0,
		(int)mProp1
		)

	typedef void callable(int i);
	callable *a;
};

REFLECT(TestObj)

int main() {
	auto obj = new TestObj("stringProp", 110);
	obj->SetAttr("mProp1", 100);
	printVar(obj->GetAttr("mProp1"));
	//obj->SetAttr("mProp1", 1);
	//std::count << obj->GetAttr("mProp1") << std::endl;
	//std::cout << boost::hash_value("mProp1") << std::endl;
	boost::hash<const char*> func;
	//std::cout << meta::hash_cstring<mpl::c_str<"mProp1">::value>::value << std::endl;
	obj->a(1);
	return 0;

}
