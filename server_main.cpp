#include <boost/asio.hpp>
#include <iostream>
#include "NetworkServer.h"

int main() {
    try {
        boost::asio::io_context io;
        NetworkServer server(io, 3000);

        std::cout << "Server running on port 3000...\n";
        io.run();
    } catch (const std::exception& e) {
        std::cerr << "Server error: " << e.what() << "\n";
    }

    return 0;
}