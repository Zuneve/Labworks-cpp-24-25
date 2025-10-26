#include <fstream>

#include "FlightScheduleManager.h"

int NumberFileRequest(const JsonRequestHandler& json_handler_) {
    std::string code_from = json_handler_.GetCodeFrom();
    std::string code_to = json_handler_.GetCodeTo();
    std::string date = json_handler_.GetDate();
    std::string file_name = "cache";
    int possible_number_of_files = 100;
    for (int i = 0; i < possible_number_of_files; ++i) {
        std::string file_name_i = file_name + std::to_string(i) + ".txt";
        if (!std::filesystem::exists(file_name_i)) {
            continue;
        }
        std::ifstream file_i(file_name_i);
        if (!file_i.is_open()) {
            std::cerr << "Error while opening file" << std::endl;
            return -1;
        }
        std::string file_date;
        std::string file_code_from;
        std::string file_code_to;
        long long file_time;
        file_i >> file_date >> file_code_from >> file_code_to;
        file_i >> file_time;
        file_i.close();
        std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
        std::time_t cur_timestamp = std::chrono::system_clock::to_time_t(now);
        if (file_date == date && file_code_from == code_from
            && file_code_to == code_to && 
            cur_timestamp - file_time < 7200) {
                return i;
        }
    }
    return -1;
}

void PrintInfoFromFile(const std::string& file_name) {
    std::ifstream cache_file(file_name);
    if (!cache_file.is_open()) {
        std::cerr << "Error while opening file" << std::endl;
        return;
    }
    std::string line;

    //skip the first two lines
    std::getline(cache_file, line);
    std::getline(cache_file, line);

    while(std::getline(cache_file, line)) {
        std::cout << line << std::endl;
    }

    cache_file.close();
}

void YandexScheduleService::run() {
    int num = NumberFileRequest(json_handler_);
    if (num != -1) {
        std::string file_name = "cache" + std::to_string(num) + ".txt";
        PrintInfoFromFile(file_name);
    } else {
        json_info_printer_.SetResponse(json_handler_.GetResponse());
        json_info_printer_.SetCodeFrom(json_handler_.GetCodeFrom());
        json_info_printer_.SetCodeTo(json_handler_.GetCodeTo());
        json_info_printer_.SetDate(json_handler_.GetDate());

        json_info_printer_.GetInfoFromResponse();
    }

    json_handler_.SwapDepartureArrival(); //way back
    
    num = NumberFileRequest(json_handler_);
    if (num != -1) {
        std::string file_name = "cache" + std::to_string(num) + ".txt";
        PrintInfoFromFile(file_name);
    } else {
        json_info_printer_.SetCodeFrom(json_handler_.GetCodeFrom());
        json_info_printer_.SetCodeTo(json_handler_.GetCodeTo());
        json_info_printer_.SetResponse(json_handler_.GetResponse());

        json_info_printer_.GetInfoFromResponse();
    }
    
    json_handler_.SwapDepartureArrival(); 

    json_info_printer_.SetCodeFrom(json_handler_.GetCodeFrom());
    json_info_printer_.SetCodeTo(json_handler_.GetCodeTo());
}
