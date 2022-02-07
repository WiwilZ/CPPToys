#include <iostream>
#include "random.h"


int main() {
    auto x = random_integer<uint16_t>();

    std::cout << x << std::endl;

    for (auto&& e: random_integer<uint64_t>(x)) {
        std::cout << e << " ";
    }
    std::cout << std::endl;

    std::cout << random_string(x) << std::endl;


    return 0;
}
    
