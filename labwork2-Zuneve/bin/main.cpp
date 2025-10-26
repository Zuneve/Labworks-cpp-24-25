#include <lib/number.h>
#include <iostream>

int main() {
    uint239_t value = FromString("1", 0);
    for (int i = 0; i < 35; ++i) std::cout << (int)value.data[i];

    return 0;
}
