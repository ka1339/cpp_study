#include "../common/socket.hpp"

#include <iostream>
#include <string>

namespace
{

constexpr unsigned short port = 39000;

} // namespace

int main()
{
    networking::SocketRuntime runtime;
    networking::Socket connection = networking::connect_loopback(port);
    const std::string message = "Hello over TCP\n";

    networking::send_all(connection, message);
    networking::shutdown_send(connection);
    std::cout << "Sent: " << message
              << "Received: " << networking::receive_until_closed(connection);
}
