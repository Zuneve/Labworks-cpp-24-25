#pragma once
#include <algorithm>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include "Field.h"
#include "Player.h"
#include "MasterPlayer.h"
#include "SlavePlayer.h"

class Game {
    uint64_t width_ = 0;
    uint64_t height_ = 0;
    uint64_t ships_count_[4] = {0};
    bool is_game_started_ = false;
    std::unique_ptr<Player> player_;
    bool is_master_ = false;
    bool is_finished_ = false;
    std::unordered_map<std::pair<uint64_t, uint64_t>, uint8_t, pair_hash> has_fired_;

public:
    Game();

    void CreateMaster();

    void CreateSlave();

    void СonfigureGame();

    std::pair<uint64_t, uint64_t> MakeShot(std::pair<uint64_t, uint64_t> &last_hit);

    bool SetWidth(uint64_t w);

    bool SetHeight(uint64_t h);

    bool SetShipCount(uint8_t type, uint64_t count);

    bool Start(uint64_t& count_of_ships);

    void Stop();

    void Dump(const std::string &path);

    void SetStrategy(const std::string &strategy);

    bool Load(const std::string &path);

    std::string Shoot(uint64_t x, uint64_t y);

    bool IsFinished(uint64_t count_of_killed_ships,
        uint64_t count_of_ships) const ;

    bool IsWin(uint64_t count_of_killed_ships,
        uint64_t count_of_ships) const;

    bool IsLose(uint64_t count_of_killed_ships,
        uint64_t count_of_ships) const;

    uint64_t GetWidth() const;

    uint64_t GetHeight() const;

    uint64_t GetCount(uint8_t size_of_ship) const;

    bool SetResult() const;

    void PlaceShips();

private:
    uint64_t TotalShips() const;
};
