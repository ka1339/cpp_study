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

    std::thread server([&]
                       {
                           try
                           {
                               networking::Socket connection =
                                   networking::accept_connection(listener);
                               const std::string message =
                                   networking::receive_until_closed(connection);
                               networking::send_all(connection, "Echo: " + message);
                           }
                           catch (...)
                           {
                               server_error = std::current_exception();
                           }
                       });

    std::exception_ptr client_error;
    std::string reply;
    try
    {
        networking::Socket client = networking::connect_loopback(port);
        networking::send_all(client, "Hello over TCP\n");
        networking::shutdown_send(client);
        reply = networking::receive_until_closed(client);
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

    std::cout << "TCP loopback reply: " << reply;
}
