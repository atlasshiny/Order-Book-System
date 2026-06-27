#include "orderbook/Order.hpp"

class IGateway {
    public:
        virtual ~IGateway() = default;
        virtual void sendOrder(const Order& order) = 0;
        virtual void receiveOrder(const char* rawData, size_t dataSize) = 0;
        virtual void cancelOrder(int orderId) = 0;
        virtual void onMarketDataUpdate() = 0;
};