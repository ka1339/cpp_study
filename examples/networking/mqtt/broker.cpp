#include "../common/mqtt_protocol.hpp"

#include <algorithm>
#include <exception>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

namespace
{

constexpr unsigned short default_port = 1883;
constexpr std::uint8_t connack_packet[] = {0x20, 0x02, 0x00, 0x00};

struct ClientSession
{
    explicit ClientSession(networking::Socket accepted_socket)
        : socket(std::move(accepted_socket))
    {
    }

    networking::Socket socket;
    std::mutex send_mutex;
    std::vector<std::string> subscriptions;
};

class Broker
{
public:
    void run(unsigned short port)
    {
        networking::Socket listener = networking::make_loopback_listener_on(port);
        std::cout << "MQTT 3.1.1 demo broker listening on 127.0.0.1:"
                  << port << '\n';

        for (;;)
        {
            auto client = std::make_shared<ClientSession>(
                networking::accept_connection(listener));
            {
                const std::lock_guard<std::mutex> lock(clients_mutex_);
                clients_.push_back(client);
            }
            std::thread([this, client]
                        { handle_client(client); })
                .detach();
        }
    }

private:
    void send(const std::shared_ptr<ClientSession> &client,
              const std::vector<std::uint8_t> &packet)
    {
        const std::lock_guard<std::mutex> lock(client->send_mutex);
        networking::mqtt::send_packet(client->socket, packet);
    }

    void handle_client(const std::shared_ptr<ClientSession> &client)
    {
        bool connected = false;
        try
        {
            networking::mqtt::Packet packet{};
            while (networking::mqtt::receive_packet(client->socket, packet))
            {
                const std::uint8_t type = packet.header >> 4;
                if (!connected && type != 1)
                {
                    throw std::runtime_error("first MQTT packet must be CONNECT");
                }

                if (type == 1)
                {
                    handle_connect(client, packet);
                    connected = true;
                }
                else if (type == 8)
                {
                    handle_subscribe(client, packet);
                }
                else if (type == 3)
                {
                    handle_publish(client, packet);
                }
                else if (type == 12 && packet.body.empty())
                {
                    send(client, {0xd0, 0x00});
                }
                else if (type == 14 && packet.body.empty())
                {
                    break;
                }
                else
                {
                    throw std::runtime_error("unsupported MQTT packet type");
                }
            }
        }
        catch (const std::exception &error)
        {
            std::cerr << "MQTT client disconnected: " << error.what() << '\n';
        }

        const std::lock_guard<std::mutex> lock(clients_mutex_);
        clients_.erase(std::remove(clients_.begin(), clients_.end(), client),
                       clients_.end());
    }

    void handle_connect(const std::shared_ptr<ClientSession> &client,
                        const networking::mqtt::Packet &packet)
    {
        if (packet.header != 0x10)
        {
            throw std::runtime_error("invalid CONNECT flags");
        }
        networking::mqtt::Reader reader(packet.body);
        const std::string protocol_name = reader.read_string();
        const std::uint8_t protocol_level = reader.read_byte();
        const std::uint8_t flags = reader.read_byte();
        reader.read_u16();
        const std::string client_id = reader.read_string();
        if (protocol_name != "MQTT" || protocol_level != 4 ||
            flags != 0x02 || client_id.empty() || !reader.empty())
        {
            throw std::runtime_error("only MQTT 3.1.1 clean-session CONNECT is supported");
        }

        std::vector<std::uint8_t> response(
            std::begin(connack_packet), std::end(connack_packet));
        send(client, response);
        std::cout << "Client connected to broker: " << client_id << '\n';
    }

    void handle_subscribe(const std::shared_ptr<ClientSession> &client,
                          const networking::mqtt::Packet &packet)
    {
        if ((packet.header & 0x0f) != 0x02)
        {
            throw std::runtime_error("invalid SUBSCRIBE flags");
        }
        networking::mqtt::Reader reader(packet.body);
        const std::uint16_t packet_id = reader.read_u16();
        const std::string filter = reader.read_string();
        const std::uint8_t requested_qos = reader.read_byte();
        if (filter.empty() || requested_qos != 0 || !reader.empty())
        {
            throw std::runtime_error("demo broker supports one QoS 0 filter per SUBSCRIBE");
        }

        {
            const std::lock_guard<std::mutex> lock(clients_mutex_);
            client->subscriptions.push_back(filter);
        }

        std::vector<std::uint8_t> body{
            static_cast<std::uint8_t>(packet_id >> 8),
            static_cast<std::uint8_t>(packet_id & 0xff),
            0};
        send(client, networking::mqtt::make_packet(0x90, body));
        std::cout << "Client subscribed to " << filter << '\n';
    }

    void handle_publish(const std::shared_ptr<ClientSession> &publisher,
                        const networking::mqtt::Packet &packet)
    {
        const std::uint8_t qos = (packet.header >> 1) & 0x03;
        if (qos != 0 || (packet.header & 0x01) != 0)
        {
            throw std::runtime_error(
                "demo broker supports non-retained QoS 0 PUBLISH only");
        }

        networking::mqtt::Reader reader(packet.body);
        const std::string topic = reader.read_string();
        const std::string payload = reader.read_rest();
        if (topic.empty() || topic.find_first_of("+#") != std::string::npos)
        {
            throw std::runtime_error("PUBLISH topic must not contain wildcards");
        }

        std::vector<std::shared_ptr<ClientSession>> recipients;
        {
            const std::lock_guard<std::mutex> lock(clients_mutex_);
            for (const auto &client : clients_)
            {
                if (client == publisher)
                {
                    continue;
                }
                const bool subscribed = std::any_of(
                    client->subscriptions.begin(), client->subscriptions.end(),
                    [&topic](const std::string &filter)
                    { return topic_matches(filter, topic); });
                if (subscribed)
                {
                    recipients.push_back(client);
                }
            }
        }

        const auto forwarded = networking::mqtt::make_publish(topic, payload);
        for (const auto &recipient : recipients)
        {
            send(recipient, forwarded);
        }
        std::cout << "Published on " << topic << " to "
                  << recipients.size() << " subscriber(s)\n";
    }

    static std::vector<std::string> split_levels(const std::string &value)
    {
        std::vector<std::string> levels;
        std::size_t start = 0;
        for (;;)
        {
            const std::size_t separator = value.find('/', start);
            levels.push_back(value.substr(
                start, separator == std::string::npos ? separator : separator - start));
            if (separator == std::string::npos)
            {
                return levels;
            }
            start = separator + 1;
        }
    }

    static bool topic_matches(const std::string &filter, const std::string &topic)
    {
        if (!topic.empty() && topic[0] == '$' &&
            (filter.empty() || filter[0] == '+' || filter[0] == '#'))
        {
            return false;
        }

        const auto filter_levels = split_levels(filter);
        const auto topic_levels = split_levels(topic);
        std::size_t index = 0;
        for (; index < filter_levels.size(); ++index)
        {
            if (filter_levels[index] == "#")
            {
                return index + 1 == filter_levels.size();
            }
            if (index >= topic_levels.size())
            {
                return false;
            }
            if (filter_levels[index] != "+" &&
                filter_levels[index] != topic_levels[index])
            {
                return false;
            }
        }
        return index == topic_levels.size();
    }

    std::mutex clients_mutex_;
    std::vector<std::shared_ptr<ClientSession>> clients_;
};

} // namespace

int main(int argc, char **argv)
{
    networking::SocketRuntime runtime;
    Broker broker;
    broker.run(networking::mqtt::port_from_args(argc, argv, default_port));
}
