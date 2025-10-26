#include <algorithm>
#include <fstream>
#include <iostream>
#include <vector>

constexpr size_t max_str_size = (1 << 14);

struct Args {
    FILE *input_file;
    FILE *input_file_copy;
    std::ofstream output_file;
    std::vector<char *> lines;
    std::vector<std::pair<char, const char *>> command_line_arguments;
    int64_t start_time = 0;
    int64_t finish_time = LONG_LONG_MAX;
    int32_t stats_n = 10; // значение по умолчанию
};

struct InfoFromLog {
    int32_t code;
    char *request;
    int64_t time;
};


struct WindowData {
    int64_t max_requests = 0;
    int64_t time_l = 0;
    int64_t time_r = 0;
    int64_t l = 0;
    int64_t r = 0;
    int64_t ans_l = 0;
    int64_t ans_r = 0;
};

struct DateTime {
    int32_t day;
    int32_t month;
    int32_t year;
    int32_t hour;
    int32_t minute;
    int32_t second;
};

void Error() {
    std::cout << "ERROR\nUse --help for usage information.\n";
}

bool ContainsAll(const char *str, const char *chars) {
    size_t len = strlen(chars);
    for (size_t i = 0; i < len; ++i) {
        if (strchr(str, chars[i]) == nullptr) {
            return false;
        }
    }
    return true;
}

bool IsStringValid(const char *str) {
    const char *chars_to_find = "[]\"/-";
    return ContainsAll(str, chars_to_find);
}

void PrintHelpArg() {
    std::cout << "Usage: AnalyzeLog [OPTIONS] logs_filename\n"
            << "\nOptions:\n"
            << "  -o path, --output=path   Path to the file where error requests will be logged. If not specified, "
            "error request analysis is not performed.\n"
            << "  -p,   --print            Duplicate the error requests output to stdout.\n"
            << "  -s n, --stats=n          Show the top n most frequent requests with 5XX status codes. Default n is "
            "10.\n"
            << "  -w t, --window=t         Find and display the time window of t seconds with the maximum number of "
            "requests. Default is 0 (no calculation).\n"
            << "  -f t, --from=time        Start analyzing from the specified timestamp. Default is the earliest "
            "time in the log.\n"
            << "  -t t, --to=time          Stop analyzing at the specified timestamp. Default is the latest time in "
            "the log.\n"
            << "\nExample:\n"
            << "  AnalyzeLog --stats=2 --window=60 --from=805821284 --to=807117284 access.log\n"
            << "  AnalyzeLog -w 10 access.log\n"
            << "  AnalyzeLog -s 2 access.log\n";
}

void PrintHelpLog() {
    std::cout << "Logs are text args where each line represents a server access event in the following format:\n"
            << "\n"
            << "<remote_addr> - - [<local_time>] \"<request>\" <status> <bytes_send>\n"
            << "\n"
            << "Value        Description\n"
            << "remote_addr  The IP address from which the request was sent.\n"
            << "local_time   The time when the request was received.\n"
            << "request      The URL of the request.\n"
            << "status       The server's response status code.\n"
            << "bytes_send   The number of bytes sent in the response.\n"
            << "\nExample log lines:\n"
            << "198.112.92.15 - - [03/Jul/2024:10:50:02 -0400] \"GET /shuttle/countdown/HTTP/1.0\" 200 3985\n"
            << "198.112.92.15 - - [03/Jul/2024:10:50:04 -0400] \"GET /shuttle/nosuchpath/HTTP/1.0\" 404 144\n";
}

void PrintHelp() {
    PrintHelpArg();
    std::cout << "\n\n\n";
    PrintHelpLog();
    exit(0);
}

bool IsNumber(const char *s) {
    return (s != nullptr && strlen(s) > 0)
           && (strspn(s, "0123456789") == strlen(s));
}

int32_t MonthIndex(const char *month_str) {
    const char *months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    for (int32_t i = 0; i < 12; ++i) {
        if (strcmp(month_str, months[i]) == 0) {
            return i;
        }
    }
    return -1;
}

DateTime ParseDate(const char *date_str) {
    DateTime dt{};
    char month_str[4];
    sscanf(date_str, "%d/%3s/%d:%d:%d:%d", &dt.day, month_str, &dt.year, &dt.hour, &dt.minute, &dt.second);
    dt.month = MonthIndex(month_str);
    dt.year -= 1900;
    return dt;
}

int64_t DateTimeToTimestamp(const DateTime& dt) {
    std::tm time_info = {dt.second, dt.minute, dt.hour, dt.day, dt.month, dt.year};
    return timegm(&time_info);
}

