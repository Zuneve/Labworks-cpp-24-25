#include <nlohmann/json.hpp>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "MyCache.h"

void JsonCache::SaveRequest(const std::ostringstream& output) {
    std::string file_name = "cache";
    int possible_number_of_files = 30;
    for (int i = 0; i < possible_number_of_files; ++i) {
        std::string filename_i = file_name + std::to_string(i) + ".txt";
        if (!std::filesystem::exists(filename_i)) {
            file_name = filename_i;
            break;
        }
    }

    std::ofstream cache_file(file_name);
    if (cache_file.is_open()) {
        std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
        std::time_t cur_timestamp = std::chrono::system_clock::to_time_t(now);
        cache_file << date_ << ' ' << code_from_ << ' ' << code_to_ << '\n';
        cache_file << cur_timestamp << '\n';
        cache_file << output.str();
        cache_file.close();
    } else {
        std::cerr << "Error while opening file" << std::endl;
    }
}
