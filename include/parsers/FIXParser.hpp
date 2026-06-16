#pragma once
#include "parsers/IParser.hpp"

class FIXParser : public IParser {
    public:
        Order parse(const std::string& message) override;
};