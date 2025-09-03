#include <chrono>
#include <iostream>
#include <random>
#include <syncstream>
#include <thread>

#define sync_cout std::osyncstream(std::cout)

using namespace std::chrono_literals;

namespace {
    int result = 0;
};


void func(int& result) {
    std::this_thread::sleep_for(1s);
    result = 1 + (rand() % 10);
}

int main() {
    std::thread t(func, std::ref(result));
    t.join();
    sync_cout << "Result: " << result << std::endl;

    std::thread t_lambda([&] () { func(result); });
    t_lambda.join();
    sync_cout << "Result: " << result << std::endl;

    return 0;

/**
 * @brief Unnamed (anonymous) namespace in C++.
 *
 * Usage:
 * @code
 * namespace {
 *     int result = 0;
 * }
 * @endcode
 *
 * @details
 * - **Internal linkage**: All entities declared inside an unnamed namespace
 *   are visible only within the current translation unit (.cpp file).
 *   They cannot be accessed from other translation units.
 *
 * - **Modern replacement for static globals**: In older C++, one would use
 *   `static int result = 0;` to restrict visibility to the file.
 *   Today, the preferred way is to place the declaration inside
 *   an unnamed namespace.
 *
 * - **Unique per translation unit**: Each translation unit gets its own
 *   distinct unnamed namespace. This means that two different .cpp files
 *   can both declare a variable `result` inside an unnamed namespace
 *   without causing linkage conflicts.
 *
 * @note
 * In your example:
 * @code
 * namespace {
 *     int result = 0;
 * }
 * @endcode
 * - The variable `result` is private to this .cpp file.
 * - Another .cpp file with its own unnamed namespace and a `result`
 *   variable would define a completely separate variable.
 *
 * @summary
 * An unnamed namespace is the modern C++ way to declare "private globals"
 * that are restricted to a single translation unit.
 */


 /**
 * @brief Global static functions vs. anonymous namespace.
 *
 * @details
 * 1. **Static global functions (C++98 style)**:
 * @code
 * static void helper() {
 *     // only visible inside this .cpp file
 * }
 * @endcode
 * - Declaring a function as `static` at namespace scope gives it
 *   **internal linkage** (restricted to the current translation unit).
 * - This was the traditional way to hide globals before C++11.
 *
 * 2. **Anonymous namespace (modern C++ style)**:
 * @code
 * namespace {
 *     void helper() {
 *         // also only visible inside this .cpp file
 *     }
 * }
 * @endcode
 * - Anything inside an unnamed namespace also has **internal linkage**.
 * - Preferred in modern C++ because it works consistently for functions,
 *   variables, and classes without needing to mark each one `static`.
 *
 * 3. **Combining them**:
 * @code
 * namespace {
 *     static void helper() { / ... / }
 * 
 * @endcode
 * - This is valid C++, but the `static` is redundant.
 * - The anonymous namespace already enforces internal linkage.
 *
 * @note
 * Best practice: use **either `static` or anonymous namespaces**, not both.
 * Modern C++ recommends anonymous namespaces for file-local (translation unit
 * private) functions and variables.
 */

