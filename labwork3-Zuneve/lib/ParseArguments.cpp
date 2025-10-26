#include <fstream>
#include <iostream>

#include "MyStructs.h"
#include "ParseArguments.h"

void Error() {
    std::cout << "ERROR\nUse --help for usage information.\n";
    exit(EXIT_FAILURE);
}

void PrintHelp() {
    std::cout << "Usage: your_program [OPTIONS]\n"
            << "Options:\n"
            << "  --input=<file> или -i <file>    TSV-файл с данными\n"
            << "  --output=<dir> или -o <dir>     путь к директории для сохранения картинок\n"
            << "  --max-iter=<num> или -m <num>   максимальное количество итераций модели\n"
            << "  --freq=<num> или -f <num>       частота, с которой должны сохранятся картинки\n";
}

void IndicateInputFile(const char* input_path, Arguments* args) {
    args->input_file = fopen(input_path, "r");
    if (!args->input_file) {
        std::cerr << "Error opening input file!\n";
        Error();
    }
}

void SetArgument(Arguments* args, const std::pair<char, char*>& formatted_arg) {
    if (formatted_arg.first == 'i') {
        IndicateInputFile(formatted_arg.second, args);
    } else if (formatted_arg.first == 'o') {
        args->output_file = formatted_arg.second;
    } else if (formatted_arg.first == 'm') {
        args->max_iter = std::stoi(formatted_arg.second);
    } else {
        args->freq = std::stoi(formatted_arg.second);
    }
}

void HandleShortArgument(Arguments* args, int& i) {
    if (args->argv[i][1] == 'h' || i + 1 >= args->argc) {
        PrintHelp();
    }
    std::pair<char, char*> formatted_arg;
    formatted_arg = std::make_pair(args->argv[i][1], args->argv[i + 1]);
    ++i;
    SetArgument(args, formatted_arg);
}

std::pair<char, char*> TakeLongArgument(char* argument) {
    char* pos_equal = strchr(argument, '=');
    ++pos_equal;
    char* value = new char[strlen(pos_equal) + 1];
    strcpy(value, pos_equal);

    return std::make_pair(argument[2], value);
}

void HandleLongArgument(Arguments* args, int& i) {
    if (args->argv[i][2] == 'h') {
        PrintHelp();
    }
    std::pair<char, char*> formatted_arg;
    formatted_arg = TakeLongArgument(args->argv[i]);
    SetArgument(args, formatted_arg);
}

void SetArguments(Arguments* args) {
    for (int i = 1; i < args->argc; i++) {
        if (args->argv[i][1] == '-') {
            HandleLongArgument(args, i);
        } else {
            HandleShortArgument(args, i);
        }
    }
}
