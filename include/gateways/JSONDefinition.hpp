#pragma once
#include <string_view>

namespace JSON {
    namespace Tags {
        constexpr std::string_view OrderType = "order_type"; 
        constexpr std::string_view Symbol = "symbol";
        constexpr std::string_view Side = "side";
        constexpr std::string_view Price = "price";
        constexpr std::string_view Quantity = "quantity";
        constexpr std::string_view Timestamp = "timestamp";
        constexpr std::string_view ExecType = "exec_type"; // Might not be implemented yet
        constexpr std::string_view OrdStatus = "ord_status";
        constexpr std::string_view OrderID = "order_id";
        constexpr std::string_view ClientID = "client_id"; 
        constexpr std::string_view LastShares = "last_shares";
    }
}
