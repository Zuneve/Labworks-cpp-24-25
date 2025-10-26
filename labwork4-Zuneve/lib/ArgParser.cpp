#include "ArgParser.h"
#include "ArgSettings.h"
#include <iostream>
#include <sstream>

namespace ArgumentParser {
    ArgParser::ArgParser(const std::string &name) : parser_name_(name) {}

    std::string DefineArgumentName(const std::string& argument) {
        size_t eq_pos = argument.find('=');
        std::string name;
        bool is_short_arg = (argument[1] != '-');
        if (eq_pos != std::string::npos) {
            name = argument.substr(2 - is_short_arg, eq_pos - 2 + is_short_arg);
        } else {
            name = argument.substr(2 - is_short_arg);
        }

        return name;
    }

    bool ArgParser::ProcessArgument(const std::string& name,
        ArgumentSettings::Type type, int& i, int argc, const std::vector<std::string>& args) {
        ++i;
        bool have_values = false;
        while (i < argc && args[i][0] != '-') {
            if (type == ArgumentSettings::Type::String) { 
                args_[name].AddValue(args[i]);
            } else {
                int int_to_add = std::stoi(args[i]);
                args_[name].AddValue(int_to_add);
            }
            ++i;
            have_values = true;
            if (!args_[name].IsMultiValue()) {
                break;
            }
        }
        return have_values;
    }

    bool ArgParser::ProcessArgument(const std::string& name,
        ArgumentSettings::Type type, int& i, int argc, char** argv) {
        ++i;
        bool have_values = false;
        while (i < argc && argv[i][0] != '-') {
            if (type == ArgumentSettings::Type::String) {
                args_[name].AddValue(argv[i]);
            } else {
                int int_to_add = std::stoi(argv[i]);
                args_[name].AddValue(int_to_add);
            }
            ++i;
            have_values = true;
            if (!args_[name].IsMultiValue()) {
                break;
            }
        }
        return have_values;
    }

    bool ArgParser::ProcessLongArg(const std::vector<std::string>& args,
        int& i, bool& is_help_arg) {
        std::string name = DefineArgumentName(args[i]);
        args_[name].SetParameterParsed();
        if (!help_argument_.empty()) {
            if (name == short_to_long_[help_argument_]) {
                is_help_arg = true;
                return true;
            }
        }
        ArgumentSettings::Type type = args_[name].GetType();;
        if (type == ArgumentSettings::Type::Flag) {
            args_[name].AddValue(true);
            ++i;
            return true;
        }
        size_t eq_pos = args[i].find('=');
        if (eq_pos != std::string::npos) {
            std::string value = args[i].substr(eq_pos + 1);
            if (type == ArgumentSettings::Type::Int) {
                int val = std::stoi(value);
                args_[name].AddValue(val);
            } else {
                args_[name].AddValue(value);
            }
            ++i;
        } else {
            return ProcessArgument(name, type, i, args.size(), args);
        }
        return true;
    }

    bool ArgParser::ProcessLongArg(char** argv, int& i, int argc, bool& is_help_arg) {
        std::string current_arg = argv[i];
        std::string name = DefineArgumentName(argv[i]);
        args_[name].SetParameterParsed();
        if (!help_argument_.empty()) {
            if (name == short_to_long_[help_argument_]) {
                is_help_arg = true;
                return true;
            }
        }
        ArgumentSettings::Type type = args_[name].GetType();;
        if (type == ArgumentSettings::Type::Flag) {
            args_[name].AddValue(true);
            ++i;
            return true;
        }
        size_t eq_pos = current_arg.find('=');
        if (eq_pos != std::string::npos) {
            std::string value = current_arg.substr(eq_pos + 1);
            if (type == ArgumentSettings::Type::Int) {
                int val = std::stoi(value);
                args_[name].AddValue(val);
            } else {
                args_[name].AddValue(value);
            }
            ++i;
        } else {
            return ProcessArgument(name, type, i, argc, argv);
        }
        return true;
    }

