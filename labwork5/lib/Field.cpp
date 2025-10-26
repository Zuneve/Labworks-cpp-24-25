#include "Field.h"
#include <string>
#include <memory>
#include <algorithm>

Field::Field(uint64_t width, uint64_t height)
    : width_(width), height_(height), last_x_(0), last_y_(0) {
}

void Field::MarkHit(uint64_t x, uint64_t y) {
    has_fired_[{x, y}].second = true;
}

bool Field::IsShipSunk(const Ship& ship) {
    return has_fired_[{ship.x, ship.y}].first == 2;
}

uint64_t Field::calculate(Ship& cur_ship,
                          std::unordered_map<std::pair<uint64_t, uint64_t>, uint64_t, pair_hash>& coordinates,
                          uint64_t height, uint64_t width) {
    uint64_t ans = 0;
    uint64_t x = cur_ship.x;
    uint64_t y = cur_ship.y;
    uint64_t size = cur_ship.GetSize();
    if (cur_ship.orientation == 'h') {
        if (x > 0) {
            ans += coordinates[{x - 1, y}];
            if (y > 0) {
                ans += coordinates[{x - 1, y - 1}];
            }
            if (y + 1 < height) {
                ans += coordinates[{x - 1, y + 1}];
            }
        }
        for (int k = 0; k < size; ++k) {
            if (y > 0) {
                ans += coordinates[{x + k, y - 1}];
            }
            ans += coordinates[{x + k, y}] - 1;

            if (y + 1 < height) {
                ans += coordinates[{x + k, y + 1}];
            }
        }
        if (x + size < width) {
            ans += coordinates[{x + size, y}];
            if (y > 0) {
                ans += coordinates[{x + size, y - 1}];
            }
            if (y + 1 < height) {
                ans += coordinates[{x + size, y + 1}];
            }
        }
    } else {
        if (y > 0) {
            ans += coordinates[{x, y - 1}];
            if (x > 0) {
                ans += coordinates[{x - 1, y - 1}];
            }
            if (x + 1 < width) {
                ans += coordinates[{x + 1, y - 1}];
            }
        }
        for (int k = 0; k < size; ++k) {
            if (x > 0) {
                ans += coordinates[{x - 1, y + k}];
            }
            ans += coordinates[{x, y + k}] - 1;

            if (x + 1 < width) {
                ans += coordinates[{x + 1, y + k}];
            }
        }
        if (y + size < height) {
            ans += coordinates[{x, y + size}];
            if (x > 0) {
                ans += coordinates[{x - 1, y + size}];
            }
            if (x + 1 < width) {
                ans += coordinates[{x + 1, y + size}];
            }
        }
    }
    return ans;
}


void Field::AddShip(Ship& ship,
                    std::unordered_map<std::pair<uint64_t, uint64_t>, uint64_t, pair_hash>& coordinates) {
    if (ship.orientation == 'h') {
        for (int k = 0; k < ship.GetSize(); ++k) {
            ++coordinates[{ship.x + k, ship.y}];
        }
    } else {
        for (int k = 0; k < ship.GetSize(); ++k) {
            ++coordinates[{ship.x, ship.y + k}];
        }
    }
}

void Field::DelShip(Ship& ship,
                    std::unordered_map<std::pair<uint64_t, uint64_t>, uint64_t, pair_hash>& coordinates) {
    if (ship.orientation == 'h') {
        for (int k = 0; k < ship.GetSize(); ++k) {
            --coordinates[{ship.x + k, ship.y}];
        }
    } else {
        for (int k = 0; k < ship.GetSize(); ++k) {
            --coordinates[{ship.x, ship.y + k}];
        }
    }
}

