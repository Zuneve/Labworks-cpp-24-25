#include "JsonInfoPrinter.h"

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

struct SegmentContext {
    std::string departure_city;
    std::string arrival_city;
    std::string departure_time;
    std::string arrival_time;
    std::ostringstream& output;
};

std::string FormatDateTime(const std::string& iso_datetime) {
    std::string date = iso_datetime.substr(0, 10);
    std::string time = iso_datetime.substr(11, 5);
    return date + " " + time;
}

void ProcessTransferSegment(const nlohmann::json_abi_v3_11_3::json& segment,
                            const SegmentContext& context) {
    std::vector<std::string> transport_types = segment["transport_types"];
    std::string departure = segment["departure_from"]["title"];
    std::string transfer;
    std::string transfer_city;
    std::string transfer_time;
    std::string arrival = segment["arrival_to"]["title"];

    for (const auto& detail : segment["details"]) {
        if (detail.contains("transfer_from")) {
            transfer = detail["transfer_from"]["title"];
        }
        if (detail.contains("transfer_point")) {
            transfer_city = detail["transfer_point"]["title"];
        }
        if (detail.contains("arrival") && transfer_time.empty()) {
            transfer_time = FormatDateTime(detail["departure"]);
        }
    }

    context.output << "Transport types: \n1) " << transport_types[0] << '\n'
                   << "2) " << transport_types[1] << '\n';

    context.output << context.departure_city << ' ' << context.departure_time << " -> "
                   << transfer_city << ' ' << transfer_time << " -> "
                   << context.arrival_city << ' ' << context.arrival_time << '\n';

    context.output << departure << " -> " << transfer << " -> " << arrival << "\n\n";
}

void ProcessDirectSegment(const nlohmann::json_abi_v3_11_3::json& segment,
                          const SegmentContext& context) {
    std::string transport_type = segment["thread"].contains("transport_type")
                                     ? segment["thread"]["transport_type"]
                                     : "Unknown";
    context.output << "Transport type: " << transport_type << '\n';

    std::string from = segment["from"]["title"];
    std::string to = segment["to"]["title"];
    context.output << context.departure_time << " " << from << " -> " << to << ' '
                   << context.arrival_time << "\n\n";
}

void ProcessSegment(const nlohmann::json_abi_v3_11_3::json& segment,
                    const SegmentContext& context) {
    if (segment["has_transfers"] && segment["transfers"].size() > 1) {
        return;
    }

    std::string departure_time = FormatDateTime(segment["departure"]);
    std::string arrival_time = FormatDateTime(segment["arrival"]);

    SegmentContext updated_context = context;
    updated_context.departure_time = departure_time;
    updated_context.arrival_time = arrival_time;

    if (segment["has_transfers"]) {
        ProcessTransferSegment(segment, updated_context);
    } else {
        ProcessDirectSegment(segment, updated_context);
    }
}

void JsonInfoPrinter::GetInfoFromResponse() {
    std::ostringstream output;

    if (response_.status_code == 200) {
        nlohmann::json json_response = nlohmann::json::parse(response_.text);

        if (!json_response.contains("segments") || !json_response.contains("search")) {
            return;
        }

        SegmentContext context{
            json_response["search"]["from"]["title"],
            json_response["search"]["to"]["title"],
            "", // The departure time will be set later
            "", // The arrival time will be set later
            output
        };

        for (const auto& segment : json_response["segments"]) {
            ProcessSegment(segment, context);
        }

        std::cout << output.str();
        json_cache_.SaveRequest(output);
    } else {
        std::cout << "Error: " << response_.status_code << " - "
                  << response_.error.message << std::endl;
    }
}
