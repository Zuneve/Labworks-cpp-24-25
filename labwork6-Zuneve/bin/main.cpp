
#include <iostream>
#include <string>

#include "API_KEY.h"
#include <lib/CityInput.h>
#include <lib/FlightScheduleManager.h>
#include <lib/GetCityCode.h>

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cout << "Usage:\n\t" << argv[0] << " <DATE>\n";
        std::cout << "Example:\n\t" << argv[0] << " 2025-02-22" << std::endl;
        return 1;
    }
    std::string date = argv[1];
    std::string city1;
    std::string city2;

    GetCityInput(city1, city2);

    std::string code_first_city = GetCityCode(city1);
    std::string code_second_city = GetCityCode(city2);
    
    if (code_first_city.empty() || code_second_city.empty()) {
        std::cout << "One of the cities was not found." << std::endl;
        return 1;
    }

    YandexScheduleService app(api_key, code_first_city, code_second_city, date);
    app.run();
    return 0;
}
