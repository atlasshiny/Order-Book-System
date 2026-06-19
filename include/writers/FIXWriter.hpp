#pragma once
#include "IWriter.hpp"

class FIXWriter : public IWriter {
    public:
        size_t write(const Order& order, char* buffer, size_t bufferSize) override;
};