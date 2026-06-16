#pragma once
#include "parsers/IParser.hpp"

class FIXParser : public IParser {
    public:
        std::optional<Order> parse(std::string_view rawData) override;
};