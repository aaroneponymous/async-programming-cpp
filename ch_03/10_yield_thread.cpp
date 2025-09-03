#include <iostream>
#include <syncstream>
#include <string>
#include <random>
#include <thread>

#define sync_cout std::osyncstream(std::cout)

using namespace std::chrono;

namespace {
    int val = 0;
    std::mutex mtx;
}

int main() {

    auto work = [&] (const std::string& name) {
        while (true) {
            bool work_to_do = rand() % 2;
            if (work_to_do) {
                sync_cout << name << ": working\n";
                std::lock_guard<std::mutex> lock(mtx);
                for (auto start = steady_clock::now(), now = start; now < start + 3s; now = steady_clock::now()) { }
            
            } else {
                sync_cout << name << ": yielding\n";
                std::this_thread::yield();
            }
        }
    };

    std::jthread t1(work, "t1");
    std::jthread t2(work, "t2");

    return 0;
};