    bool ArgParser::ProcessShortArg(const std::vector<std::string>& args,
        int& i, bool& is_help_arg) {
        size_t eq_pos = args[i].find('=');
        std::string name = DefineArgumentName(args[i]);
        name = short_to_long_[name];
        args_[name].SetParameterParsed();
        if (!help_argument_.empty()) {
            if (name == short_to_long_[help_argument_]) {
                is_help_arg = true;
                return true;
            }
        }
        ArgumentSettings::Type type = args_[name].GetType();
        if (type == ArgumentSettings::Type::Flag) {
            for (int j = 1; j < args[i].size(); ++j) {
                std::string str_j(1, args[i][j]);
                args_[short_to_long_[str_j]].AddValue(true);
                args_[short_to_long_[str_j]].SetParameterParsed();
            }
            ++i;
            return true;
        }
        if (eq_pos != std::string::npos) {
            std::string value = args[i].substr(eq_pos + 1);
            if (type == ArgumentSettings::Type::Int) {
                int val = std::stoi(value);
                args_[name].AddValue(val);
            } else {
                args_[name].AddValue(value);
            }
            ++i;
        } else {
            return ProcessArgument(name, type, i, args.size(), args);
        }
        return true;
    }

    bool ArgParser::ProcessShortArg(char** argv, int& i,
        int argc, bool& is_help_arg) {
        std::string current_arg = argv[i];
        size_t eq_pos = current_arg.find('=');

        std::string name = DefineArgumentName(argv[i]);
        name = short_to_long_[name];
        args_[name].SetParameterParsed();
        if (!help_argument_.empty()) {
            if (name == short_to_long_[help_argument_]) {
                is_help_arg = true;
                return true;
            }
        }
        ArgumentSettings::Type type = args_[name].GetType();
        if (type == ArgumentSettings::Type::Flag) {
            for (int j = 1; j < current_arg.size(); ++j) {
                std::string str_j(1, argv[i][j]);
                args_[short_to_long_[str_j]].AddValue(true);
                args_[short_to_long_[str_j]].SetParameterParsed();
            }
            ++i;
            return true;
        }
        if (eq_pos != std::string::npos) {
            std::string value = current_arg.substr(eq_pos + 1);
            if (type == ArgumentSettings::Type::Int) {
                int val = std::stoi(value);
                args_[name].AddValue(val);
            } else {
                args_[name].AddValue(value);
            }
            ++i;
        } else {
            return ProcessArgument(name, type, i, argc, argv);
        }
        return true;
    }

    void ArgParser::ProcessPositionalArgument(const std::vector<std::string>& args,
        int&i, bool &is_parsed) {

        if (last_positional_.empty()) {
            is_parsed = false;
        }
        args_[last_positional_].SetParameterParsed();
        ArgumentSettings::Type type = args_[last_positional_].GetType();
        if (type == ArgumentSettings::Type::String) {
            while (i < args.size() && args[i][0] != '-') {
                args_[last_positional_].AddValue(args[i]);
                ++i;
                if (!args_[last_positional_].IsMultiValue()) {
                    break;
                }
            }
        } else if (type == ArgumentSettings::Type::Int) {
            while (i < args.size() && args[i][0] != '-') {
                int int_to_add = std::stoi(args[i]);
                args_[last_positional_].AddValue(int_to_add);
                ++i;
                if (!args_[last_positional_].IsMultiValue()) {
                    break;
                }
            }
        }
    }


    void ArgParser::ProcessPositionalArgument(char** argv, int argc,
        int& i, bool &is_parsed) {

        if (last_positional_.empty()) {
            is_parsed = false;
        }
        args_[last_positional_].SetParameterParsed();
        ArgumentSettings::Type type = args_[last_positional_].GetType();
        if (type == ArgumentSettings::Type::String) {
            while (i < argc && argv[i][0] != '-') {
                args_[last_positional_].AddValue(argv[i]);
                ++i;
                if (!args_[last_positional_].IsMultiValue()) {
                    break;
                }
            }
        } else if (type == ArgumentSettings::Type::Int) {
            while (i < argc && argv[i][0] != '-') {
                int int_to_add = std::stoi(argv[i]);
                args_[last_positional_].AddValue(int_to_add);
                ++i;
                if (!args_[last_positional_].IsMultiValue()) {
                    break;
                }
            }
        }
    }

