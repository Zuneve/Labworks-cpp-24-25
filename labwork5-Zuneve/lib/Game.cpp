#include <iostream>
#include <algorithm>
#include "Game.h"
#include "Random.h"
#include <memory>
Game::Game() = default;

void Game::CreateMaster() {
    player_ = std::make_unique<MasterPlayer>();
    is_master_ = true;
}

void Game::CreateSlave() {
    player_ = std::make_unique<SlavePlayer>();
    is_master_ = false;
}

void Game::СonfigureGame() {
    width_ = 200000;
    height_ = 27;
    player_->GetField().SetDimensions(width_, height_);
    for (int i = 0; i < 4; i++) {
        ships_count_[i] = 200000;
    }
    player_->GetField().MasterPlaceShips();
}

std::pair<uint64_t, uint64_t> Game::MakeShot(std::pair<uint64_t, uint64_t>& last_hit) {
    uint64_t x = last_hit.first;
    uint64_t y = last_hit.second;
    if (player_->GetStrategy() == "custom") {
        while (has_fired_[{x, y}]) {
            x = (1ull * rnd() * rnd()) % width_;
            y = (1ull * rnd() * rnd()) % height_;
        }
        has_fired_[{x, y}] = 1;
        return {x, y};
        
    }
    return player_->NextShot();
}

bool Game::SetWidth(uint64_t w) {
    if (is_game_started_ || w == 0) return false;
    width_ = w;
    player_->GetField().SetDimensions(width_, height_);
    return true;
}

bool Game::SetHeight(uint64_t h) {
    if (is_game_started_ || h == 0) return false;
    height_ = h;
    player_->GetField().SetDimensions(width_, height_);
    return true;
}

bool Game::SetShipCount(uint8_t type, uint64_t count) {
    if (is_game_started_ || type < 1 || type > 4) return false;
    ships_count_[type - 1] = count;
    return true;
}

bool Game::Start(uint64_t& count_of_ships) {
    count_of_ships = TotalShips();
    if (is_game_started_ || width_ == 0 || height_ == 0 || count_of_ships == 0) return false;
    is_game_started_ = true;
    return true;
}

void Game::Stop() {
    is_game_started_ = false;
}

void Game::Dump(const std::string& path) {
    player_->GetField().SaveToFile(path);
}

void Game::SetStrategy(const std::string& strategy) {
    player_->SetStrategy(strategy);
}

bool Game::Load(const std::string& path) {
    if (is_game_started_) return false;
    player_->GetField().LoadFromFile(path, ships_count_);
    width_ = player_->GetField().GetWidth();
    height_ = player_->GetField().GetHeight();

    return true;
}

std::string Game::Shoot(uint64_t x, uint64_t y) {
    if (!is_game_started_) {
        return "failed";
    }
    return player_->GetField().Shoot(x, y);
}

bool Game::IsFinished(uint64_t count_of_killed_ships,
                      uint64_t count_of_ships) const {
    if (count_of_ships == count_of_killed_ships) {
        return true;
    }
    return player_->GetField().AllShipsSunk();
}

bool Game::IsWin(uint64_t count_of_killed_ships, uint64_t count_of_ships) const {
    return IsFinished(count_of_killed_ships, count_of_ships)
           && (count_of_ships == count_of_killed_ships);
}

bool Game::IsLose(uint64_t count_of_killed_ships, uint64_t count_of_ships) const {
    return IsFinished(count_of_killed_ships, count_of_ships) && player_->GetField().AllShipsSunk();
}

uint64_t Game::GetWidth() const {
    return width_;
}

uint64_t Game::GetHeight() const {
    return height_;
}

uint64_t Game::GetCount(uint8_t size_of_ship) const {
    return ships_count_[size_of_ship - 1];
}

bool Game::SetResult() const {
    return player_->GetField().IsHit();
}

void Game::PlaceShips() {
    player_->GetField().PlaceShips(ships_count_);
}

uint64_t Game::TotalShips() const {
    uint64_t total = 0;
    for (uint64_t i: ships_count_) {
        total += i;
    }
    return total;
}
