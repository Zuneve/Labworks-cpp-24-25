#pragma once

#include <unordered_map>
#include <string>
#include <memory>
#include <algorithm>
#include "Player.h"

class SlavePlayer : public Player {
private:
    uint64_t last_x_ = 0;
    uint64_t last_y_ = 0;
    std::string strategy_ = "custom";
    std::unordered_map<std::pair<uint64_t, uint64_t>, uint8_t, pair_hash> used_shots_;

public:
    SlavePlayer();

    explicit SlavePlayer(const std::shared_ptr<Field>& slaveField);

    std::pair<uint64_t, uint64_t> NextShot() override;

    void SetStrategy(const std::string& strategy) override;

    std::pair<uint64_t, uint64_t> NearShoot(uint64_t& x, uint64_t& y) override;

    std::string GetStrategy() const override;
};
