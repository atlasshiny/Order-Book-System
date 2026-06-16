#pragma once
#include "parsers/IParser.hpp"

class JSONParser : public IParser {
    public:
        Order parse(const std::string& message) override;
};