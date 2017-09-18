//refer to http://blog.jobbole.com/44015/
#include <stdio.h>
#include <map>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <assert.h>

using namespace std;

//enum
void myEnum(){
    //enum old way
    enum Days { Saturday,Sunday,Tuesday,Wednesday,Thursday,Friday};
    Days day = Saturday;
    if(day == Saturday){
        cout << "enum day Tuesday" << Tuesday << endl;
    }

    //c++11 use enum class as a namespace, underlying_type is good, static_cast<type>(x) is good
    enum class Dayscpp { Saturday,Sunday,Tuesday,Wednesday,Thursday,Friday};
    Dayscpp daycpp = Dayscpp::Saturday;
    if(daycpp == Dayscpp::Saturday){
        cout << "enum day Tuesday: " << static_cast<underlying_type<Dayscpp>::type>(Dayscpp::Tuesday) << endl;
    }

    long x = 1;
    cout << static_cast<int>(x) << endl;
}

//smart pointers
void foo(int* p)
{
    std::cout << *p << std::endl;
}
void bar(std::shared_ptr<int> p)
{
    ++(*p);
}
void myPtrs(){
    //unique_ptr
	std::unique_ptr<int> p1(new int(42));
	std::unique_ptr<int> p2 = std::move(p1); // transfer ownership

	if(p1)
		foo(p1.get());

	(*p2)++;

	if(p2)
		foo(p2.get());

    //shared_ptr
    std::shared_ptr<int> p3(new int(42));
    //use make_shared<int>(42) as a shared_ptr in auto, onetime alloc mem for int ptr and the shared_ptr 
    //auto p3 = std::make_shared<int>(42);)
    //for etc
    //foo(std::shared_ptr<int>(new int(42)), seed());)
    //this may cause mem leak if seed is rise a exception
    std::shared_ptr<int> p4 = p3;

    bar(p3);
    //use get to obtain the ptr
    foo(p4.get());

    int* x =new int(45);
    //a err, shared_ptr must init with new...
    //shared_ptr<int> p5 = x; 

    //weak_ptr, must use lock to obtain the ptr
	auto p6 = std::make_shared<int>(42);
	std::weak_ptr<int> wp = p6;

	//lambda func?
	{
		auto sp = wp.lock();
        //it is the ptr
		std::cout << sp.get() << std::endl;
        //it is the value of the ptr
		std::cout << *sp << std::endl;
	}

	//decrease the p6 ptr counter
	p6.reset();
	if(wp.expired())
		std::cout << "expired" << std::endl;
}

//lambda
void myLambda(){
	std::vector<int> v;
	v.push_back(1);
	v.push_back(2);
	v.push_back(3);
	std::for_each(std::begin(v), std::end(v), [](int n) {std::cout << n << std::endl;});

	auto is_odd = [](int n) {return n%2==1;};
	auto pos = std::find_if(std::begin(v), std::end(v), is_odd);
	if(pos != std::end(v))
		std::cout << *pos << std::endl;

	std::function<int(int)> lfib = [&lfib](int n) {return n < 2 ? 1 : lfib(n-1) + lfib(n-2);};
}

//static_assert when init vector
template <typename T, size_t Size>
class Vector
{
   static_assert(Size < 3, "Size is small enough");
   T _points[Size];
};

//typer assert and decltype
template <typename T1, typename T2>
auto add(T1 t1, T2 t2) -> decltype(t1 + t2)
{
    static_assert(std::is_integral<T1>::value, "Type T1 must be integral");
    static_assert(std::is_integral<T2>::value, "Type T2 must be integral");

    return t1 + t2;
}

//what move means? move is semantic which move some mem out(smart ptr and string etc implement it), a temp value is like a return value of a function
//so move is used to avoid unnessary deep copy, you need to write a func with right value param
template <typename T>
class Buffer 
{
	std::string          _name;
	size_t               _size;
	std::unique_ptr<T[]> _buffer;

