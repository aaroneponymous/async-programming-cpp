#include <iostream>
#include <syncstream>
#include <thread>
#include <string>
#include <vector>
#include <functional> // std::ref(), std::cref() define in functional

// std::ref(), std::cref()
// Helper functions are used to generate std::reference_wrapper objects
// which wrap a reference in a copyable and assignable object

/**
 * @brief Behavior of argument passing to std::thread.
 *
 * When you pass arguments to a std::thread constructor:
 * - Unless wrapped with std::ref() or std::cref(), each argument is
 *   **decay-copied** into the thread’s internal storage.
 * - When the thread starts, it invokes the target function with those
 *   stored copies.
 *
 * Example:
 * @code
 * void funcByValue(const std::string& str, int val);
 * std::thread t1(funcByValue, str_val, 1);
 * @endcode
 * - Here, `str_val` is copied into the thread’s storage.
 * - Inside funcByValue, `str` (a const reference) binds to that copy,
 *   not to the original `str_val`.
 *
 * To actually bind to the original object, wrap it:
 * @code
 * std::thread t1(funcByValue, std::cref(str_val), 1);
 * @endcode
 * - Now `str` refers to the original `str_val`.
 *
 * Additional notes:
 * - If the function requires a non-const lvalue reference (e.g. `std::string&`),
 *   std::thread(f, obj) won’t compile because it passes an rvalue.
 *   Use std::ref(obj).
 *
 * - Using std::move(obj) moves the object into the thread’s storage.
 *   The parent’s obj becomes moved-from (valid but unspecified state),
 *   which is safe but requires reassignment before reuse.
 */



#define sync_cout std::osyncstream(std::cout)

// Passing Arguments


/**
 * 01. Passing by value
 * - Avoids race conditions but costly due to
 *   copying
 */

void funcByValue(const std::string& str, int val) {

    sync_cout << "str: " << str << ", val: " << val << std::endl;
    
}


/**
 * 02. Passing by reference
 * 
 */

void modifyValues(std::string& str, int& val) {
    str += " (Thread)";
    val++;
}


/**
 * 03. Passing by const-reference
 */

void printVector(const std::vector<int>& vec) {
    sync_cout << "Vector: ";
    for (int num: vec) {
        sync_cout << num << " ";
    }
    sync_cout << std::endl;
}

/**
 * 06. Passing by non-const reference (Vector)  - Compilation Error
 */

void modifyVector(std::vector<int>& vec) {
    for (auto& elem : vec) {
        elem = elem + elem;
    }
}



int main() {

    // 01. Passing by Value
    std::string str_val{"Passing by value"};
    std::cout << str_val << std::endl;
    std::thread t1(funcByValue, str_val, 1);
    t1.join();

    // 02. Passing by reference
    std::string str_ref{"Passing by reference"};
    std::cout << str_ref << std::endl;
    int val = 1;
    std::thread t2(modifyValues, std::ref(str_ref), std::ref(val));
    t2.join();
    std::cout << "str_ref: " << str_ref << ", val_ref: " << val << std::endl;

    // 03. Passing by const-reference
    std::cout << "Passing by const-reference" << std::endl;
    std::vector<int> vec{1, 2, 3, 4, 5};
    std::thread t3(printVector, std::cref(vec));
    t3.join();

    // 04. Moving object to thread (invalidates in parent thread (undefined behavior))
    std::thread t_move(printVector, std::move(vec));
    t_move.join();


    // 05. Threads accessing variables by lambda capture
    std::string hello{"Hello"};
    std::thread t([&] () {
        sync_cout << "str_lamda: " << hello << std::endl;
    });
    t.join();


    // 06. Passing by non-const reference

    std::vector<int> vec_2{1, 2, 3, 4, 5};

    // This results in compilation error:
    // non-const l-value passed as argument : and thread
    // expects rvalue
    // error: static assertion failed: std::thread arguments must be 
    // invocable after conversion to rvalues

    // std::thread t6(modifyValues, vec_2);

    // Use std::ref
    std::thread t6(modifyVector, std::ref(vec_2));
    t6.join();
    printVector(vec_2);

    return 0;

};