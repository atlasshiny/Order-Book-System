#pragma once
#include "writers/IWriter.hpp"

class JSONWriter : public IWriter {
public:
    virtual size_t write(const Order& order, char* buffer, size_t bufferSize) override;

    virtual size_t writeExecutionReport(const Order& order, int lastShares, std::string_view execType, std::string_view ordStatus, char* buffer, size_t bufferSize) override;
};