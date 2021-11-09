#include <iostream>
#include "streams_divider.hpp"

bool error_handler(const char* error_message) {
    std::cout << error_message << "\n";
    return false;
}

int main() {
    std::cout << "main invoked 🏎\n";
    streams_divider();
}