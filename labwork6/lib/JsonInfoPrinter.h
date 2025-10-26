#pragma once

#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

#include <string>

#include "MyCache.h"

class JsonInfoPrinter {
public:
    JsonInfoPrinter() {}

    void GetInfoFromResponse();

    void SetResponse(const cpr::Response& response) {
        response_ = response;
    }

    void SetCodeFrom(const std::string& code_from) {
        json_cache_.SetCodeFrom(code_from);
    }
    void SetCodeTo(const std::string& code_to) {
        json_cache_.SetCodeTo(code_to);
    }
    void SetDate(const std::string& date) {
        json_cache_.SetDate(date);
    }

private:
    cpr::Response response_;
    JsonCache json_cache_;
};
