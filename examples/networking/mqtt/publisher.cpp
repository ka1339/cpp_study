#include "../common/mqtt_protocol.hpp"

#include <iostream>
#include <stdexcept>

namespace
{

} // namespace

int main(int argc, char **argv)
{
    networking::SocketRuntime runtime;
    const auto port = networking::mqtt::port_from_args(argc, argv, 1883);
    networking::Socket connection = networking::connect_loopback(port);
    networking::mqtt::send_packet(
        connection, networking::mqtt::make_connect("cpp-study-publisher"));

    networking::mqtt::Packet connack{};
    if (!networking::mqtt::receive_packet(connection, connack) ||
        connack.header != 0x20 || connack.body.size() != 2 ||
        connack.body[1] != 0)
    {
        throw std::runtime_error("broker rejected the MQTT connection");
    }

    const std::string topic = "cpp-study/demo";
    const std::string payload = "Hello from the MQTT publisher";
    networking::mqtt::send_packet(
        connection, networking::mqtt::make_publish(topic, payload));
    networking::mqtt::send_packet(connection, {0xe0, 0x00});
    std::cout << "Published to " << topic << ": " << payload << '\n';
}
