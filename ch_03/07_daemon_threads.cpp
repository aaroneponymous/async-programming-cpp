#include <iostream>
#include <syncstream>
#include <chrono>
#include <thread>

#define sync_cout std::osyncstream(std::cout)

using namespace std::chrono_literals;

namespace {
    int timeout = 3;
}

void daemonThread() {
    sync_cout << "Daemon thread starting ...\n";
    while (timeout-- > 0) {
        sync_cout << "Daemon thread running ...\n";
        std::this_thread::sleep_for(1s);
    }
    sync_cout << "Daemon thread exiting ...\n";
}

int main() {
    sync_cout << "Main thread starting ...\n";
    std::thread daemon(daemonThread);
    daemon.detach();

    std::this_thread::sleep_for(std::chrono::seconds(timeout + 1));
    sync_cout << "Main thread exiting ...\n";
    return 0;
}