int64_t ParseDateToTimestamp(const char *date_str) {
    DateTime dt = ParseDate(date_str);
    int64_t timestamp = DateTimeToTimestamp(dt);
    char timezone[6];
    strncpy(timezone, date_str + 21, 5);
    timezone[5] = '\0';
    int32_t hours = atoi(timezone + 1) / 100; // -0400 -> -400
    int32_t minutes = atoi(timezone + 3); // -0400 -> 00 == 0
    int64_t sec = hours * 3600 + minutes * 60;
    return timezone[0] == '+' ? timestamp - sec : timestamp + sec;
}

InfoFromLog GetInfoFromLog(const char *str) {
    InfoFromLog info{};
    //"198.112.92.15 - - [03/Jul/2024:10:50:04 -0400] \"GET /shuttle/nosuchpath/HTTP/1.0\" 404 144
    char remote_addr[256];
    char date[32];
    int32_t responseSize;
    sscanf(str, "%s - - [%[^]] %*c", remote_addr, date);
    const char *firstQuote = strchr(str, '\"');
    const char *lastQuote = strrchr(str, '\"');
    size_t length = lastQuote - firstQuote - 1;
    char *buffer = new char[length + 1];
    strncpy(buffer, firstQuote + 1, length);
    buffer[length] = '\0';
    info.request = buffer;
    const char *statusStart = strrchr(str, ' ') - 4;
    sscanf(statusStart, "%d %d", &info.code, &responseSize);
    info.time = ParseDateToTimestamp(date);
    return info;
}

bool IndicateOutputPath(const char *output_path, Args *args) {
    args->output_file.open(output_path);
    if (!args->output_file) {
        std::cerr << "Error opening output file!\n";
        Error();
        return false;
    }
    return true;
}

bool FindArguments(const std::pair<char, const char *>& formatted_arg, Args *args) {
    if (formatted_arg.first == 'o') {
        IndicateOutputPath(formatted_arg.second, args);
    } else if (formatted_arg.first == 'f') {
        if (!IsNumber(formatted_arg.second)) {
            Error();
            return false;
        }
        args->start_time = std::stoll(formatted_arg.second);
    } else if (formatted_arg.first == 't') {
        if (!IsNumber(formatted_arg.second)) {
            Error();
            return false;
        }
        args->finish_time = std::stoll(formatted_arg.second);
    } else if (formatted_arg.first == 's') {
        if (!IsNumber(formatted_arg.second)) {
            Error();
            return false;
        }
        args->stats_n = std::stoi(formatted_arg.second);
    }
    return true;
}

bool IsTimeCorrect(int64_t current_time, Args *args) {
    return current_time >= args->start_time && current_time <= args->finish_time;
}

void PrintErrors(Args *args) {
    fseek(args->input_file, 0, SEEK_SET);
    InfoFromLog info{};
    char *line = new char[max_str_size];

    while (fgets(line, max_str_size, args->input_file) != nullptr) {
        if (!IsStringValid(line)) {
            continue;
        }
        info = GetInfoFromLog(line);
        if (IsTimeCorrect(info.time, args) && info.code / 100 == 5) {
            std::cout << info.request << std::endl;
        }
    }
    free(line);
}

std::pair<char, char *> TakeLongArgument(char *& argument) {
    char *pos_equal = strchr(argument, '=');
    ++pos_equal;
    char *value = new char[strlen(pos_equal) + 1];
    strcpy(value, pos_equal);
    return std::make_pair(argument[2], value);
}

bool ComparePairs(const std::pair<char *, int32_t>& a, const std::pair<char *, int32_t>& b) {
    return a.second > b.second;
}

void UpdateRequestCount(const char *request, std::vector<std::pair<char *, int32_t>>& unique_requests) {
    for (auto& pair: unique_requests) {
        if (strcmp(pair.first, request) == 0) {
            pair.second++;
            return;
        }
    }
    unique_requests.emplace_back(strdup(request), 1);
}

void StatsN(Args *args) {
    fseek(args->input_file, 0, SEEK_SET);
    char *line = new char[max_str_size];
    std::vector<std::pair<char *, int32_t>> unique_requests;

    while (fgets(line, max_str_size, args->input_file)) {
        if (!IsStringValid(line)) {
            continue;
        }
        InfoFromLog info = GetInfoFromLog(line);
        if (IsTimeCorrect(info.time, args) && info.code / 100 == 5) {
            UpdateRequestCount(info.request, unique_requests);
        }
    }

    std::sort(unique_requests.begin(), unique_requests.end(), ComparePairs);
    for (int32_t i = 0; i < args->stats_n && i < unique_requests.size(); ++i) {
        args->output_file << unique_requests[i].first << std::endl;
    }

    free(line);
}


