add_library(MyMap 
            MyCache.cpp 
            GetCityCode.cpp 
            Request.cpp 
            JsonInfoPrinter.cpp 
            FlightScheduleManager.cpp
            CityInput.cpp)

target_link_libraries(MyMap PRIVATE 
        cpr::cpr
        nlohmann_json::nlohmann_json
        ftxui::screen ftxui::dom ftxui::component)