	public:
	// default constructor
	Buffer():
		_size(16),
		_buffer(new T[16])
	{}

	// constructor
	Buffer(const std::string& name, size_t size):
		_name(name),
		_size(size),
		_buffer(new T[size])
	{}

	// copy constructor
	Buffer(const Buffer& copy):
		_name(copy._name),
		_size(copy._size),
		_buffer(new T[copy._size])
	{
		T* source = copy._buffer.get();
		T* dest = _buffer.get();
		std::copy(source, source + copy._size, dest);
	}

	// copy assignment operator
	Buffer& operator=(const Buffer& copy)
	{
		if(this != &copy)
		{
			_name = copy._name;

			if(_size != copy._size)
			{
                //because it is a unique_ptr
				_buffer = nullptr;
				_size = copy._size;
				_buffer = _size > 0 ? new T[_size] : nullptr;
			}

			T* source = copy._buffer.get();
			T* dest = _buffer.get();
			std::copy(source, source + copy._size, dest);
		}
		return *this;
	}

	// move constructor
    // right value
	Buffer(Buffer&& temp):
        //string implement the move to get the right value
		_name(std::move(temp._name)),
		_size(temp._size),
		_buffer(std::move(temp._buffer))
	{
		temp._buffer = nullptr;
		temp._size = 0;
	}

	// move assignment operator
	Buffer& operator=(Buffer&& temp)
	{
        assert(this != &temp); // assert if this is not a temporary

		_buffer = nullptr;
		_size = temp._size;
		_buffer = std::move(temp._buffer);

		_name = std::move(temp._name);

		temp._buffer = nullptr;
		temp._size = 0;

		return *this;
	}
};

//the function's return value is right value, same as every function
template <typename T>
Buffer<T> getBuffer(const std::string& name) 
{
   Buffer<T> b(name, 128);
   return b;
}

int main(){
    //unordered map is a hash talbe, so traverse is not ordered
    unordered_map<int, bool> m;
    m.insert(make_pair(4, true));
    m.insert(make_pair(1, true));
    m.insert(pair<int, bool>(100, true));

    for(unordered_map<int,bool>::iterator it = m.begin(); it != m.end(); it++){
        //cout << "first:" << it->first << ", second: " << it->second << endl;
    }

    //auto
    auto x = 0LL;
    //cout << x << endl;

    //for with begin/end by auto, as a iterator
    for(auto it = begin(m); it != end(m); it++){
        //cout << "first:" << it->first << ", second: " << it->second << endl;
    }

    //for like foreach, auto is a value
    for(auto mkv: m){
        //cout << "first:" << mkv.first << ", second: " << mkv.second << endl;
    }
    //we should use reference instead
    for(const auto& mkv: m){
        //cout << "first:" << mkv.first << ", second: " << mkv.second << endl;
    }

    //nullptr
    int *p = nullptr;
    // int y = nullptr; can not convert int to nullptr
    bool z = nullptr;
    //cout << "bool z: " << z << ", nullptr p: " << p << endl;
    
	//myEnum();
	//myPtrs();
	Vector<int, 1> a1;
    //static_assert will fail when compile
	//Vector<double, 12> a2;

    //cout << "add int: " << add(1, 2) << endl;
    //add double will err
    //cout << "add double: " << add(1, 2.1) << endl;


	Buffer<int> b1;
	Buffer<int> b2("buf2", 64);
	Buffer<int> b3 = b2; //call copy assign func
	Buffer<int> b4 = getBuffer<int>("buf4"); //call move assign func
	b1 = getBuffer<int>("buf5");//call move
	return 0;
}

//override and final used in virtual function
class B 
{
public:
   virtual void f(int) {std::cout << "B::f" << std::endl;}
};
 
class D : public B
{
public:
   virtual void f(int) override final {std::cout << "D::f" << std::endl;}
};
 
/*function f is marked as final, so it can not be override anymore
 * class F : public D
{
public:
   virtual void f(int) override {std::cout << "F::f" << std::endl;}
};*/


