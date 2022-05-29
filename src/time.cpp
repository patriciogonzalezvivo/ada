#include "ada/time.h"

#ifdef _WIN32
    #define NOMINMAX
    #include <windows.h>
    #include <chrono>
    #include <thread>
#else
    #include <unistd.h>
#endif 

namespace ada {

void sleep_ms(uint64_t value) {
#if defined(_WIN32)
    std::this_thread::sleep_for(std::chrono::microseconds(value));
#else
    usleep(value);
#endif 

}

}