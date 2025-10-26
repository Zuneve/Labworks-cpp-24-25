#pragma once

#include <cstdint>
#include <string>
#include "Field.h"
#include <memory>
#include <algorithm>
class Player {
protected:
    std::shared_ptr<Field> field_;
    uint64_t width_;
    uint64_t height_;

public:
    Player() : field_(std::make_shared<Field>(0, 0)), width_(0), height_(0) {
    }

    explicit Player(const std::shared_ptr<Field>& player_field)
        : field_(player_field), width_(player_field->GetWidth()), height_(player_field->GetHeight()) {
    }

    virtual std::pair<uint64_t, uint64_t> NextShot() = 0;

    virtual void SetStrategy(const std::string& new_strategy) = 0;

    Field &GetField() const { return *field_; }

    virtual std::string GetStrategy() const = 0;

    virtual std::pair<uint64_t, uint64_t> NearShoot(uint64_t& x, uint64_t& y) = 0;

    bool IsValidCoordinate(uint64_t x, uint64_t y) const {
        return x < width_ && y < height_;
    }

    virtual ~Player() = default;
};
