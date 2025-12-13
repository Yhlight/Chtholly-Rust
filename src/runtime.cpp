#include <iostream>

extern "C" {

void println(const char* s) {
    std::cout << s << std::endl;
}

}
