#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

// 생성자 위임, 멤버 초기화 순서, 불변 조건 검증을 한 예제에서 보여준다.
class ServerConfig
{
public:
    explicit ServerConfig(std::string host)
        // 포트 기본값을 사용하는 생성 로직을 아래 생성자에 맡긴다.
        : ServerConfig(std::move(host), 8080)
    {
    }

    ServerConfig(std::string host, int port)
        // 멤버는 선언된 순서대로 초기화되므로 endpoint_는 검증된 host_와 port_를 사용한다.
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
    // 잘못된 입력은 객체 생성 시점에 예외로 거부해 유효한 상태만 유지한다.
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
    // 기본 멤버 초기화자는 생성자에서 따로 지정하지 않은 경우에 적용된다.
    int timeout_seconds_{30};
};

// const 멤버를 포함한 객체도 초기화 목록에서 값을 정해 생성할 수 있다.
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

// 파생 객체의 가상 함수를 객체 생성이 끝난 뒤 호출한다.
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
    // 파생 객체를 기반 클래스 포인터로 안전하게 반환한다.
    return std::make_unique<EmailJob>(std::move(recipient));
}

int main()
{
    // 인자를 생략하면 위임 생성자가 지정한 기본 포트와 기본 타임아웃이 사용된다.
    const ServerConfig config("api.example.com");
    std::cout << "Endpoint: " << config.endpoint()
              << ", timeout: " << config.timeout_seconds() << " seconds\n";

    // 생성자 검증을 통과한 계정 정보만 출력된다.
    const Account account("account-1001", 25000);
    std::cout << "Account " << account.id()
              << " balance: " << account.balance_cents() << " cents\n";

    // 기반 클래스 인터페이스를 통해 파생 클래스의 동작을 호출한다.
    const std::unique_ptr<Job> job = make_email_job("ops@example.com");
    job->run();
}