void Field::SetShip(Ship& ship, uint64_t height, uint64_t width) {
    ship.orientation = (rnd() & 1 ? 'h' : 'v');
    ship.x = std::min(static_cast<unsigned long long>((1ull * rnd() * rnd()) % width), static_cast<unsigned long long>(UINT64_MAX - 4));
    ship.y = std::min(static_cast<unsigned long long>((1ull * rnd() * rnd()) % height), static_cast<unsigned long long>(UINT64_MAX - 4));
    int count = 0;
    while (ship.orientation == 'h' && ship.x + ship.GetSize() > width
           || ship.orientation == 'v' && ship.y + ship.GetSize() > height) {
        ++count;
        if (count % 100 == 0) {
            ship.orientation = (ship.orientation == 'v' ? 'h' : 'v');
            continue;
        }
        ship.x = std::min(static_cast<unsigned long long>((1ull * rnd() * rnd()) % width), static_cast<unsigned long long>(UINT64_MAX - 4));
        ship.y = std::min(static_cast<unsigned long long>((1ull * rnd() * rnd()) % height), static_cast<unsigned long long>(UINT64_MAX - 4));
    }
}

uint8_t Field::FindSizeOfShip(uint64_t x, uint64_t y) {
    uint8_t size = 1;
    while (x > 0 && has_fired_[{x - 1, y}].second) {
        x--;
    }
    while (y > 0 && has_fired_[{x, y - 1}].second) {
        y--;
    }
    while (x + 1 < width_ && has_fired_[{x + 1, y}].second) {
        ++x;
        ++size;
    }
    while (y + 1 < height_ && has_fired_[{x, y + 1}].second) {
        ++y;
        ++size;
    }
    return size;
}

bool Field::IsHit() {
    return has_fired_[{last_x_, last_y_}].first == 2;
}

void Field::MarkShipAsKilled(uint64_t x, uint64_t y) {
    while (x + 1 < width_ && has_fired_[{x + 1, y}].first) {
        ++x;
    }
    while (y + 1 < height_ && has_fired_[{x, y + 1}].first) {
        ++y;
    }
    has_fired_[{x, y}].first = 2;
    while (true) {
        if (x > 0 && has_fired_[{x - 1, y}].second) {
            x--;
            has_fired_[{x, y}].first = 2;
        } else if (y > 0 && has_fired_[{x, y - 1}].second) {
            y--;
            has_fired_[{x, y}].first = 2;
        } else {
            break;
        }
    }
}

std::string Field::Shoot(uint64_t x, uint64_t y) {
    if (has_fired_[{x, y}].first == 2) {
        return "kill";
    }
    uint64_t last_x = x;
    uint64_t last_y = y;
    while (last_x + 1 < width_ && has_fired_[{last_x + 1, last_y}].first) {
        ++last_x;
    }
    while (last_y + 1 < height_ && has_fired_[{last_x, last_y + 1}].first) {
        ++last_y;
    }
    if (has_fired_[{x, y}].second) {
        bool is_horisontal = false;
        while (x > 0 && has_fired_[{x - 1, y}].second
               && has_fired_[{x - 1, y}].first) {
            x--;
            is_horisontal = true;
        }
        while (y > 0 && has_fired_[{x, y - 1}].second) {
            y--;
        }
        uint8_t size_of_ship = FindSizeOfShip(x, y);
        bool has_killed = size_of_ship == std::max(last_x - x + 1,
                                                   last_y - y + 1);
        while (true) {
            if (!has_fired_[{x, y}].second) {
                break;
            }
            if (!has_fired_[{x, y}].first) {
                has_fired_[{x, y}].first = 1;
                break;
            }
            if (is_horisontal) {
                x++;
            } else {
                y++;
            }
        }
        if (has_killed) {
            MarkShipAsKilled(x, y);
            return "kill";
        }
        return "hit";
    }
    return "miss";
}

bool Field::AllShipsSunk() {
    for (auto& ship: ships_) {
        if (!IsShipSunk(ship)) {
            return false;
        }
    }
    return true;
}

