#pragma once

#include <cpr/cpr.h>

#include <string>

class JsonRequestHandler {
public:
    JsonRequestHandler() {}
    
    JsonRequestHandler(const std::string& api_key,
        const std::string& code_from, const std::string& code_to,
        const std::string& date) : 
        api_key_(api_key), code_from_(code_from), 
        code_to_(code_to), date_(date) {}

    cpr::Response GetResponse() const;

    void SwapDepartureArrival();

    std::string GetCodeFrom() const {
        return code_from_;
    }

    std::string GetCodeTo() const {
        return code_to_;
    }

    std::string GetDate() const {
        return date_;
    }

private:
    std::string api_key_;
    std::string code_from_;
    std::string code_to_;
    std::string date_;
};
