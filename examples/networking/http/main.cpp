#include "../common/socket.hpp"

#include <exception>
#include <iostream>
#include <string>
#include <thread>

int main()
{
    networking::SocketRuntime runtime;
    unsigned short port = 0;
    networking::Socket listener = networking::make_loopback_listener(port);
    std::exception_ptr server_error;
    std::string received_request;

    std::thread server([&]
                       {
                           try
                           {
                               networking::Socket connection =
                                   networking::accept_connection(listener);
                               received_request = networking::receive_until(
                                   connection, "\r\n\r\n", 8192);
                               const std::string body = "Hello from a local HTTP server\n";
                               const std::string response =
                                   "HTTP/1.1 200 OK\r\n"
                                   "Content-Type: text/plain; charset=utf-8\r\n"
                                   "Content-Length: " +
                                   std::to_string(body.size()) +
                                   "\r\nConnection: close\r\n\r\n" + body;
                               networking::send_all(connection, response);
                           }
                           catch (...)
                           {
                               server_error = std::current_exception();
                           }
                       });

    std::exception_ptr client_error;
    std::string response;
    try
    {
        networking::Socket client = networking::connect_loopback(port);
        networking::send_all(
            client,
            "GET / HTTP/1.1\r\n"
            "Host: localhost\r\n"
            "Connection: close\r\n\r\n");
        response = networking::receive_until_closed(client);
    }
    catch (...)
    {
        client_error = std::current_exception();
    }

    server.join();
    if (client_error)
    {
        std::rethrow_exception(client_error);
    }
    if (server_error)
    {
        std::rethrow_exception(server_error);
    }

    std::cout << "Request received by server:\n"
              << received_request
              << "Response received by client:\n"
              << response;
}
