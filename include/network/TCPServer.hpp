#pragma once
#include <asio.hpp>
#include <iostream>
#include "gateways/IGateway.hpp"
#include "network/TCPSession.hpp"

// TCPServer class that listens for incoming connections and manages sessions
class TCPServer {
    public:
        TCPServer(unsigned short port, std::shared_ptr<IGateway> gateway);

        void run();

    private:
        void doAccept();

        asio::io_context io_context_;
        asio::ip::tcp::acceptor acceptor_;
        std::shared_ptr<IGateway> gateway_;
};