void MainLoopProcessInWindow(const char *line, char *line_copy,
                             WindowData& window_data, int32_t time_in_window, Args *args) {
    if (!IsStringValid(line)) {
        return;
    }
    window_data.time_r = GetInfoFromLog(line).time;
    if (window_data.r == 0) {
        fgets(line_copy, max_str_size, args->input_file_copy);
        if (!IsStringValid(line_copy)) {
            return;
        }
        window_data.l++;
        window_data.time_l = GetInfoFromLog(line_copy).time;
    }
    window_data.r++;
    if (!IsTimeCorrect(window_data.time_r, args)) {
        return;
    }

    while (!IsTimeCorrect(window_data.time_l, args) ||
           window_data.time_r - window_data.time_l >= time_in_window) {
        fgets(line_copy, max_str_size, args->input_file_copy);
        if (!IsStringValid(line_copy)) {
            continue;
        }
        window_data.l++;
        window_data.time_l = GetInfoFromLog(line_copy).time;
    }
    if (window_data.r - window_data.l + 1 > window_data.max_requests) {
        window_data.max_requests = window_data.r - window_data.l + 1;
        window_data.ans_l = window_data.time_l;
        window_data.ans_r = window_data.time_r;
    }
}


void Window(int32_t time_in_window, Args *args) {
    if (time_in_window == 0) {
        return;
    }
    fseek(args->input_file, 0, SEEK_SET);
    fseek(args->input_file_copy, 0, SEEK_SET);
    WindowData window_data;
    char *line = new char[max_str_size];
    char *line_copy = new char[max_str_size];
    while (fgets(line, max_str_size, args->input_file) != nullptr) {
        MainLoopProcessInWindow(line, line_copy, window_data, time_in_window, args);
    }
    free(line);
    free(line_copy);
    args->output_file << "MAX REQUESTS: " << window_data.max_requests << std::endl;
    args->output_file << "First window timestamp: " << window_data.ans_l << std::endl
            << "Last window timestamp: " << window_data.ans_r << std::endl;
}


bool ProcessOtherArguments(Args *args) {
    for (const std::pair<char, const char *>& argument: args->command_line_arguments) {
        if (argument.first == 'p') {
            PrintErrors(args);
        } else if (argument.first == 'w') {
            if (!IsNumber(argument.second)) {
                Error();
                return false;
            }
            Window(std::stoi(argument.second), args);
        }
    }
    StatsN(args);
    return true;
}


void HandleShortArgument(char *argument, int32_t& argument_counter, int32_t argc,
                         char **argv, Args *args) {
    std::pair<char, const char *> formatted_arg;
    if (argument[1] == 'h') {
        PrintHelp();
    } else if (argument[1] == 'p') {
        formatted_arg = std::make_pair('p', nullptr);
    } else {
        if (argument_counter + 1 < argc) {
            formatted_arg = std::make_pair(argument[1], argv[++argument_counter]);
        } else {
            PrintHelp();
        }
        FindArguments(formatted_arg, args);
    }

    args->command_line_arguments.push_back(formatted_arg);
}

void HandleLongArgument(char *argument, Args *args) {
    std::pair<char, const char *> formatted_arg;

    if (argument[2] == 'h') {
        PrintHelp();
    } else if (argument[2] == 'p') {
        formatted_arg = std::make_pair('p', nullptr);
    } else {
        formatted_arg = TakeLongArgument(argument);
    }

    FindArguments(formatted_arg, args);
    args->command_line_arguments.push_back(formatted_arg);
}

bool ParseArguments(int32_t argc, char **argv, Args *args) {
    char *argument;
    int32_t argument_counter = 1;

    while (argument_counter < argc - 1) {
        argument = argv[argument_counter];

        if (argument[1] != '-') {
            HandleShortArgument(argument, argument_counter, argc, argv, args);
        } else {
            HandleLongArgument(argument, args);
        }

        ++argument_counter;
    }
    return ProcessOtherArguments(args);
}

bool NeedsHelp(int32_t argc, char **argv) {
    return (argc == 2 &&
        (argv[1][0] == 'h' or
            argv[1][0] == '-' and argv[1][2] == 'h'));
}


signed main(int32_t argc, char **argv) {
    if (NeedsHelp(argc, argv)) {
        PrintHelp();
        return 0;
    }
    Args args;
    args.input_file = fopen(argv[argc - 1], "r");
    args.input_file_copy = fopen(argv[argc - 1], "r");
    if (args.input_file == nullptr) {
        std::cerr << "Error opening log file!" << std::endl;
        Error();
        return 0;
    }
    if (!ParseArguments(argc, argv, &args)) {
        return 0;
    }
    fclose(args.input_file);
    fclose(args.input_file_copy);
    args.output_file.close();
    return 0;
}
