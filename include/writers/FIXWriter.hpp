#pragma once
#include "IWriter.hpp"

class FIXWriter : public IWriter {
    public:
        size_t write(const Order& order, char* buffer, size_t bufferSize) override;

        size_t writeExecutionReport(const Order& order, std::string_view execType, char* buffer, size_t bufferSize) override;
};