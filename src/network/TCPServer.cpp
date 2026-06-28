#include "network/TCPServer.hpp"

TCPServer::TCPServer(unsigned short port, std::shared_ptr<IGateway> gateway)
    : io_context_(),
      acceptor_(io_context_, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)),
      gateway_(std::move(gateway)) {
    doAccept();
}

void TCPServer::run() {
    io_context_.run();
}

void TCPServer::doAccept() {
    acceptor_.async_accept(
        [this](std::error_code ec, asio::ip::tcp::socket socket) {
            if (!ec) {
                std::cout << "[Server] New client inbound connection from: " 
                          << socket.remote_endpoint() << std::endl;
                
                // Instantiate a standalone session container to manage this client's lifetime
                std::make_shared<TCPSession>(std::move(socket), gateway_)->start();
            }
            
            // Keep the loop primed for additional clients
            doAccept();
        });
}