#pragma once

template <typename Key, typename Value>
struct KV {
	Key key;
	Value value;

    KV() = default;
    KV(const Key& k, const Value& v) : key(k), value(v) {}

    bool operator==(const KV<Key, Value>& other) const {
        return key == other.key && value == other.value;
    }
};
