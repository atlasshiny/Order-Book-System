#pragma once
#include "parsers/IParser.hpp"

class JSONParser : public IParser {
    public:
        std::optional<Order> parse(std::string_view rawData) override;
};