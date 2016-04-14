#include "CppMirror.hpp"
#include "Bottle.hpp"

#include <list>

class TestObj {
public:
	std::string mProp0;
	int mProp1;
	TestObj(std::string prop0, int prop1) :mProp0(prop0), mProp1(prop1){
	}

	int fun0(int i, int j) {
		std::cout << i << "," << j << std::endl;
		return 1;
	}
	int fun1(double d) {
		std::cout << d << std::endl;
		return 1;
	}

public:
	REFLECT_PROP(
		(std::string) mProp0,
		(int) mProp1
		)
	REFLECT_FUN(
		(void)(fun0)((int) i, (int) j),
		(void)(fun1)((double) d)
		)
};

REFLECT(TestObj)



int main() {
	int intV = 100001;
	auto obj = new TestObj("stringProp", 110);
	std::cout << obj->GetAttr("mProp0");
	obj->SetAttr("mProp0", string("100"));
	auto re = obj->GetAttr("mProp0");
	std::cout << re << std::endl;

	std::vector<Bottle> args;
	args.push_back(Bottle(1));
	args.push_back(Bottle(2));
	obj->CallMethod("fun0", args);

	std::vector<Bottle> args0;
	args0.push_back(Bottle(1.11111));

	auto bi = Bottle(int_ptr(new int(66666)));

	std::cout << bi << "," << *Funnel<int_ptr>::Pour(bi) << std::endl;

	obj->CallMethod("fun1", args0);
	return 0;

}
