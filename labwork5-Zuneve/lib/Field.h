#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>
#include <unordered_map>
#include <memory>
#include "Ship.h"
#include "PairHash.h"
#include "Random.h"

class Field {
    uint64_t width_;
    uint64_t height_;
    std::vector<Ship> ships_;
    uint64_t last_x_;
    uint64_t last_y_;
    std::unordered_map<std::pair<uint64_t, uint64_t>, std::pair<uint8_t, bool>, pair_hash> has_fired_;

    void MarkHit(uint64_t x, uint64_t y);

    bool IsShipSunk(const Ship& ship);

    uint64_t calculate(Ship& cur_ship,
                       std::unordered_map<std::pair<uint64_t, uint64_t>, uint64_t, pair_hash>& coordinates,
                       uint64_t height, uint64_t width);

    void AddShip(Ship& ship, std::unordered_map<std::pair<uint64_t, uint64_t>, uint64_t, pair_hash>& coordinates);

    void DelShip(Ship& ship, std::unordered_map<std::pair<uint64_t, uint64_t>, uint64_t, pair_hash>& coordinates);

    void SetShip(Ship& ship, uint64_t height, uint64_t width);

    uint8_t FindSizeOfShip(uint64_t x, uint64_t y);

public:
    Field(uint64_t width, uint64_t height);

    bool IsHit();

    void MarkShipAsKilled(uint64_t x, uint64_t y);

    std::string Shoot(uint64_t x, uint64_t y);

    bool AllShipsSunk();

    bool IsOverlapping(const Ship& s1, const Ship& s2) const;

    void SaveToFile(const std::string& path);

    void SetDimensions(uint64_t width, uint64_t height);

    void LoadFromFile(const std::string& path, uint64_t *ship_counts);

    uint64_t SetCountIntersections(
        Ship& ship, std::unordered_map<std::pair<uint64_t, uint64_t>, uint64_t, pair_hash>& coordinates);

    bool OnlyRandomPlace(uint64_t n, const uint64_t ship_counts[4]);

    void PlaceShips(uint64_t ship_counts[]);

    void MasterPlaceShips();

    uint64_t GetWidth() const;

    uint64_t GetHeight() const;
};
