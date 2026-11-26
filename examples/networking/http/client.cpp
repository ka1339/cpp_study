#include "../common/socket.hpp"

#include <iostream>
#include <string>

namespace
{

constexpr unsigned short port = 8080;

} // namespace

int main()
{
    networking::SocketRuntime runtime;
    networking::Socket connection = networking::connect_loopback(port);
    networking::send_all(
        connection,
        "GET / HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "Connection: close\r\n\r\n");

    const std::string response = networking::receive_until_closed(connection);
    std::cout << "Received response:\n" << response;
}
