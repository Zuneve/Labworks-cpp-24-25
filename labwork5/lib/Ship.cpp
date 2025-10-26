#include "Ship.h"
#include <cstdio>
#include <sstream>
#include <string>
#include <memory>
#include <algorithm>
Ship::Ship() : size_(0), orientation('h'), x(0), y(0) {
}

Ship::Ship(uint8_t size, char orientation, uint64_t x, uint64_t y)
    : size_(size), orientation(orientation), x(x), y(y) {
}

std::string Ship::ToString() const {
    return std::to_string(size_) + " " + orientation + " " +
           std::to_string(x) + " " + std::to_string(y);
}

uint8_t Ship::GetSize() const {
    return size_;
}

void Ship::SetSize(uint8_t size) {
    size_ = size;
}

Ship Ship::FromString(const std::string& data) {
    uint8_t size;
    char orientation;
    uint64_t x, y;
    sscanf(data.c_str(), "%hhu %c %llu %llu", &size, &orientation, &x, &y);
    return Ship(size, orientation, x, y);
}
