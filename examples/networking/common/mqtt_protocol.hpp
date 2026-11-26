#pragma once

#include "socket.hpp"

#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

namespace networking
{
namespace mqtt
{

inline unsigned short port_from_args(
    int argc, char **argv, unsigned short default_port)
{
    if (argc == 1)
    {
        return default_port;
    }
    if (argc != 2)
    {
        throw std::invalid_argument("provide at most one port argument");
    }

    std::size_t parsed = 0;
    const unsigned long port = std::stoul(argv[1], &parsed);
    if (parsed != std::string(argv[1]).size() || port == 0 || port > 65535)
    {
        throw std::invalid_argument("port must be between 1 and 65535");
    }
    return static_cast<unsigned short>(port);
}

struct Packet
{
    std::uint8_t header;
    std::vector<std::uint8_t> body;
};

inline void append_remaining_length(
    std::vector<std::uint8_t> &bytes, std::size_t length)
{
    if (length > 268435455)
    {
        throw std::runtime_error("MQTT packet is too large");
    }

    do
    {
        std::uint8_t encoded = static_cast<std::uint8_t>(length % 128);
        length /= 128;
        if (length != 0)
        {
            encoded |= 0x80;
        }
        bytes.push_back(encoded);
    } while (length != 0);
}

inline void append_string(std::vector<std::uint8_t> &bytes, const std::string &value)
{
    if (value.size() > 65535)
    {
        throw std::runtime_error("MQTT string is too long");
    }

    const auto length = static_cast<std::uint16_t>(value.size());
    bytes.push_back(static_cast<std::uint8_t>(length >> 8));
    bytes.push_back(static_cast<std::uint8_t>(length & 0xff));
    bytes.insert(bytes.end(), value.begin(), value.end());
}

inline std::vector<std::uint8_t> make_packet(
    std::uint8_t header, const std::vector<std::uint8_t> &body)
{
    std::vector<std::uint8_t> packet{header};
    append_remaining_length(packet, body.size());
    packet.insert(packet.end(), body.begin(), body.end());
    return packet;
}

inline std::vector<std::uint8_t> make_connect(const std::string &client_id)
{
    std::vector<std::uint8_t> body;
    append_string(body, "MQTT");
    body.insert(body.end(), {4, 2, 0, 60});
    append_string(body, client_id);
    return make_packet(0x10, body);
}

inline std::vector<std::uint8_t> make_publish(
    const std::string &topic, const std::string &payload)
{
    std::vector<std::uint8_t> body;
    append_string(body, topic);
    body.insert(body.end(), payload.begin(), payload.end());
    return make_packet(0x30, body);
}

inline std::vector<std::uint8_t> make_subscribe(
    std::uint16_t packet_id, const std::string &topic_filter)
{
    std::vector<std::uint8_t> body{
        static_cast<std::uint8_t>(packet_id >> 8),
        static_cast<std::uint8_t>(packet_id & 0xff)};
    append_string(body, topic_filter);
    body.push_back(0);
    return make_packet(0x82, body);
}

inline void send_packet(const Socket &connection,
                        const std::vector<std::uint8_t> &packet)
{
    send_all(connection, std::string_view(
                             reinterpret_cast<const char *>(packet.data()),
                             packet.size()));
}

inline bool receive_packet(const Socket &connection, Packet &packet)
{
    char header = 0;
    if (!receive_exact(connection, &header, 1, true))
    {
        return false;
    }

    std::size_t remaining_length = 0;
    std::size_t multiplier = 1;
    for (int count = 0; count < 4; ++count)
    {
        char encoded_char = 0;
        receive_exact(connection, &encoded_char, 1);
        const auto encoded = static_cast<std::uint8_t>(encoded_char);
        remaining_length += (encoded & 0x7f) * multiplier;
        if ((encoded & 0x80) == 0)
        {
            if (remaining_length > 1024 * 1024)
            {
                throw std::runtime_error("MQTT packet exceeds 1 MiB demo limit");
            }
            packet.header = static_cast<std::uint8_t>(header);
            packet.body.resize(remaining_length);
            if (!packet.body.empty())
            {
                receive_exact(connection,
                              reinterpret_cast<char *>(packet.body.data()),
                              packet.body.size());
            }
            return true;
        }
        multiplier *= 128;
    }
    throw std::runtime_error("invalid MQTT remaining length");
}

class Reader
{
public:
    explicit Reader(const std::vector<std::uint8_t> &bytes)
        : bytes_(bytes)
    {
    }

    std::uint8_t read_byte()
    {
        require(1);
        return bytes_[position_++];
    }

    std::uint16_t read_u16()
    {
        const std::uint16_t high = read_byte();
        return static_cast<std::uint16_t>((high << 8) | read_byte());
    }

    std::string read_string()
    {
        const std::uint16_t length = read_u16();
        require(length);
        if (length == 0)
        {
            return {};
        }
        const auto *start = bytes_.data() + position_;
        position_ += length;
        return std::string(reinterpret_cast<const char *>(start), length);
    }

    std::string read_rest()
    {
        const std::size_t length = bytes_.size() - position_;
        if (length == 0)
        {
            return {};
        }
        const auto *start = bytes_.data() + position_;
        position_ = bytes_.size();
        return std::string(reinterpret_cast<const char *>(start), length);
    }

    bool empty() const
    {
        return position_ == bytes_.size();
    }

private:
    void require(std::size_t length) const
    {
        if (length > bytes_.size() - position_)
        {
            throw std::runtime_error("malformed MQTT packet body");
        }
    }

    const std::vector<std::uint8_t> &bytes_;
    std::size_t position_{0};
};

} // namespace mqtt
} // namespace networking
