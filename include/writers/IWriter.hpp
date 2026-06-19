#pragma once
#include "orderbook/Order.hpp"
#include <cstddef>

class IWriter {
    public:
        virtual size_t write(const Order& order, char* buffer, size_t bufferSize) = 0;
        virtual ~IWriter() = default;
};