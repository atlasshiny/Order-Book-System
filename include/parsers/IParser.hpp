#pragma once
#include <string>
#include <string_view>
#include <optional>
#include "orderbook/Order.hpp"

class IParser {
    public:
        virtual std::optional<Order> parse(std::string_view rawData) = 0;
        virtual ~IParser() = default;
};