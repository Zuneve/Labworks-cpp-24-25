#pragma once

#include "Player.h"
#include <unordered_map>
#include <memory>
#include <algorithm>

class MasterPlayer : public Player {
private:
    uint64_t last_x_ = 0;
    uint64_t last_y_ = 0;
    std::string strategy_ = "custom";
    std::unordered_map<std::pair<uint64_t, uint64_t>, uint8_t, pair_hash> used_shots_;

public:
    MasterPlayer();

    explicit MasterPlayer(const std::shared_ptr<Field>& master_field);

    void SetStrategy(const std::string& new_strategy) override;

    std::pair<uint64_t, uint64_t> NextShot() override;


    std::pair<uint64_t, uint64_t> NearShoot(uint64_t& x, uint64_t& y) override;

    std::string GetStrategy() const override;
};
