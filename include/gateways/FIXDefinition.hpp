#pragma once
#include <string_view>

namespace FIX {
    // Standard FIX tags for order fields
    namespace Tags {
    // header tags for the messages
        constexpr int BeginString = 8;
        constexpr int BodyLength = 9;
        constexpr int MsgType = 35;
        constexpr int Checksum = 10;

        // common order fields
        constexpr int ClOrdID = 11;
        constexpr int OrderID = 37;
        constexpr int OrdType = 40;
        constexpr int Symbol = 55;
        constexpr int SIDE = 54;
        constexpr int PRICE = 44;
        constexpr int QUANTITY = 38;
        constexpr int TIMESTAMP = 60;
        // Add other message types as needed
    }

    namespace MsgTypes {
        constexpr char NewOrderSingle = 'D';
        // Add other message types as needed
    }
}