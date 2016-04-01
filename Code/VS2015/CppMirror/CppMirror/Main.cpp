#include "CppMirror.hpp"
#include "MagicVar.hpp"

#include <list>

class TestObj {
public:
	std::string mProp0;
	int mProp1;
	TestObj(const char* prop0, int prop1) :mProp0(prop0), mProp1(prop1){
		a = [](int i) {
			std::cout << i << std::endl;
		};
	}

	void fun0(int i, int j) {
		std::cout << i << "," << j << std::endl;
	}
	void fun1(std::string str) {
		std::cout << str << std::endl;
	}


	magicVar Call(std::string funcName, std::list<magicVar> args)
	{
		//int index = this->SelMethod(funcName);
		//magicVar result;
		//switch (index)
		//{
		//case 1:
		//	int argsNum = 10;
		//	if (args.size() != argsNum)
		//	{
		//		//error
		//		
		//	}
		//	try {
		//		result = this->fun0(args[0].intV, )
		//	}catch (int i) {
		//		//error
		//	}
		//	break;
		//}
		return magicVar();
	}

public:
	REFLECT_PROP(
		(std::string) mProp0,
		(int) mProp1
		)
	REFLECT_FUN(
		(void)(fun0)((int) i, (int) j)
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
	std::cout << obj->GenMethodIndex("fun1")<< std::endl;
	obj->a(1);
	return 0;

}