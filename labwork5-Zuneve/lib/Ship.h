#pragma once

#include <string>
#include <cstdint>
#include <memory>
#include <algorithm>
class Ship {
public:
    char orientation;
    uint64_t x;
    uint64_t y;
    uint8_t size_;

    Ship();

    Ship(uint8_t size, char orientation, uint64_t x, uint64_t y);

    std::string ToString() const;

    static Ship FromString(const std::string& data);

    uint8_t GetSize() const;

    void SetSize(uint8_t size);
};
