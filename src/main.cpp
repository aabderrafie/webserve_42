
#include <iostream>
#include "server/Server.hpp"
#include "client/Client.hpp"

int main() {
    try {
        boost::asio::io_context io_context;

        Server server(io_context, 12345);

        std::thread server_thread([&io_context]() { io_context.run(); });

        std::this_thread::sleep_for(std::chrono::seconds(1)); // Give the server time to start

        Client client(io_context, "127.0.0.1", "12345");

        io_context.run();
        server_thread.join();
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}