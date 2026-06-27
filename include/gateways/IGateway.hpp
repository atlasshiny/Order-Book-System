#include "orderbook/Order.hpp"

class IGateway {
    public:
        virtual ~IGateway() = default;
        virtual size_t sendOrder(const Order& order) = 0;
        virtual std::optional<Order> receiveOrder(std::string_view rawData) = 0;
        virtual void cancelOrder(int orderId) = 0;
        virtual void onMarketDataUpdate() = 0;
};