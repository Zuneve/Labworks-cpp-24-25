#pragma once

#include <string>
#include <vector>

class ArgumentSettings {
public:
    enum class Type {
        String,
        Int,
        Flag
    };

    ArgumentSettings() : type_(Type::Flag), description_("") {
    }

    ArgumentSettings(Type type, const std::string& description)
        : type_(type), description_(description) {
    }

    ArgumentSettings& SetDefaultValue(const std::string& value) {
        default_string_value_ = value;
        is_parametr_parsed = true;
        return *this;
    }

    ArgumentSettings& SetDefaultValue(bool value) {
        default_bool_value_ = value;
        is_parametr_parsed = true;
        return *this;
    }

    ArgumentSettings& SetDefaultValue(int value) {
        default_int_value_ = value;
        is_parametr_parsed = true;
        return *this;
    }

    ArgumentSettings& SetMultiValue(size_t min_count = 0) {
        is_multi_value_ = true;
        min_count_ = min_count;
        return *this;
    }

    ArgumentSettings& SetPositional() {
        is_positional_ = true;
        return *this;
    }

    ArgumentSettings& SetStoreValues(std::vector<std::string>& container) {
        string_reference_container_ = &container;
        return *this;
    }

    ArgumentSettings& SetStoreValues(std::vector<int>& container) {
        int_reference_container_ = &container;
        return *this;
    }

    ArgumentSettings& SetStoreValue(std::string& value) {
        string_reference_ = &value;
        return *this;
    }

    ArgumentSettings& SetStoreValue(int& value) {
        int_reference_ = &value;
        return *this;
    }

    ArgumentSettings& SetStoreValue(bool& value) {
        bool_reference_ = &value;
        return *this;
    }

    ArgumentSettings& AddValue(const std::string& value) {
        if (is_multi_value_) {
            vector_size_++;
            if (string_reference_container_) {
                string_reference_container_->push_back(value);
            } else {
                if (string_container_ == nullptr) {
                    string_container_ = std::make_unique<std::vector<std::string> >();
                }
                string_container_->push_back(value);
            }
        } else {
            if (string_reference_) {
                *string_reference_ = value;
            } else {
                if (string_value_ == nullptr) {
                    string_value_ = std::make_unique<std::string>();
                }
                *string_value_ = value;
            }
        }
        return *this;
    }

    ArgumentSettings& AddValue(int value) {
        if (is_multi_value_) {
            vector_size_++;
            if (int_reference_container_) {
                int_reference_container_->push_back(value);
            } else {
                if (int_container_ == nullptr) {
                    int_container_ = std::make_unique<std::vector<int> >();
                }
                int_container_->push_back(value);
            }
        } else {
            if (int_reference_) {
                *int_reference_ = value;
            } else {
                if (int_value_ == nullptr) {
                    int_value_ = std::make_unique<int>();
                }
                *int_value_ = value;
            }
        }
        return *this;
    }

    ArgumentSettings& AddValue(bool value) {
        if (bool_reference_) {
            *bool_reference_ = value;
        } else {
            if (bool_value_ == nullptr) {
                bool_value_ = std::make_unique<bool>();
            }
            *bool_value_ = value;
        }
        return *this;
    }

    bool GetBoolValue() const {
        if (bool_reference_) {
            return *bool_reference_;
        }
        if (bool_value_ == nullptr) {
            return default_bool_value_;
        }
        return *bool_value_;
    }

    int GetIntVal(int index = 0) const {
        if (is_multi_value_) {
            if (int_reference_container_ && int_reference_container_->size() > index) {
                return (*int_reference_container_)[index];
            }
            if (int_container_ && int_container_->size() > index) {
                return (*int_container_)[index];
            }
            return default_int_value_;
        }
        if (int_reference_) {
            return *int_reference_;
        }
        if (int_value_ == nullptr) {
            return default_int_value_;
        }
        return *int_value_;
    }

    std::string GetStringVal(int index = 0) const {
        if (is_multi_value_) {
            if (string_reference_container_ && string_reference_container_->size() > index) {
                return (*string_reference_container_)[index];
            }
            if (string_container_ && string_container_->size() > index) {
                return (*string_container_)[index];
            }
            return default_string_value_;
        }
        if (string_reference_) {
            return *string_reference_;
        }
        if (string_value_ == nullptr) {
            return default_string_value_;
        }
        return *string_value_;
    }

    const std::string &GetDefaultValueString() const {
        return default_string_value_;
    }

    const std::string &GetDescription() const {
        return description_;
    }

    int GetSize() const {
        return vector_size_;
    }

    int GetDefaultValueInt() const {
        return default_int_value_;
    }

    void SetParameterParsed() {
        is_parametr_parsed = true;
    }

    bool IsParamParsed() const {
        return is_parametr_parsed;
    }

    bool IsMultiValue() const {
        return is_multi_value_;
    }

    bool IsPositional() const {
        return is_positional_;
    }

    bool GetDefaultValueBool() const {
        return default_bool_value_;
    }

    size_t GetMinCount() const {
        return min_count_;
    }

    Type GetType() const {
        return type_;
    }

private:
    Type type_;

    bool default_bool_value_ = false;
    bool is_positional_ = false;
    bool is_parametr_parsed = false;
    bool is_multi_value_ = false;

    bool* bool_reference_ = nullptr;

    int* int_reference_ = nullptr;

    int default_int_value_ = 0;
    int vector_size_ = 0;

    size_t min_count_ = 0;

    std::string description_;
    std::string default_string_value_;

    std::vector<std::string>* string_reference_container_ = nullptr;
    std::vector<int>* int_reference_container_ = nullptr;
    std::string* string_reference_ = nullptr;


    std::unique_ptr<std::vector<std::string> > string_container_ = nullptr;
    std::unique_ptr<std::vector<int> > int_container_ = nullptr;
    std::unique_ptr<std::string> string_value_ = nullptr;
    std::unique_ptr<int> int_value_ = nullptr;
    std::unique_ptr<bool> bool_value_ = nullptr;
};
