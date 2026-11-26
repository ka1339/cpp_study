#include "../common/mqtt_protocol.hpp"

#include <iostream>
#include <stdexcept>
#include <string>

namespace
{

constexpr char topic[] = "cpp-study/demo";

networking::mqtt::Packet receive_required_packet(
    const networking::Socket &connection)
{
    networking::mqtt::Packet packet{};
    if (!networking::mqtt::receive_packet(connection, packet))
    {
        throw std::runtime_error("broker closed the connection");
    }
    return packet;
}

} // namespace

int main(int argc, char **argv)
{
    networking::SocketRuntime runtime;
    const auto port = networking::mqtt::port_from_args(argc, argv, 1883);
    networking::Socket connection = networking::connect_loopback(port);
    networking::mqtt::send_packet(
        connection, networking::mqtt::make_connect("cpp-study-subscriber"));

    const auto connack = receive_required_packet(connection);
    if (connack.header != 0x20 || connack.body.size() != 2 ||
        connack.body[1] != 0)
    {
        throw std::runtime_error("broker rejected the MQTT connection");
    }

    networking::mqtt::send_packet(
        connection, networking::mqtt::make_subscribe(1, topic));
    const auto suback = receive_required_packet(connection);
    if (suback.header != 0x90 || suback.body.size() != 3 ||
        suback.body[2] == 0x80)
    {
        throw std::runtime_error("broker rejected the subscription");
    }

    std::cout << "Subscribed to " << topic << "; waiting for one message...\n";
    for (;;)
    {
        const auto message = receive_required_packet(connection);
        if ((message.header >> 4) != 3)
        {
            continue;
        }

        networking::mqtt::Reader reader(message.body);
        const std::string received_topic = reader.read_string();
        std::cout << "Received on " << received_topic << ": "
                  << reader.read_rest() << '\n';
        networking::mqtt::send_packet(connection, {0xe0, 0x00});
        return 0;
    }
}
