#include "real_random.h"
#include <iostream>



int main() {
    size_t size = random_integer<uint8_t>();
    std::cout << size << std::endl;
    std::cout << random_string(size) << std::endl;
    for (auto e: random_integers<short>(10)) {
        std::cout << e << " ";
    }

    return 0;
}