    bool ArgParser::Parse(const std::vector<std::string>& args) {
        if (args.empty()) {
            return false;
        }
        bool is_parsed = true;
        int i = 1;
        while (i < args.size()) {
            bool is_help_arg = false;
            if (args[i][0] == '-') {
                if (args[i][1] == '-') {
                    is_parsed &= ProcessLongArg(args, i, is_help_arg);
                } else {
                    is_parsed &= ProcessShortArg(args, i, is_help_arg);
                }
                if (is_help_arg) {
                    return true;
                }
            } else {
                ProcessPositionalArgument(args, i, is_parsed);
            }
        }
        for (const auto& [name, setting]: args_) {
            is_parsed &= setting.IsParamParsed();
            if (setting.IsMultiValue()) {
                is_parsed &= (setting.GetSize() >= setting.GetMinCount());
            }
        }

        return is_parsed;
    }

    bool ArgParser::Parse(int argc, char** argv) {
        if (argc == 0) {
            return false;
        }
        bool is_parsed = true;
        int i = 1;
        while (i < argc) {
            bool is_help_arg = false;
            if (argv[i][0] == '-') {
                if (argv[i][1] == '-') {
                    is_parsed &= ProcessLongArg(argv, i, argc, is_help_arg);
                } else {
                    is_parsed &= ProcessShortArg(argv, i, argc, is_help_arg);
                }
                if (is_help_arg) {
                    return true;
                }
            } else {
                ProcessPositionalArgument(argv, argc, i, is_parsed);
            }
        }
        for (const auto& [name, setting]: args_) {
            is_parsed &= setting.IsParamParsed();
            if (setting.IsMultiValue()) {
                is_parsed &= (setting.GetSize() >= setting.GetMinCount());
            }
        }

        return is_parsed;
    }

    bool ArgParser::Help() const {
        return have_add_help_;
    }

    bool ArgParser::GetFlag(const std::string str) {
        if (args_.count(str)) {
            return args_[str].GetBoolValue();
        }
        return false;
    }

    bool ArgParser::GetFlag(const char ch) {
        std::string str(1, ch);
        if (args_.count(str)) {
            return args_[str].GetBoolValue();
        }
        return false;
    }

    int ArgParser::GetIntValue(const std::string str, int ind) {
        if (args_.count(str)) {
            return args_[str].GetIntVal(ind);
        }
        return -1;
    }

    std::string ArgParser::GetStringValue(const std::string str, int ind) {
        if (args_.count(str)) {
            return args_[str].GetStringVal(ind);
        }
        return "";
    }


    ArgParser &ArgParser::AddStringArgument(const std::string& str, const std::string& description) {
        args_[str] = ArgumentSettings(ArgumentSettings::Type::String, description);
        last_added_ = str;
        return *this;
    }

    ArgParser &ArgParser::AddStringArgument(const char& ch, const std::string& str2, const std::string& description) {
        std::string str1(1, ch);
        args_[str2] = ArgumentSettings(ArgumentSettings::Type::String, description);
        short_to_long_[str1] = str2;
        long_to_short_[str2] = str1;
        last_added_ = str2;
        return *this;
    }

    ArgParser &ArgParser::AddIntArgument(const std::string& str, const std::string& description) {
        args_[str] = ArgumentSettings(ArgumentSettings::Type::Int, description);
        last_added_ = str;
        return *this;
    }

    ArgParser &ArgParser::AddIntArgument(const char& ch, const std::string& str2, const std::string& description) {
        std::string str1(1, ch);
        args_[str2] = ArgumentSettings(ArgumentSettings::Type::Int, description);
        short_to_long_[str1] = str2;
        long_to_short_[str2] = str1;
        last_added_ = str2;
        return *this;
    }

    ArgParser &ArgParser::AddFlag(const std::string& str, const std::string& description) {
        args_[str] = ArgumentSettings(ArgumentSettings::Type::Flag, description);
        last_added_ = str;
        return *this;
    }

