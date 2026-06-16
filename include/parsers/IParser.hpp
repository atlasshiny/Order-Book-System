#pragma once
#include <string>
#include <orderbook/Order.hpp>

class IParser {
    public:
        virtual Order parse(const std::string& message) = 0;
        virtual ~IParser() = default;
};