#include "Request.h"
#include "MyCache.h"

cpr::Response JsonRequestHandler::GetResponse() const {
    cpr::Url url = "https://api.rasp.yandex.net/v3.0/search/";

    cpr::Parameters parameters = {
        {"from", code_from_},
        {"to", code_to_}, 
        {"date", date_}, 
        {"transfers", "true"},
        {"apikey", api_key_}
    };

    cpr::Response response = cpr::Get(url, parameters);

    return response;
}

void JsonRequestHandler::SwapDepartureArrival() {
    std::swap(code_from_, code_to_);
}

