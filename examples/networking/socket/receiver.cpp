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
    networking::Socket listener = networking::make_loopback_listener_on(port);
    std::cout << "TCP receiver listening on 127.0.0.1:" << port << '\n';

    networking::Socket connection = networking::accept_connection(listener);
    const std::string message = networking::receive_until_closed(connection);
    std::cout << "Received: " << message;
    networking::send_all(connection, "Echo: " + message);
}
