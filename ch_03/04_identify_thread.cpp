#include <chrono>
#include <iostream>
#include <thread>

using namespace std::chrono_literals;

void func() {
    std::this_thread::sleep_for(3s);
    std::cout << "Inside Thread[t]: " << std::this_thread::get_id() << "\n";
}

int main() {
    std::thread t(func);
    std::cout << "Thread[t] ID: " << t.get_id() << "\n";
    std::cout << "Thread[main] ID: " << std::this_thread::get_id() << std::endl;
    t.join();
    return 0;
}