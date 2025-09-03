#include <iostream>
#include <thread>
#include <syncstream> // c++ 20 onwards

/**
 * The primary purpose of std::osyncstream is to prevent output from multiple threads 
 * to the same stream from becoming interleaved or garbled.
 *  
 * It achieves this by buffering output operations and 
 * then atomically transferring the buffered content to the wrapped stream (e.g., std::cout) 
 * upon destruction of the osyncstream object or when explicitly flushed.
 */

#define sync_cout std::osyncstream(std::cout)

int main() {
    std::thread t1([] () {
        for (int i = 0; i < 100; ++i) {
            sync_cout << "1 " << "2 " << "3 " << "4 "
                      << std::endl;
        };
    });
    
    std::thread t2([] () {
        for (int i = 0; i < 100; ++i) {
            sync_cout << "5 " << "6 " << "7 " << "8 "
                      << std::endl;
        };
    });

    t1.join();
    t2.join();
    return 0;
}