bool Field::IsOverlapping(const Ship& s1, const Ship& s2) const {
    uint64_t s1_x_end = s1.orientation == 'h' ? s1.x + s1.GetSize() - 1 : s1.x;
    uint64_t s1_y_end = s1.orientation == 'v' ? s1.y + s1.GetSize() - 1 : s1.y;
    uint64_t s2_x_end = s2.orientation == 'h' ? s2.x + s2.GetSize() - 1 : s2.x;
    uint64_t s2_y_end = s2.orientation == 'v' ? s2.y + s2.GetSize() - 1 : s2.y;
    return !(s1_x_end < s2.x || s2_x_end < s1.x ||
             s1_y_end < s2.y || s2_y_end < s1.y);
}

void Field::SaveToFile(const std::string& path) {
    std::ofstream outFile(path);
    if (!outFile) {
        std::cerr << "Failed to open file for writing: " << path << std::endl;
        return;
    }

    outFile << width_ << " " << height_ << "\n";

    for (const auto& ship: ships_) {
        outFile << ship.ToString() << "\n";
    }

    outFile.close();
}

void Field::LoadFromFile(const std::string& path,
                         uint64_t *ship_counts) {
    std::ifstream inFile(path);
    if (!inFile) {
        std::cerr << "Failed to open file for reading: " << path << std::endl;
        return;
    }

    inFile >> width_ >> height_;

    ships_.clear();
    std::string line;
    std::getline(inFile, line);
    while (std::getline(inFile, line)) {
        ships_.push_back(Ship::FromString(line));
        ship_counts[ships_.back().GetSize() - 1]++;
        has_fired_[{ships_.back().x, ships_.back().y}].second = true;
        if (ships_.back().orientation == 'h') {
            for (int i = 0; i < ships_.back().GetSize(); i++) {
                has_fired_[{ships_.back().x + i, ships_.back().y}].second = true;
            }
        } else {
            for (int i = 0; i < ships_.back().GetSize(); i++) {
                has_fired_[{ships_.back().x, ships_.back().y + i}].second = true;
            }
        }
    }

    inFile.close();
}


void Field::SetDimensions(uint64_t width, uint64_t height) {
    width_ = width;
    height_ = height;
}

uint64_t Field::SetCountIntersections(Ship& ship,
                                      std::unordered_map<std::pair<uint64_t, uint64_t>, uint64_t, pair_hash>&
                                      coordinates) {
    uint64_t count = 0;
    if (ship.orientation == 'h') {
        for (int k = 0; k < ship.GetSize(); ++k) {
            count += coordinates[{ship.x + k, ship.y}] - 1;
        }
    } else {
        for (int k = 0; k < ship.GetSize(); ++k) {
            count += coordinates[{ship.x, ship.y + k}] - 1;
        }
    }
    return count;
}

bool Field::OnlyRandomPlace(uint64_t n, const uint64_t ship_counts[4]) {
    std::vector<Ship> ships(n);
    uint64_t f_old = INT64_MAX;

    while (f_old > 0) {
        std::unordered_map<std::pair<uint64_t, uint64_t>, uint64_t, pair_hash> coordinates;
        uint64_t cnt = 0;

        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < ship_counts[i]; ++j) {
                ships[cnt].SetSize(i + 1);
                SetShip(ships[cnt], height_, width_);
                AddShip(ships[cnt], coordinates);
                int count = 0;
                uint64_t count_intersections = SetCountIntersections(ships[cnt], coordinates);
                while (count_intersections) {
                    count++;
                    if (count > 100) {
                        return false;
                    }
                    DelShip(ships[cnt], coordinates);
                    SetShip(ships[cnt], height_, width_);
                    count_intersections = SetCountIntersections(ships[cnt], coordinates);
                }
                cnt++;
            }
        }
        uint64_t f_new = 0;
        for (int i = 0; i < n; i++) {
            f_new += calculate(ships[i], coordinates, height_, width_);
        }
        f_old = f_new;
    }
    for (Ship& ship: ships) {
        if (ship.orientation == 'h') {
            for (int k = 0; k < ship.GetSize(); ++k) {
                has_fired_[{ship.x + k, ship.y}].second = true;
            }
        } else {
            for (int k = 0; k < ship.GetSize(); ++k) {
                has_fired_[{ship.x, ship.y + k}].second = true;
            }
        }
    }
    ships_ = ships;
    return true;
}

