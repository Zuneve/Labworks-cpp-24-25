#include "number.h"

size_t GetStringLength(const char* str) {
    size_t length = 0;
    while (str[length] != '\0') {
        ++length;
    }

    return length;
}

uint239_t SetShiftBits(const uint239_t &value, uint32_t shift) {
    // добавляем служебные биты, не делая сдвиг
    uint239_t num_from_value = value;
    for (int8_t byte = 34; byte >= 0; --byte) {
        uint8_t cur_bit_shift = shift % 2;
        if (cur_bit_shift) {
            num_from_value.data[byte] |= 1 << 7;
        }
        shift /= 2;
    }

    return num_from_value;
}

uint64_t GetShift(const uint239_t &value) {
    //Получаем значение сдвига
    uint64_t shift_value = 0;
    for (int8_t byte = 34; byte >= 0; --byte) {
        shift_value += ((value.data[byte] >> 7) & 1) << (34 - byte);
    }

    return shift_value;
}

uint239_t MakeShift(const uint239_t &value, uint32_t shift) {
    //Сдвигаем число по заданному shift
    uint239_t result = value;
    for (int k = 0; k < shift % 245; ++k) {
        // k - k-ый сдвиг на 1 влево
        uint8_t last_to_insert_prev = 0, last_to_insert_cur = 0;
        for (int8_t byte = 34; byte >= 0; --byte) {
            //result.data[byte] = abcdefgh
            last_to_insert_cur = (result.data[byte] >> 6) & 1; // b
            result.data[byte] = result.data[byte] << 1; //abcdefgh -> bcdefgh0
            result.data[byte] |= last_to_insert_prev; // bcdefgh0 -> bcdefgh + prev_b
            last_to_insert_prev = last_to_insert_cur;
        }
        result.data[34] |= last_to_insert_prev;
    }
    for (int8_t byte = 34; byte >= 0; --byte) {
        result.data[byte] &= ~(1 << 7);
    }

    return SetShiftBits(result, shift); // устанавливаем служебные биты
}

uint239_t FromInt(uint32_t value, uint32_t shift) {
    uint239_t num_from_value;
    for (int8_t byte = 34; byte >= 0; --byte) {
        num_from_value.data[byte] = 0;
    }
    uint32_t copy_shift = shift;
    for (int8_t byte = 34; byte >= 0; --byte) {
        for (uint8_t bit = 0; bit < 7; ++bit) {
            uint8_t cur_bit = value % 2;
            if (cur_bit) {
                num_from_value.data[byte] |= (1 << bit);
            }
            value /= 2;
        }
        uint8_t cur_bit_shift = shift % 2;
        if (cur_bit_shift) {
            num_from_value.data[byte] |= (1 << 7);
        }
        shift /= 2;
    }

    return MakeShift(num_from_value, copy_shift);
}

uint239_t GetNumWithoutShift(const uint239_t &value) {
    //We get the number in normal form without shift and service bits
    uint239_t result = value;
    uint32_t shift = GetShift(value);
    for (int k = 0; k < shift % 245; ++k) {
        // k - k-ый сдвиг на 1 вправо
        uint8_t last_to_insert_prev = 0, last_to_insert_cur = 0;
        for (int8_t byte = 0; byte < 35; ++byte) {
            //value.data[byte] = abcdefgh
            last_to_insert_cur = result.data[byte] & 1; // h
            result.data[byte] = result.data[byte] >> 1; // abcdefgh -> 0abcdefg
            result.data[byte] &= ~(1 << 6); // 0abcdefg -> 00bcdefg
            if (last_to_insert_prev) {
                result.data[byte] |= 1 << 6; // 00bcdefg -> 01bcdefg if last_to_insert_prev
            }
            last_to_insert_prev = last_to_insert_cur;
        }
        if (last_to_insert_prev) {
            result.data[0] |= 1 << 6;
        }
    }
    for (int8_t byte = 34; byte >= 0; --byte) {
        result.data[byte] &= ~(1 << 7);
    }

    return result; // БЕЗ СДВИГА И БИТОВ СДВИГА
}

