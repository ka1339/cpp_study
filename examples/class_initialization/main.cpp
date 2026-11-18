#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

class ServerConfig
{
public:
    explicit ServerConfig(std::string host)
        : ServerConfig(std::move(host), 8080)
    {
    }

    ServerConfig(std::string host, int port)
        : host_(validate_host(std::move(host))),
          port_(validate_port(port)),
          endpoint_(host_ + ":" + std::to_string(port_))
    {
    }

    const std::string &endpoint() const
    {
        return endpoint_;
    }

    int timeout_seconds() const
    {
        return timeout_seconds_;
    }

private:
    static std::string validate_host(std::string host)
    {
        if (host.empty())
        {
            throw std::invalid_argument("host must not be empty");
        }
        return host;
    }

    static int validate_port(int port)
    {
        if (port < 1 || port > 65535)
        {
            throw std::invalid_argument("port must be between 1 and 65535");
        }
        return port;
    }

    std::string host_;
    int port_;
    std::string endpoint_;
    int timeout_seconds_{30};
};

class Account
{
public:
    Account(std::string id, int initial_balance_cents)
        : id_(validate_id(std::move(id))),
          balance_cents_(validate_balance(initial_balance_cents))
    {
    }

    const std::string &id() const
    {
        return id_;
    }

    int balance_cents() const
    {
        return balance_cents_;
    }

private:
    static std::string validate_id(std::string id)
    {
        if (id.empty())
        {
            throw std::invalid_argument("account id must not be empty");
        }
        return id;
    }

    static int validate_balance(int balance_cents)
    {
        if (balance_cents < 0)
        {
            throw std::invalid_argument("balance must not be negative");
        }
        return balance_cents;
    }

    const std::string id_;
    int balance_cents_;
};

class Job
{
public:
    virtual ~Job() = default;
    virtual void run() const = 0;
};

class EmailJob final : public Job
{
public:
    explicit EmailJob(std::string recipient)
        : recipient_(std::move(recipient))
    {
    }

    void run() const override
    {
        std::cout << "Sending email to " << recipient_ << '\n';
    }

private:
    std::string recipient_;
};

std::unique_ptr<Job> make_email_job(std::string recipient)
{
    return std::make_unique<EmailJob>(std::move(recipient));
}

int main()
{
    const ServerConfig config("api.example.com");
    std::cout << "Endpoint: " << config.endpoint()
              << ", timeout: " << config.timeout_seconds() << " seconds\n";

    const Account account("account-1001", 25000);
    std::cout << "Account " << account.id()
              << " balance: " << account.balance_cents() << " cents\n";

    const std::unique_ptr<Job> job = make_email_job("ops@example.com");
    job->run();
}
