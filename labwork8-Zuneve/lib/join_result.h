#pragma once

#include <optional>

template <typename Base, typename Joined>
struct JoinResult {
	Base base;
	std::optional<Joined> joined;

	JoinResult() = default;

	JoinResult(Base base, std::optional<Joined> joined) : base(std::move(base)),
		joined(std::move(joined)) {
	}

	bool operator==(const JoinResult<Base, Joined>& other) const {
		return base == other.base && joined == other.joined;
	}
};
