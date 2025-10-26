#pragma once

#include <cpr/cpr.h>

class JsonCache {
public:
    JsonCache() {}
    void SaveRequest(const std::ostringstream& output);

    void SetCodeFrom(const std::string& code_from) {
        code_from_ = code_from;
    }
    void SetCodeTo(const std::string& code_to) {
        code_to_ = code_to;
    }
    void SetDate(const std::string& date) {
        date_ = date;
    }
    
private:
    std::string code_from_;
    std::string code_to_;
    std::string date_;
};
