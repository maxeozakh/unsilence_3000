#include <iostream>

void streams_divider();

void print_padding() {
    std::cout << "\n";
    std::cout << "\n";
}

int main() {
    print_padding();
    std::cout << "main invoked 🏎\n";
    streams_divider();
    print_padding();
    std::cin.get();
}