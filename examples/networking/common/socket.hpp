#pragma once

#include <cerrno>
#include <cstddef>
#include <stdexcept>
#include <string>
#include <string_view>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

namespace networking
{

#ifdef _WIN32
using SocketHandle = SOCKET;
inline constexpr SocketHandle invalid_socket = INVALID_SOCKET;
using SocketLength = int;
#else
using SocketHandle = int;
inline constexpr SocketHandle invalid_socket = -1;
using SocketLength = socklen_t;
#endif

inline void throw_socket_error(const char *operation)
{
#ifdef _WIN32
    const int error = WSAGetLastError();
#else
    const int error = errno;
#endif
    throw std::runtime_error(std::string(operation) + " failed with error " +
                             std::to_string(error));
}

class SocketRuntime
{
public:
    SocketRuntime()
    {
#ifdef _WIN32
        WSADATA data{};
        if (WSAStartup(MAKEWORD(2, 2), &data) != 0)
        {
            throw_socket_error("WSAStartup");
        }
#endif
    }

    ~SocketRuntime()
    {
#ifdef _WIN32
        WSACleanup();
#endif
    }

    SocketRuntime(const SocketRuntime &) = delete;
    SocketRuntime &operator=(const SocketRuntime &) = delete;
};

class Socket
{
public:
    explicit Socket(SocketHandle handle = invalid_socket)
        : handle_(handle)
    {
    }

    ~Socket()
    {
        close();
    }

    Socket(const Socket &) = delete;
    Socket &operator=(const Socket &) = delete;

    Socket(Socket &&other) noexcept
        : handle_(other.handle_)
    {
        other.handle_ = invalid_socket;
    }

    Socket &operator=(Socket &&other) noexcept
    {
        if (this != &other)
        {
            close();
            handle_ = other.handle_;
            other.handle_ = invalid_socket;
        }
        return *this;
    }

    SocketHandle get() const
    {
        return handle_;
    }

private:
    void close() noexcept
    {
        if (handle_ == invalid_socket)
        {
            return;
        }
#ifdef _WIN32
        closesocket(handle_);
#else
        ::close(handle_);
#endif
        handle_ = invalid_socket;
    }

    SocketHandle handle_;
};

inline Socket create_tcp_socket()
{
    const SocketHandle handle =
        ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (handle == invalid_socket)
    {
        throw_socket_error("socket");
    }
    return Socket(handle);
}

inline Socket make_loopback_listener(unsigned short &port)
{
    Socket listener = create_tcp_socket();
    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    address.sin_port = 0;

    if (::bind(listener.get(), reinterpret_cast<sockaddr *>(&address),
               static_cast<SocketLength>(sizeof(address))) != 0)
    {
        throw_socket_error("bind");
    }
    if (::listen(listener.get(), 1) != 0)
    {
        throw_socket_error("listen");
    }

    SocketLength address_length = static_cast<SocketLength>(sizeof(address));
    if (::getsockname(listener.get(), reinterpret_cast<sockaddr *>(&address),
                      &address_length) != 0)
    {
        throw_socket_error("getsockname");
    }
    port = ntohs(address.sin_port);
    return listener;
}

inline Socket connect_loopback(unsigned short port)
{
    Socket connection = create_tcp_socket();
    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    address.sin_port = htons(port);

    if (::connect(connection.get(), reinterpret_cast<sockaddr *>(&address),
                  static_cast<SocketLength>(sizeof(address))) != 0)
    {
        throw_socket_error("connect");
    }
    return connection;
}

inline Socket make_loopback_listener_on(unsigned short port)
{
    Socket listener = create_tcp_socket();
    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    address.sin_port = htons(port);

    if (::bind(listener.get(), reinterpret_cast<sockaddr *>(&address),
               static_cast<SocketLength>(sizeof(address))) != 0)
    {
        throw_socket_error("bind");
    }
    if (::listen(listener.get(), 16) != 0)
    {
        throw_socket_error("listen");
    }
    return listener;
}

inline bool receive_exact(
    const Socket &connection, char *buffer, std::size_t length, bool allow_eof = false)
{
    std::size_t received_total = 0;
    while (received_total < length)
    {
#ifdef _WIN32
        const int received = ::recv(
            connection.get(), buffer + received_total,
            static_cast<int>(length - received_total), 0);
#else
        const ssize_t received = ::recv(
            connection.get(), buffer + received_total, length - received_total, 0);
#endif
        if (received == 0)
        {
            if (allow_eof && received_total == 0)
            {
                return false;
            }
            throw std::runtime_error("connection closed before all bytes arrived");
        }
        if (received < 0)
        {
            throw_socket_error("recv");
        }
        received_total += static_cast<std::size_t>(received);
    }
    return true;
}

inline Socket accept_connection(const Socket &listener)
{
    const SocketHandle handle =
        ::accept(listener.get(), nullptr, nullptr);
    if (handle == invalid_socket)
    {
        throw_socket_error("accept");
    }
    return Socket(handle);
}

inline void send_all(const Socket &connection, std::string_view message)
{
    std::size_t sent = 0;
    while (sent < message.size())
    {
        const std::size_t remaining = message.size() - sent;
#ifdef _WIN32
        const int result = ::send(
            connection.get(), message.data() + sent,
            static_cast<int>(remaining > 0x7fffffff ? 0x7fffffff : remaining), 0);
#else
        int flags = 0;
#ifdef MSG_NOSIGNAL
        flags = MSG_NOSIGNAL;
#endif
        const ssize_t result =
            ::send(connection.get(), message.data() + sent, remaining, flags);
#endif
        if (result <= 0)
        {
            throw_socket_error("send");
        }
        sent += static_cast<std::size_t>(result);
    }
}

inline void shutdown_send(const Socket &connection)
{
#ifdef _WIN32
    const int result = ::shutdown(connection.get(), SD_SEND);
#else
    const int result = ::shutdown(connection.get(), SHUT_WR);
#endif
    if (result != 0)
    {
        throw_socket_error("shutdown");
    }
}

inline std::string receive_until_closed(const Socket &connection)
{
    std::string message;
    char buffer[1024];
    for (;;)
    {
#ifdef _WIN32
        const int received =
            ::recv(connection.get(), buffer, static_cast<int>(sizeof(buffer)), 0);
#else
        const ssize_t received =
            ::recv(connection.get(), buffer, sizeof(buffer), 0);
#endif
        if (received == 0)
        {
            return message;
        }
        if (received < 0)
        {
            throw_socket_error("recv");
        }
        message.append(buffer, static_cast<std::size_t>(received));
    }
}

inline std::string receive_until(
    const Socket &connection, std::string_view delimiter, std::size_t limit)
{
    std::string message;
    char buffer[1024];
    while (message.find(delimiter) == std::string::npos)
    {
#ifdef _WIN32
        const int received =
            ::recv(connection.get(), buffer, static_cast<int>(sizeof(buffer)), 0);
#else
        const ssize_t received =
            ::recv(connection.get(), buffer, sizeof(buffer), 0);
#endif
        if (received == 0)
        {
            throw std::runtime_error("connection closed before message delimiter");
        }
        if (received < 0)
        {
            throw_socket_error("recv");
        }
        message.append(buffer, static_cast<std::size_t>(received));
        if (message.size() > limit)
        {
            throw std::runtime_error("received message exceeds size limit");
        }
    }
    return message;
}

} // namespace networking
