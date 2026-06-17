#pragma once
#include <string_view>

namespace FIX {
    // Standard FIX tags for order fields
    namespace Tags {
        constexpr int MsgType = 35;
        constexpr int SIDE = 54;
        constexpr int PRICE = 44;
        constexpr int QUANTITY = 38;
        constexpr int TIMESTAMP = 60;
        // Add other message types as needed
    }

    namespace MsgTypes {
        constexpr char NewOrderSingle = 'D';
        constexpr std::string_view ExecutionReport = "8";
        // Add other message types as needed
    }
}