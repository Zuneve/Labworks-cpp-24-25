#pragma once

#include <nlohmann/json.hpp>

#include <iostream>

#include "Request.h"
#include "JsonInfoPrinter.h"

class YandexScheduleService {
public:
    YandexScheduleService() {}

    YandexScheduleService(const std::string& api_key,
        const std::string& code_from,
        const std::string& code_to,
        const std::string& date) : json_handler_(api_key, code_from, code_to, date) {}

    void run();
private:
    JsonRequestHandler json_handler_;
    JsonInfoPrinter json_info_printer_;
};