    ArgParser &ArgParser::AddFlag(const char& ch, const std::string& str2, const std::string& description) {
        std::string str1(1, ch);
        args_[str2] = ArgumentSettings(ArgumentSettings::Type::Flag, description);
        args_[str2].SetParameterParsed();
        short_to_long_[str1] = str2;
        long_to_short_[str2] = str1;
        last_added_ = str2;
        return *this;
    }

    ArgParser &ArgParser::AddHelp(const char& ch, const std::string& str2, const std::string& description) {
        have_add_help_ = true;
        std::string str1(1, ch);
        help_argument_ = str1;
        return AddFlag(ch, str2, description);
    }


    ArgParser &ArgParser::MultiValue(size_t minimum_size) {
        if (!last_added_.empty() && args_.count(last_added_)) {
            args_[last_added_].SetMultiValue(minimum_size);
        }
        return *this;
    }

    ArgParser &ArgParser::StoreValues(std::vector<std::string>& container) {
        if (!last_added_.empty() && args_.count(last_added_)) {
            args_[last_added_].SetStoreValues(container);
        }
        return *this;
    }

    ArgParser &ArgParser::StoreValues(std::vector<int>& container) {
        if (!last_added_.empty() && args_.count(last_added_)) {
            args_[last_added_].SetStoreValues(container);
        }
        return *this;
    }

    ArgParser &ArgParser::StoreValue(std::string& value) {
        if (!last_added_.empty() && args_.count(last_added_)) {
            args_[last_added_].SetStoreValue(value);
        }
        return *this;
    }

    ArgParser &ArgParser::StoreValue(int& value) {
        if (!last_added_.empty() && args_.count(last_added_)) {
            args_[last_added_].SetStoreValue(value);
        }
        return *this;
    }

    ArgParser &ArgParser::StoreValue(bool& value) {
        if (!last_added_.empty() && args_.count(last_added_)) {
            args_[last_added_].SetStoreValue(value);
        }
        return *this;
    }

    ArgParser &ArgParser::Positional() {
        if (!last_added_.empty() && args_.count(last_added_)) {
            args_[last_added_].SetPositional();
            last_positional_ = last_added_;
        }
        return *this;
    }

    ArgParser &ArgParser::Default(const char* value) {
        if (!last_added_.empty() && args_.count(last_added_)) {
            args_[last_added_].SetDefaultValue(static_cast<std::string>(value));
        }
        return *this;
    }

    ArgParser &ArgParser::Default(const int& value) {
        if (!last_added_.empty() && args_.count(last_added_)) {
            args_[last_added_].SetDefaultValue(value);
        }
        return *this;
    }

    ArgParser &ArgParser::Default(const bool& value) {
        if (!last_added_.empty() && args_.count(last_added_)) {
            args_[last_added_].SetDefaultValue(value);
        }
        return *this;
    }

    
    std::string ArgParser::HelpDescription() {
        std::ostringstream oss;
        oss << parser_name_ << "\n";
        std::string full_help_arg = short_to_long_[help_argument_];
        oss << args_[full_help_arg].GetDescription() << "\n\n";
        
        for (const auto& [name, setting]: args_) {
            std::string full_name = name;
            if (name == full_help_arg) {
                continue;
            }
            if (long_to_short_.find(name) != long_to_short_.end()) {
                full_name = "-" + long_to_short_[name] + ",  --" + name;
            } else {
                full_name = "     --" + name;
            }

            oss << full_name;
            if (setting.GetType() == ArgumentSettings::Type::String) {
                oss << "=<string>";
            } else if (setting.GetType() == ArgumentSettings::Type::Int) {
                oss << "=<int>";
            }

            oss << ",  " << setting.GetDescription();

            if (setting.IsMultiValue()) {
                oss << " [repeated, min args = " << setting.GetMinCount() << "]";
            }
            oss << "\n";
        }

        oss << "\n";
        oss << '-' << help_argument_ << ", --" << full_help_arg << " Display this help and exit\n";
        return oss.str();

    }
} // namespace ArgumentParser
