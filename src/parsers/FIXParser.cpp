#include "parsers/FIXParser.hpp"
#include "gateways/FIXDefinition.hpp"
#include <charconv>

std::optional<Order> FIXParser::parse(std::string_view rawData) {
    Order order;
    size_t pos = 0;

    // Complete basic message checks
    if (rawData.empty()) {
        return std::nullopt; // No data to parse
    }

    rawData.remove_prefix(rawData.find_first_not_of(" \t\n\r\f\v")); // Trim leading whitespace
    rawData.remove_suffix(rawData.find_last_not_of(" \t\n\r\f\v") + 1); // Trim trailing whitespace

    while (pos < rawData.size()) {
        // Process each chunk of data separated by the SOH delimiter
        std::string_view chunk = rawData.substr(pos);
        size_t nextPos = chunk.find_first_of('\x01'); // SOH delimiter in FIX messages
        if (nextPos == std::string_view::npos) {
            break; // No more chunks to process
        }
        chunk = chunk.substr(0, nextPos); // Get the current chunk
        pos += nextPos + 1; // Move to the next chunk

        size_t eqPos = chunk.find('='); // Find the position of the '=' character
        if (eqPos == std::string_view::npos) {
            return std::nullopt; // malformed field
        }

        std::string_view tagStr = chunk.substr(0, eqPos); // Extract the tag number
        std::string_view valStr = chunk.substr(eqPos + 1); // Extract the value

        int tag = 0;
        // Convert the tag string to an int to match your FIX::Tags definitions
        auto result = std::from_chars(tagStr.data(), tagStr.data() + tagStr.size(), tag);
        
        if (result.ec == std::errc()) {
            switch(tag) {
                case FIX::Tags::MsgType: {
                    if (valStr == "D") {
                        order.type = OrderType::LIMIT; // Assuming NewOrderSingle corresponds to a LIMIT order
                    } else {
                        return std::nullopt; // Unsupported message type
                    }
                    break;
                }

                case FIX::Tags::PRICE: {
                    // Use from_chars for the double/int value
                    int parsedPrice = 0;
                    auto res = std::from_chars(valStr.data(), valStr.data() + valStr.size(), parsedPrice);
                    
                    if (res.ec == std::errc()) {
                        order.price = parsedPrice;
                    }
                    break;
                }

                case FIX::Tags::QUANTITY: {
                    // Use from_chars for the quantity
                    int parsedQuantity = 0;
                    auto res = std::from_chars(valStr.data(), valStr.data() + valStr.size(), parsedQuantity);
                    
                    if (res.ec == std::errc()) {
                        order.quantity = parsedQuantity;
                    }
                    break;
                }
            }   
        }
    }

    if (order.type == OrderType::NONE || order.price <= 0 || order.quantity <= 0) {
        return std::nullopt; // The message was incomplete or invalid
    }

    return order;
}