uint239_t operator+(const uint239_t &lhs, const uint239_t &rhs) {
    uint239_t result = FromInt(0, 0);
    uint64_t shift = GetShift(lhs) + GetShift(rhs);
    uint239_t num1 = GetNumWithoutShift(lhs);
    uint239_t num2 = GetNumWithoutShift(rhs);
    uint8_t add_bit = 0;
    for (int8_t byte = 34; byte >= 0; --byte) {
        for (int bit = 0; bit < 7; ++bit) {
            uint8_t lhs_bit = (num1.data[byte] >> bit) & 1;
            uint8_t rhs_bit = (num2.data[byte] >> bit) & 1;
            if (lhs_bit && rhs_bit) {
                result.data[byte] |= (add_bit << bit);
                add_bit = 1;
            } else if (lhs_bit != rhs_bit && !add_bit) {
                result.data[byte] |= (1 << bit);
                add_bit = 0;
            } else if (!lhs_bit && !rhs_bit) {
                result.data[byte] |= (add_bit << bit);
                add_bit = 0;
            }
        }
    }

    return MakeShift(result, shift);
}

uint239_t operator-(const uint239_t &lhs, const uint239_t &rhs) {
    uint239_t result = FromInt(0, 0);
    uint32_t shift1 = GetShift(lhs);
    uint32_t shift2 = GetShift(rhs);
    uint64_t shift;
    if (shift1 >= shift2) {
        shift = shift1 - shift2;
    } else {
        shift = (1ll << 35ll) - (shift2 - shift1);
    }
    uint239_t num1 = GetNumWithoutShift(lhs);
    uint239_t num2 = GetNumWithoutShift(rhs);
    uint8_t sub_bit = 0;
    for (int8_t byte = 34; byte >= 0; --byte) {
        for (int bit = 0; bit < 7; ++bit) {
            uint8_t lhs_bit = (num1.data[byte] >> bit) & 1;
            uint8_t rhs_bit = (num2.data[byte] >> bit) & 1;
            int32_t dif = lhs_bit - rhs_bit - sub_bit;
            if (dif < 0) {
                dif += 2;
                sub_bit = 1;
            } else {
                sub_bit = 0;
            }
            result.data[byte] |= dif << bit;
        }
    }

    return MakeShift(result, shift);
}

uint239_t operator*(const uint239_t &lhs, const uint239_t &rhs) {
    uint239_t result = FromInt(0, 0);
    uint64_t shift = (GetShift(lhs) + GetShift(rhs)) % (1ll << 35);
    uint239_t num1 = GetNumWithoutShift(lhs);
    uint239_t num2 = GetNumWithoutShift(rhs);
    uint64_t temp[71];
    for (uint8_t byte = 0; byte < 71; ++byte) {
        temp[byte] = 0;
    }
    uint8_t base = 128;
    for (int8_t byte1 = 34; byte1 >= 0; --byte1) {
        for (int8_t byte2 = 34; byte2 >= 0; --byte2) {
            uint64_t mul = 1ll * num1.data[byte1] * num2.data[byte2] + temp[69 - byte1 - byte2];
            temp[69 - byte1 - byte2] = mul % base;
            temp[70 - byte1 - byte2] += mul / base; //temp[69 - byte1 - byte2 + 1]
        }
    }
    for (uint8_t byte = 1; byte < 36; ++byte) {
        temp[byte + 1] += temp[byte] / base;
        result.data[35 - byte] = temp[byte] % base;
    }

    return MakeShift(result, shift);
}

uint239_t FromString(const char* str, uint32_t shift) {
    uint239_t result = FromInt(0, 0);
    uint239_t ten = FromInt(10, 0);
    for (int cur_char_index = 0; cur_char_index < GetStringLength(str); ++cur_char_index) {
        result = result * ten;
        result = result + FromInt(str[cur_char_index] - '0', 0);
    }

    return MakeShift(result, shift);
}


