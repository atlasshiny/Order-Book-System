#include "orderbook/Order.hpp"
#include <optional>
#include <string_view>

class IGateway {
    public:
        virtual ~IGateway() = default;
        virtual size_t sendOrder(const Order& order, char* wireBuffer_, size_t bufferSize) = 0;
        virtual std::optional<Order> receiveOrder(std::string_view rawData) = 0;
        virtual void cancelOrder(int orderId) = 0;
        virtual void onMarketDataUpdate() = 0;
};