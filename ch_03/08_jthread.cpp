#include <iostream>
#include <syncstream>
#include <chrono>
#include <thread>
#include <functional>


#define sync_cout std::osyncstream(std::cout)

/**
 * std::jthread like std::thread but with functionality:
 * thread rejoins on destruction (following RAII)
 * 
 * When two std::jthreads are destroyed, their destructors are called
 * in reverse order from their constructors (RAII)
 * 
*/



using namespace std::chrono_literals;

// class JthreadWrapper {
//    public:
//     JthreadWrapper(const std::function<void(const std::string&)>& func, const std::string& str)
//         : t(func, str), name(str) {
//             sync_cout << "Thread " << name << " being created" << std::endl;
//         }

//     ~JthreadWrapper() {
//         sync_cout << "Thread " << name << " being destroyed" << std::endl;
//     }

//    private:
//     std::jthread t;
//     std::string name;
// };

// Using perfect-forwarding without std::function
class JthreadWrapper {
public:
    template <class F>
    explicit JthreadWrapper(F&& f, std::string s)
        : t(std::forward<F>(f), s), name(std::move(s)) {
        sync_cout << "Thread " << name << " being created" << std::endl;
    }

    ~JthreadWrapper() {
        sync_cout << "Thread " << name << " being destroyed" << std::endl;
    }

private:
    std::jthread t;
    std::string  name;
};


void func(const std::string& name) {
    sync_cout << "Thread " << name << " starting..." << std::endl;
    std::this_thread::sleep_for(1s);
    sync_cout << "Thread " << name << " finishing..." << std::endl;
}

int main() {
    JthreadWrapper t1(func, "t1");
    JthreadWrapper t2(func, "t2");
    JthreadWrapper t3(func, "t3");

    std::this_thread::sleep_for(2s);

    // t1, t2, t3 will be destroyed when main exits
    sync_cout << "Main thread exiting..." << std::endl;

    return 0;
}

/**
 * @brief Meaning of the constructor parameter
 *        `const std::function<void(const std::string&)>& func`
 *
 * @details
 * - `std::function<void(const std::string&)>`:
 *   A type-erased callable wrapper that can store anything callable
 *   with the signature `void(const std::string&)` (e.g., function pointer,
 *   lambda, functor object).
 *
 * - `const &`:
 *   The function object is passed by const reference, so the constructor
 *   itself does not copy it at the call site and cannot modify it.
 *
 * - In the initializer list:
 *   @code
 *   JthreadWrapper(const std::function<void(const std::string&)>& func, const std::string& str)
 *       : t(func, str), name(str) { ... }
 *   @endcode
 *   The `std::jthread` constructor still **copies** the callable into
 *   its own storage. Thus, even with `const&`, a copy occurs inside
 *   the thread object.
 *
 * ### Pros
 * - Simple, uniform API: anything callable with `void(const std::string&)` works.
 * - Const reference avoids an extra copy at the constructor boundary.
 *
 * ### Cons
 * - **Type erasure overhead**: `std::function` may allocate and adds
 *   indirection on invocation.
 * - **Still copies**: `std::jthread` copies the callable internally,
 *   so temporaries are not perfectly forwarded.
 *
 * ### Modern alternative (C++17 perfect forwarding)
 * @code
 * class JthreadWrapper {
 * public:
 *     template <class F>
 *     explicit JthreadWrapper(F&& f, std::string s)
 *         : t(std::forward<F>(f), s), name(std::move(s)) {
 *         sync_cout << "Thread " << name << " being created" << std::endl;
 *     }
 * private:
 *     std::jthread t;
 *     std::string  name;
 * };
 * @endcode
 * - Accepts any callable type without type erasure or heap allocation.
 * - Moves temporaries into the thread (zero-copy).
 * - Works with function pointers, lambdas, and function objects.
 *
 * ### Notes on the signature
 * - Callables must be invocable with `const std::string&`.
 * - Callables taking `std::string` by value are compatible (they copy).
 * - Callables requiring `std::string&` (non-const) are **not** compatible.
 *
 * ### Alternative design
 * If you want to keep `std::function` but allow efficient moves:
 * @code
 * JthreadWrapper(std::function<void(const std::string&)> func, std::string s)
 *     : t(std::move(func), s), name(std::move(s)) {}
 * @endcode
 * - Passing by value enables moving from temporaries into the thread,
 *   while still keeping the `std::function` interface.
 */




/**
 * @variant: Using JThreadWrapper class with a function pointer
 * 
 * // A regular free function that matches the required signature
 * void printMessage(const std::string& msg) {
 *     sync_cout << "From function pointer: " << msg << std::endl;
 * }

 * // A wrapper class that launches a jthread
 * class JthreadWrapper {
 * public:
 *     // Constructor accepts any callable with signature void(const std::string&)
 *     JthreadWrapper(const std::function<void(const std::string&)>& func, const std::string& str)
 *         : t(func, str), name(str) {
 *         sync_cout << "Thread " << name << " being created" << std::endl;
 *     }

 *     ~JthreadWrapper() {
 *         sync_cout << "Thread " << name << " being destroyed" << std::endl;
 *     }

 * private:
 *     std::jthread t;
 *     std::string name;
 * };

 * int main() {
 *     // Function pointer to free function
 *     void (*fp)(const std::string&) = &printMessage;

 *     // Pass the function pointer into the wrapper
 *     JthreadWrapper jw(fp, "Hello from function pointer!");

 *     // Automatic join happens in std::jthread destructor
 *     return 0;
 * }
 */