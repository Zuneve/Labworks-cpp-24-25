#pragma once

#include "ArgSettings.h"
#include <string>
#include <unordered_map>
#include <vector>

namespace ArgumentParser {

    class ArgParser {
    public:
        explicit ArgParser(const std::string& name);

        bool Parse(const std::vector<std::string>& args);
        bool Parse(int argc, char** argv);

        ArgParser &AddStringArgument(const std::string& str, const std::string& description = "");
        ArgParser &AddStringArgument(const char& ch, const std::string& str2 = "", const std::string& description = "");
        ArgParser &AddIntArgument(const std::string& str, const std::string& description = "");
        ArgParser &AddIntArgument(const char& ch, const std::string& str2 = "", const std::string& description = "");
        ArgParser &AddFlag(const std::string& str, const std::string& description = "");
        ArgParser &AddFlag(const char& ch, const std::string& str2 = "", const std::string& description = "");
        ArgParser &AddHelp(const char& ch, const std::string& str2 = "", const std::string& description = "");

        ArgParser& MultiValue(size_t minimum_size = 0);
        ArgParser& StoreValues(std::vector<std::string>& container);
        ArgParser& StoreValues(std::vector<int>& container);
        ArgParser& StoreValue(std::string& value);
        ArgParser& StoreValue(int& value);
        ArgParser& StoreValue(bool& value);
        ArgParser& Positional();
        ArgParser& Default(const char* value);
        ArgParser& Default(const int& value);
        ArgParser& Default(const bool& value);

        bool GetFlag(const std::string str);
        bool GetFlag(const char ch);
        int GetIntValue(const std::string str, int ind = 0);
        std::string GetStringValue(const std::string str, int ind = 0);
        bool Help() const;
        std::string HelpDescription();

    private:
        bool ProcessLongArg(const std::vector<std::string>& args, int& i, bool& is_help_arg);
        bool ProcessLongArg(char** argv, int& i, int argc, bool& is_help_arg);
        bool ProcessShortArg(const std::vector<std::string>& args, int& i, bool& is_help_arg);
        bool ProcessShortArg(char** argv, int& i, int argc, bool& is_help_arg);
        bool ProcessArgument(const std::string& name,
            ArgumentSettings::Type type, int& i, int argc, const std::vector<std::string>& args);
        bool ProcessArgument(const std::string& name,
            ArgumentSettings::Type type, int& i, int argc, char** argv);
        void ProcessPositionalArgument(const std::vector<std::string>& args,
            int&i, bool &is_parsed);
        void ProcessPositionalArgument(char** argv, int argc,
            int&i, bool &is_parsed);
        bool have_add_help_ = false;
        std::unordered_map<std::string, ArgumentSettings> args_;
        std::unordered_map<std::string, std::string> short_to_long_;
        std::unordered_map<std::string, std::string> long_to_short_; //only for HelpDescription()
        std::string parser_name_;
        std::string last_added_;
        std::string last_positional_;
        std::string help_argument_;
    };

} // namespace ArgumentParser
