#include "network/TCPServer.hpp"

TCPServer::TCPServer(unsigned short port, std::shared_ptr<ExchangeOrchestrator> orchestrator)
    : io_context_(),
      acceptor_(io_context_, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)),
      orchestrator_(std::move(orchestrator)) {
    doAccept();
}

void TCPServer::run() {
    std::cout << "[Server] : Server running on port " << acceptor_.local_endpoint().port() << std::endl;

    io_context_.run();
}

void TCPServer::doAccept() {
    acceptor_.async_accept(
        [this](std::error_code ec, asio::ip::tcp::socket socket) {
            if (!ec) {
                std::cout << "[Server] New client inbound connection from: " 
                          << socket.remote_endpoint() << std::endl;
                
                // Instantiate a standalone session container to manage this client's lifetime
                std::make_shared<TCPSession>(std::move(socket), orchestrator_)->start();
            } else {
                std::cerr << "[Server] Error accepting connection: " << ec.message() << std::endl;
            }
            
            // Keep the loop primed for additional clients
            doAccept();
        });
}

int TCPServer::getPort() const {
    return acceptor_.local_endpoint().port();
}

std::shared_ptr<ExchangeOrchestrator> TCPServer::getOrchestrator() const {
    return orchestrator_;
}