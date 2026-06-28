#include "network/TCPServer.hpp"
#include "gateways/FIXGateway.hpp"
#include <memory>
#include <iostream>

int main() {
    try {
        unsigned short port = 8080;
        std::cout << "[Main] Initializing Order Book Simulator on port " << port << "...\n";

        // Create the business logic component
        auto gateway = std::make_shared<FIXGateway>();

        // Start the server (this hooks up the internal io_context acceptor loop)
        TCPServer server(port, gateway);

        // This blocks and keeps the engine alive running your async events!
        server.run();

    } catch (const std::exception& e) {
        std::cerr << "[Main] Runtime exception: " << e.what() << "\n";
    }
    return 0;
}