#include <chrono>
#include <iostream>
#include <syncstream>
#include <thread>

#define sync_cout std::osyncstream(std::cout)

using namespace std::chrono_literals;

void func() {
    for (auto i = 0; i < 10; ++i) {
        sync_cout << "Thread ID: " << std::this_thread::get_id() << " is working." << std::endl;
        std::this_thread::sleep_for(500ms);
    }
}

int main() {
    std::thread t1(func);
    sync_cout  << "T1 id: " << t1.get_id() << std::endl;
    std::this_thread::sleep_for(1s);

    // Thread 1 moved to thread 2. It keeps the same ID, and t1 is no longer joinable.
    std::thread t2 = std::move(t1);
    sync_cout  << "T2 id: " << t2.get_id() << std::endl;
    std::this_thread::sleep_for(1s);

    sync_cout << "Are threads joinable? t1=" << t1.joinable() << "  t2=" << t2.joinable() << std::endl;

    t2.join();

    return 0;
}

/**
 * @brief Why the thread ID remains the same after moving a std::thread.
 *
 * @details
 * - A `std::thread` object is only a **handle (RAII wrapper)** to an
 *   **underlying OS thread**.
 *
 * - When you write:
 *   @code
 *   std::thread t2 = std::move(t1);
 *   @endcode
 *   ownership of the same OS thread is transferred from `t1` to `t2`.
 *
 * - The actual thread of execution (running `func`) is not moved,
 *   paused, or restarted — it continues on the same OS thread.
 *
 * - Because `std::thread::get_id()` reports the ID of the underlying OS
 *   thread, the ID remains the same before and after the move.
 *
 * ### What changes after the move
 * - `t1` becomes a moved-from thread object:
 *   - It is no longer joinable (`t1.joinable() == false`).
 *   - `t1.get_id()` will return `std::thread::id()` (an empty ID).
 * - `t2` becomes the sole owner of the original thread.
 * - The OS thread continues unaffected.
 *
 * ### Analogy
 * Think of the OS thread as a drone:
 * - The drone (thread) keeps flying with the same ID.
 * - You pass the remote control from `t1` to `t2`.
 * - The old controller (`t1`) is now useless, but the drone is unchanged.
 *
 * @note
 * Seeing the same thread ID before and after moving is expected and
 * correct behavior in C++.
 */
