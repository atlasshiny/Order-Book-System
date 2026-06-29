#pragma once
#include "orderbook/Order.hpp"
#include <iostream>
#include <cstddef>

class IWriter {
    public:
        virtual size_t write(const Order& order, char* buffer, size_t bufferSize) = 0;

        virtual size_t writeExecutionReport(const Order& order, std::string_view execType, char* buffer, size_t bufferSize) = 0;

        virtual ~IWriter() = default;
};