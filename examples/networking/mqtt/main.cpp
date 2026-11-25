#include <cstdint>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

namespace
{

void append_mqtt_string(std::vector<std::uint8_t> &packet, const std::string &value)
{
    const auto length = static_cast<std::uint16_t>(value.size());
    packet.push_back(static_cast<std::uint8_t>(length >> 8));
    packet.push_back(static_cast<std::uint8_t>(length & 0xff));
    packet.insert(packet.end(), value.begin(), value.end());
}

void append_remaining_length(std::vector<std::uint8_t> &packet, std::size_t length)
{
    do
    {
        std::uint8_t encoded = static_cast<std::uint8_t>(length % 128);
        length /= 128;
        if (length > 0)
        {
            encoded |= 0x80;
        }
        packet.push_back(encoded);
    } while (length > 0);
}

std::vector<std::uint8_t> make_connect_packet(const std::string &client_id)
{
    std::vector<std::uint8_t> body;
    append_mqtt_string(body, "MQTT");
    body.push_back(4);    // MQTT 3.1.1
    body.push_back(2);    // Clean session
    body.push_back(0);    // Keep-alive: 60 seconds
    body.push_back(60);
    append_mqtt_string(body, client_id);

    std::vector<std::uint8_t> packet{0x10};
    append_remaining_length(packet, body.size());
    packet.insert(packet.end(), body.begin(), body.end());
    return packet;
}

std::vector<std::uint8_t> make_publish_packet(
    const std::string &topic, const std::string &payload)
{
    std::vector<std::uint8_t> body;
    append_mqtt_string(body, topic);
    body.insert(body.end(), payload.begin(), payload.end());

    std::vector<std::uint8_t> packet{0x30}; // PUBLISH, QoS 0
    append_remaining_length(packet, body.size());
    packet.insert(packet.end(), body.begin(), body.end());
    return packet;
}

void print_packet(const char *label, const std::vector<std::uint8_t> &packet)
{
    std::cout << label << " (" << packet.size() << " bytes): ";
    for (const std::uint8_t byte : packet)
    {
        std::cout << std::hex << std::setw(2) << std::setfill('0')
                  << static_cast<unsigned int>(byte) << ' ';
    }
    std::cout << std::dec << std::setfill(' ') << '\n';
}

} // namespace

int main()
{
    const auto connect = make_connect_packet("cpp-study-demo");
    const auto publish = make_publish_packet("cpp-study/demo", "hello from C++");

    std::cout << "MQTT 3.1.1 packet layout (hexadecimal):\n";
    print_packet("CONNECT", connect);
    print_packet("PUBLISH", publish);
    std::cout << "The CONNECT packet starts with 0x10; the QoS 0 PUBLISH packet "
                 "starts with 0x30.\n";
}
