#pragma once

#include <random>
#include <chrono>
#include <climits>
#include <algorithm>
#include <memory>
static std::mt19937 rnd(std::chrono::steady_clock::now().time_since_epoch().count());
static double rng() { return (static_cast<double>(rnd()) / INT_MAX); }