void Field::MasterPlaceShips() {
    std::vector<Ship> ships(800000);
    int cnt = 0;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 100000; ++j) {
            ships[cnt].SetSize(i + 1);
            ships[cnt].orientation = 'v';
            ships[cnt].x = j * 2;
            if (i == 0) {
                ships[cnt].y = 12;
            } else if (i == 1) {
                ships[cnt].y = 9;
            } else if (i == 2) {
                ships[cnt].y = 5;
            } else {
                ships[cnt].y = 0;
            }
            for (int k = 0; k <= i; ++k) {
                has_fired_[{ships[cnt].x, ships[cnt].y + k}].second = true;
            }
            cnt++;
        }
    }
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 100000; ++j) {
            ships[cnt].SetSize(i + 1);
            ships[cnt].orientation = 'v';
            ships[cnt].x = j * 2;
            if (i == 0) {
                ships[cnt].y = 26;
            } else if (i == 1) {
                ships[cnt].y = 23;
            } else if (i == 2) {
                ships[cnt].y = 19;
            } else {
                ships[cnt].y = 14;
            }
            for (int k = 0; k <= i; ++k) {
                has_fired_[{ships[cnt].x, ships[cnt].y + k}].second = true;
            }
            cnt++;
        }
    }
    ships_ = ships;
}

void Field::PlaceShips(uint64_t ship_counts[]) {
    uint64_t n = 0;
    for (int i = 0; i < 4; i++) {
        n += ship_counts[i];
    }
    if (n > 100) {
        int attempt_number = 0;
        while (!OnlyRandomPlace(n, ship_counts) == false || attempt_number < 100) {
            attempt_number++;
        }
        return;
    }
    std::vector<Ship> ships(n);
    uint64_t f_old = INT64_MAX;

    while (f_old > 0) {
        std::unordered_map<std::pair<uint64_t, uint64_t>, uint64_t, pair_hash> coordinates;
        uint64_t cnt = 0;
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < ship_counts[i]; ++j) {
                ships[cnt].SetSize(i + 1);
                SetShip(ships[cnt], height_, width_);
                AddShip(ships[cnt], coordinates);
                cnt++;
            }
        }
        uint64_t f_new = 0;
        for (int i = 0; i < n; i++) {
            f_new += calculate(ships[i], coordinates, height_, width_);
        }
        f_old = f_new;
        double t = 1;
        while (f_old > 0 && t > (1e-4)) {
            t *= 0.99;
            uint64_t i = (1ll * rnd() * rnd()) % n;

            uint64_t prev_x = ships[i].x;
            uint64_t prev_y = ships[i].y;
            char prev_orientation = ships[i].orientation;
            DelShip(ships[i], coordinates);

            SetShip(ships[i], height_, width_);
            AddShip(ships[i], coordinates);
            f_new = 0;
            for (int s = 0; s < n; s++) {
                f_new += calculate(ships[s], coordinates, height_, width_);
            }
            if (f_new < f_old || rng() < exp((static_cast<double>(f_old - f_new) / t))) {
                f_old = f_new;
            } else {
                DelShip(ships[i], coordinates);
                ships[i].x = prev_x;
                ships[i].y = prev_y;
                ships[i].orientation = prev_orientation;
                AddShip(ships[i], coordinates);
            }
        }
    }
    for (Ship& ship: ships) {
        if (ship.orientation == 'h') {
            for (int k = 0; k < ship.GetSize(); ++k) {
                has_fired_[{ship.x + k, ship.y}].second = true;
            }
        } else {
            for (int k = 0; k < ship.GetSize(); ++k) {
                has_fired_[{ship.x, ship.y + k}].second = true;
            }
        }
    }
    ships_ = ships;
}

uint64_t Field::GetWidth() const {
    return width_;
}

uint64_t Field::GetHeight() const {
    return height_;
}
