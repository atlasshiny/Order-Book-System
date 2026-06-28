#pragma once
#include "gateways/IGateway.hpp"
#include "orderbook/Order.hpp"
#include "parsers/FIXParser.hpp"
#include "writers/FIXWriter.hpp"

class FIXGateway : public IGateway {
public:
    std::optional<Order> on_data_received(std::shared_ptr<TCPSession> session, std::string_view raw_data) override;

    void on_client_connect(std::shared_ptr<TCPSession> session) override;
    void on_client_disconnect(std::shared_ptr<TCPSession> session) override;

private:
    FIXWriter fixWriter_;
    FIXParser fixParser_;

    size_t sendOrder(const Order& order, char* wireBuffer_, size_t bufferSize) override;

    std::optional<Order> receiveOrder(std::string_view rawData) override;

    void cancelOrder(int orderId) override;
};