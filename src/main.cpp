#include <iostream>
#include "relocator.hpp"

int main() {
    if (relocate_if_needed()) {
        return 0;
    }

    std::cout << "Hello World" << std::endl;
    return 0;
}
