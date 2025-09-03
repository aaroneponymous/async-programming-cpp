#include <iostream>
#include <thread>
#include <cxxabi.h>
#include <typeinfo>


// RTTI : Runtime Type Information (using cxxabi to demangle type string - make it human readable)

template <typename T>
std::string type_name(const T& var) {
    int status;
    char* demangled = abi::__cxa_demangle(typeid(var).name(), 0, 0, &status);
    std::string result = (status == 0) ? demangled : typeid(var).name();
    std::free(demangled);
    return result;
}

// Create six threads using different callable objects

void func() {
    std::cout << "t1: Using function pointer\n\n";
}

class FuncObjectClass {
    public:
     void operator() () {
        std::cout << "t4: Using function object class\n\n";
     }
};

// t5: using a non-static member function by passing the address of the member function
//     and the address of an object to call the member function

class Obj {
    public:
     void func() {
        std::cout << "t5: Using a non-static member function\n\n";
     }
};

// t6: using a static member function where only the address of the member function is
//     needed as the method is static

class ObjStatic {
    public:
     static void static_func() {
        std::cout << "t6: Using a static member function\n\n";
     }
};


int main()
{

    const auto processor_count = std::thread::hardware_concurrency();
    std::cout << "processor_count: " << processor_count << "\n";
    std::cout << "processor_count (std::thread::hardware_concurrency()) type: " << type_name(processor_count) << "\n\nRunning Threads:\n\n";

    std::thread t1(func);
    
    // t2: using a lambda func
    auto lambda_func = [] () {
        std::cout << "t2: Using a lambda function\n\n";
    };

    std::thread t2(lambda_func);

    // t3: using an embedded lambda func

    std::thread t3([] () {
        std::cout << "t3: Using embedded lambda function\n\n";
    });

    // t4: function object class
    std::thread t4{FuncObjectClass()};  // used { } to initialize the FuncObjectClass and then call to avoid Most Vex Parsing Error

    /**
     * What happens here:
     * 
     * - `FuncObjectClass()` creates a temporary function object.
     * - The braced initializer `{...}` is used to construct the `std::thread` with that temporary.
     * - This avoids the "most vexing parse" problem that would arise if you wrote:
     * 
     *     std::thread t4(FuncObjectClass());
     * 
     *   which the compiler could interpret as a function declaration instead of an object.
     * 
     * Correction to your comment:
     * 
     * - Your idea is right: using `{}` avoids the most vexing parse.
     * - But to be precise, the code is:
     *   * Initializing a `std::thread` object with a temporary `FuncObjectClass`.
     *   * The braces are what disambiguate it from a function declaration.
     * 
     * Better comment:
     * 
     *     // Start a std::thread with a temporary FuncObjectClass.
     *     // Using {} avoids the most vexing parse (interpreting it as a function declaration).
    */




    // t5: non-static member function
    Obj obj;
    std::thread t5(&Obj::func, &obj);

    // t6: using a static member function
    std::thread t6(&ObjStatic::static_func);

    if (t1.joinable()) t1.join();
    if (t2.joinable()) t2.join();
    if (t3.joinable()) t3.join();
    if (t4.joinable()) t4.join();
    if (t5.joinable()) t5.join();
    if (t6.joinable()) t6.join();



    return 0;
}
