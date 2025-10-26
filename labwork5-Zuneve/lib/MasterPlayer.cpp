#include "MasterPlayer.h"
#include <memory>
#include <algorithm>
MasterPlayer::MasterPlayer() : Player(), last_x_(0), last_y_(0) {
}

MasterPlayer::MasterPlayer(const std::shared_ptr<Field>& master_field) : Player(master_field) {
};


std::pair<uint64_t, uint64_t> MasterPlayer::NextShot() {
    last_x_++;
    if (last_x_ >= field_->GetWidth()) {
        last_x_ = 0;
    }
    if (last_y_ >= field_->GetHeight()) {
        last_y_ = 0;
        last_x_ = 0;
    }
    return {last_x_, last_y_};
}

void MasterPlayer::SetStrategy(const std::string& strategy) {
    strategy_ = strategy;
}

constexpr int px[] = {-3, -2, -1, 1, 2, 3, 0, 0, 0, 0, 0, 0};
constexpr int py[] = {0, 0, 0, 0, 0, 0, -3, -2, -1, 1, 2, 3};

std::pair<uint64_t, uint64_t> MasterPlayer::NearShoot(uint64_t& x, uint64_t& y) {
    uint64_t last_x_ = x;
    uint64_t last_y_ = y;
    for (int i = 0; i < 12; i++) {
        if (px[i] < 0 && -px[i] > x || py[i] < 0 && -py[i] > y) {
            continue;
        }
        uint64_t tx = px[i] + x;
        uint64_t ty = py[i] + y;
        if (tx >= field_->GetWidth() || ty >= field_->GetHeight() || used_shots_[{tx, ty}]) {
            continue;
        }
        x = last_x_;
        y = last_y_;
        return {tx, ty};
    }
    return {0, 0};
}

std::string MasterPlayer::GetStrategy() const {
    return strategy_;
}
