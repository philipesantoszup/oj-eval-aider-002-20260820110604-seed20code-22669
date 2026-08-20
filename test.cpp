#include <iostream>
#include "int2048.h"

using namespace sjtu;

int main() {
    int2048 a("12345678901234567890");
    int2048 b("98765432109876543210");

    std::cout << "a = " << a << "\n";
    std::cout << "b = " << b << "\n";
    std::cout << "a + b = " << (a + b) << "\n";
    std::cout << "a - b = " << (a - b) << "\n";
    std::cout << "b - a = " << (b - a) << "\n";

    int2048 c("123456789");
    int2048 d("987654321");
    std::cout << "c * d = " << (c * d) << "\n";

    int2048 e("10"), f("3");
    std::cout << "10 / 3 = " << (e / f) << "\n";
    std::cout << "10 % 3 = " << (e % f) << "\n";

    int2048 g("-10");
    std::cout << "-10 / 3 = " << (g / f) << "\n";
    std::cout << "-10 % 3 = " << (g % f) << "\n";

    int2048 h("-3");
    std::cout << "10 / -3 = " << (e / h) << "\n";
    std::cout << "10 % -3 = " << (e % h) << "\n";

    std::cout << "-10 / -3 = " << (g / h) << "\n";
    std::cout << "-10 % -3 = " << (g % h) << "\n";

    return 0;
}