bool operator==(const uint239_t &lhs, const uint239_t &rhs) {
    uint239_t first_num = GetNumWithoutShift(lhs);
    uint239_t second_num = GetNumWithoutShift(rhs);
    for (uint8_t byte = 0; byte < 35; ++byte) {
        if (first_num.data[byte] != second_num.data[byte]) {
            return false;
        }
    }

    return true;
}

bool operator!=(const uint239_t &lhs, const uint239_t &rhs) {
    uint239_t first_num = GetNumWithoutShift(lhs);
    uint239_t second_num = GetNumWithoutShift(rhs);
    for (uint8_t byte = 0; byte < 35; ++byte) {
        if (first_num.data[byte] != second_num.data[byte]) {
            return true;
        }
    }

    return false;
}

bool operator<(const uint239_t &lhs, const uint239_t &rhs) {
    // lhs < rhs
    uint239_t first_num = GetNumWithoutShift(lhs);
    uint239_t second_num = GetNumWithoutShift(rhs);
    for (uint8_t byte = 0; byte < 35; ++byte) {
        if (first_num.data[byte] != second_num.data[byte]) {
            return first_num.data[byte] < second_num.data[byte];
        }
    }

    return false;
}

bool operator>(const uint239_t &lhs, const uint239_t &rhs) {
    // lhs > rhs
    uint239_t first_num = GetNumWithoutShift(lhs);
    uint239_t second_num = GetNumWithoutShift(rhs);
    for (uint8_t byte = 0; byte < 35; ++byte) {
        if (first_num.data[byte] != second_num.data[byte]) {
            return first_num.data[byte] > second_num.data[byte];
        }
    }

    return false;
}

uint239_t operator/(const uint239_t &lhs, const uint239_t &rhs) {
    uint239_t result = FromInt(0, 0);
    uint32_t shift1 = GetShift(lhs);
    uint32_t shift2 = GetShift(rhs);
    uint64_t shift;
    if (shift1 >= shift2) {
        shift = shift1 - shift2;
    } else {
        shift = (1ll << 35ll) - (shift2 - shift1);
    }
    uint239_t num1 = GetNumWithoutShift(lhs);
    uint239_t num2 = GetNumWithoutShift(rhs);
    if (result == num2) {
        std::cerr << "Division by zero" << '\n';
        exit(1);
    }
    uint239_t current = FromInt(0, 0);
    uint239_t base = FromInt(128, 0);
    int byte = 0;
    while (byte < 35) {
        while (byte < 35 && current < num2) {
            current = current * base;
            current = current + FromInt(num1.data[byte++], 0);
        }
        uint8_t cnt = 0;
        while (current > num2 || current == num2) {
            cnt++;
            current = current - num2;
        }
        result = result * base;
        result.data[34] = cnt;
    }

    return MakeShift(result, shift);
}


uint239_t operator%(const uint239_t &lhs, const uint239_t &rhs) {
    //остаток от деления lhs % rhs
    uint239_t num1 = GetNumWithoutShift(lhs);
    uint239_t num2 = GetNumWithoutShift(rhs);
    //a - (a / b) * b = a % b
    uint239_t div = num1 / num2; // (a / b)
    uint239_t mul = div * num2; // (a / b) * b
    return num1 - mul;
}


std::ostream &operator<<(std::ostream &stream, const uint239_t &value) {
    uint239_t num = GetNumWithoutShift(value);

    char str[75]; // 10 ^ 75 > 2 ^ 239
    uint16_t cur_index = 0;
    const uint239_t zero = FromInt(0, 0);
    uint239_t reminder;
    const uint239_t ten = FromInt(10, 0);
    while (num > zero) {
        reminder = num % ten;
        num = num / ten;
        str[cur_index++] = static_cast<char>(reminder.data[34] + '0');
    }
    char ans[cur_index + 1];
    ans[cur_index] = '\0';
    for (int index = cur_index - 1; index >= 0; index--) {
        ans[cur_index - index - 1] = str[index];
    }
    stream << ans;
    return stream;
}