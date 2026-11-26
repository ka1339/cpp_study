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
    networking::Socket listener = networking::make_loopback_listener_on(port);
    std::cout << "HTTP server listening on http://127.0.0.1:" << port << '\n';

    networking::Socket connection = networking::accept_connection(listener);
    const std::string request =
        networking::receive_until(connection, "\r\n\r\n", 8192);
    std::cout << "Received request:\n" << request;

    const std::string body = "Hello from a local HTTP server\n";
    const std::string response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain; charset=utf-8\r\n"
        "Content-Length: " +
        std::to_string(body.size()) +
        "\r\nConnection: close\r\n\r\n" + body;
    networking::send_all(connection, response);
}
