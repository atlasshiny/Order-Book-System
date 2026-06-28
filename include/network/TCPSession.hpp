#pragma once
#include <asio.hpp>
#include <memory>
#include "gateways/IGateway.hpp"

// TCPSession class that manages a single TCP connection
class TCPSession : public std::enable_shared_from_this<TCPSession> {
public:
    TCPSession(asio::ip::tcp::socket socket, std::shared_ptr<IGateway> gateway);
    void start();
    void write(const std::string& data);
    void close();

private:
    void do_read();
    void do_write();

    asio::ip::tcp::socket socket_;
    std::array<char, 1024> buffer_;
    std::vector<std::string> pending_writes_;
    std::shared_ptr<IGateway> gateway_;
};