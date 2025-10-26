#pragma once

#include <memory>
#include <iterator>

template <typename Container>
class AsDataFlowAdapter {
public:
    using container_type = Container;
    using value_type = typename Container::value_type;
    using reference = typename Container::reference;
    using const_reference = typename Container::const_reference;
    using iterator = decltype(std::begin(std::declval<Container&>()));
    using const_iterator = decltype(std::begin(std::declval<const Container&>()));

    explicit AsDataFlowAdapter(Container& container) : container_(&container) {}

    iterator begin() {
        return std::begin(*container_);
    }

    iterator end() {
        return std::end(*container_);
    }

    const_iterator begin() const {
        return std::begin(*container_);
    }

    const_iterator end() const {
        return std::end(*container_);
    }

private:
    Container* container_;
};

template <typename Container>
inline auto AsDataFlow(Container& container) {
    return AsDataFlowAdapter<Container>(container);
}
