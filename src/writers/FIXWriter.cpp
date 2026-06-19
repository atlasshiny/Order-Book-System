#include "writers/FIXWriter.hpp"
#include "gateways/FIXDefinition.hpp"
#include <cstdio>
#include <cstring>
#include <charconv>

size_t FIXWriter::write(const Order& order, char* buffer, size_t bufferSize) {
    char* ptr = buffer;
    char* end = buffer + bufferSize;
    char* bodyStart = nullptr;

    // Helper for NUMERIC types (tags, prices, quantities, sequence numbers)
    auto appendNumericField = [&](int tag, auto value) {
        auto [p1, ec1] = std::to_chars(ptr, end, tag);
        *p1++ = '=';
        auto [p2, ec2] = std::to_chars(p1, end, value);
        *p2++ = '\x01';
        ptr = p2;
    };

    // Helper for STRING types (BeginString, MsgType)
    auto appendStringField = [&](int tag, std::string_view value) {
        auto [p1, ec1] = std::to_chars(ptr, end, tag);
        *p1++ = '=';
        std::memcpy(ptr, value.data(), value.size());
        ptr += value.size();
        *ptr++ = '\x01';
    };

    // 1. Write BeginString (Tag 8)
    appendStringField(FIX::Tags::BeginString, "FIX.4.2");

    // 2. Placeholder for BodyLength (Tag 9)
    char* bodyLenPos = ptr;
    const char* placeholder = "9=000\x01";
    std::memcpy(ptr, placeholder, 6);
    ptr += 6;
    bodyStart = ptr; 

    // 3. MsgType (Tag 35)
    // Using a string helper since MsgType is a character
    appendStringField(FIX::Tags::MsgType, "D"); 

    // 4. Order fields
    appendNumericField(FIX::Tags::PRICE, order.price);
    appendNumericField(FIX::Tags::QUANTITY, order.quantity);

    // 5. Update BodyLength (Tag 9)
    size_t bodyLen = ptr - bodyStart;
    char temp[4];
    std::snprintf(temp, 4, "%03zu", bodyLen);
    std::memcpy(bodyLenPos + 2, temp, 3);

    // 6. Calculate Checksum (Tag 10)
    uint8_t checksum = 0;
    for (char* p = buffer; p < ptr; ++p) {
        checksum += static_cast<uint8_t>(*p);
    }

    // 7. Append Checksum (Tag 10)
    appendNumericField(FIX::Tags::Checksum, checksum % 256);

    return (ptr - buffer); // Total bytes written to buffer
}