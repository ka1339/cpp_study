#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

class ServerConfig
{
public:
    // 매개변수를 값으로 받아 멤버에 저장하고, 호출자가 넘긴 임시 문자열은 move로 효율적으로 옮긴다.
    // explicit은 인자 하나로 암시적 변환 생성이 일어나는 것을 막는다.
    explicit ServerConfig(std::string host)
        // 생성자 위임은 멤버 초기화 목록에서 같은 클래스의 다른 생성자를 호출한다.
        : ServerConfig(std::move(host), 8080)
    {
    }

    ServerConfig(std::string host, int port)
        // 초기화 목록은 멤버 선언 순서로 적용되며, 괄호 대신 중괄호 초기화도 사용할 수 있다.
        // endpoint_에서 앞서 초기화한 host_와 port_를 사용한다.
        : host_(validate_host(std::move(host))),
          port_(validate_port(port)),
          endpoint_(host_ + ":" + std::to_string(port_))
    {
    }

    // 반환형의 &는 참조, 함수 끝의 const는 *this를 통한 멤버 변경 금지를 나타낸다.
    const std::string &endpoint() const
    {
        return endpoint_;
    }

    int timeout_seconds() const
    {
        return timeout_seconds_;
    }

private:
    // 생성자에서 공통으로 검증하면 유효하지 않은 상태의 객체가 만들어지는 것을 막을 수 있다.
    // static 멤버 함수는 객체 인스턴스 없이 호출할 수 있으며, 클래스 내부에서 선언한다.
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
    // 멤버 선언의 중괄호 초기화자는 생성자 초기화 목록에서 값이 생략될 때 쓰인다.
    int timeout_seconds_{30};
};

class Account
{
public:
    // 검증에 실패하면 invalid_argument 예외를 던지고 객체 생성이 완료되지 않는다.
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

    // const 데이터 멤버는 대입할 수 없으므로 생성자 초기화 목록에서 초기화한다.
    const std::string id_;
    int balance_cents_;
};

class Job
{
public:
    // virtual 소멸자와 = default는 기반 클래스 포인터로 파생 객체를 소멸할 때 필요하다.
    virtual ~Job() = default;
    // = 0은 순수 가상 함수로, 이 클래스가 추상 클래스가 되게 한다.
    virtual void run() const = 0;
};

// final은 이 클래스를 더 파생할 수 없게 한다.
class EmailJob final : public Job
{
public:
    // std::move는 recipient의 내용을 옮길 수 있게 하며, 원래 변수는 유효하지만 값은 지정되지 않은 상태가 된다.
    explicit EmailJob(std::string recipient)
        : recipient_(std::move(recipient))
    {
    }

    // override는 기반 클래스의 가상 함수 재정의를 명시하고, 시그니처 오류를 검사한다.
    void run() const override
    {
        std::cout << "Sending email to " << recipient_ << '\n';
    }

private:
    std::string recipient_;
};

std::unique_ptr<Job> make_email_job(std::string recipient)
{
    // make_unique는 동적 객체를 만들고, unique_ptr<Job>은 소유권을 단독 관리한다.
    // 반환 시 소유권이 호출자에게 이동하므로 raw new/delete를 직접 관리할 필요가 없다.
    return std::make_unique<EmailJob>(std::move(recipient));
}

int main()
{
    // const 객체는 const 멤버 함수만 호출할 수 있어 읽기 전용 설정값 표현에 적합하다.
    const ServerConfig config("api.example.com");
    std::cout << "Endpoint: " << config.endpoint()
              << ", timeout: " << config.timeout_seconds() << " seconds\n";

    const Account account("account-1001", 25000);
    std::cout << "Account " << account.id()
              << " balance: " << account.balance_cents() << " cents\n";

    // unique_ptr는 복사할 수 없고 소유권을 이동한다. 포인터처럼 ->로 객체의 멤버 함수를 호출한다.
    const std::unique_ptr<Job> job = make_email_job("ops@example.com");
    job->run();
}
