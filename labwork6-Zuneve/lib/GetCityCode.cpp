#include "GetCityCode.h"

std::string ReplaceSpacesToPercent20(const std::string& str) {
    std::string result;
    for (char c : str) {
        if (c == ' ') {
            result += "%20";
        } else {
            result += c;
        }
    }
    return result;
}

std::string GetCityCode(const std::string& city) {
    std::string formatted_city = ReplaceSpacesToPercent20(city);
    std::string url = "https://suggests.rasp.yandex.net/all_suggests?format=old&part=" + formatted_city;

    cpr::Response response = cpr::Get(cpr::Url{url});

    if (response.status_code == 200) {
        nlohmann::json json_response = nlohmann::json::parse(response.text);
        if (json_response.size() > 1 && !json_response[1].empty()
            && !json_response[1][0].empty()
            && !json_response[1][0][0].is_null()) {
                std::string code = json_response[1][0][0];
                return code;
        }
        return "";
    } else {
        std::cerr << "Error: " << response.status_code << std::endl;
        return "";
    }
}
