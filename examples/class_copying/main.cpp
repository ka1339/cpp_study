#include <iostream>
#include <memory>

// 얕은 복사: 자원 자체가 아니라 그 자원을 가리키는 핸들을 복사해 함께 사용한다.
// 결과를 서로 공유해야 할 때 적합하고, 한쪽 변경이 다른 쪽에도 보이는 점에 주의한다.
class SharedCounter
{
public:
    explicit SharedCounter(int value)
        : value_(std::make_shared<int>(value))
    {
    }

    // shared_ptr의 기본 복사는 포인터와 소유권을 복사하므로 두 객체가 같은 값을 공유한다.
    // 공유 상태가 의도된 경우에만 이런 얕은 복사를 선택한다.
    void set(int value)
    {
        *value_ = value;
    }

    int value() const
    {
        return *value_;
    }

private:
    std::shared_ptr<int> value_;
};

class DeepCopyCounter
{
public:
    explicit DeepCopyCounter(int value)
        : value_(std::make_unique<int>(value))
    {
    }

    // 깊은 복사: 복사본이 원본과 독립적으로 변경되어야 할 때 새 자원을 만들어 값을 복사한다.
    // 복사 생성자는 같은 형식의 const 참조를 받는다.
    DeepCopyCounter(const DeepCopyCounter &other)
        : value_(std::make_unique<int>(*other.value_))
    {
    }

    // 복사 대입 연산자는 기존 객체에 값을 대입할 때도 별도 자원을 할당한다.
    DeepCopyCounter &operator=(const DeepCopyCounter &other)
    {
        if (this != &other)
        {
            // 새 데이터 준비가 성공한 뒤 교체하므로 자기 대입과 할당 예외에 안전하다.
            auto copy = std::make_unique<int>(*other.value_);
            value_ = std::move(copy);
        }
        return *this;
    }

    void set(int value)
    {
        *value_ = value;
    }

    int value() const
    {
        return *value_;
    }

private:
    // unique_ptr는 자원을 단독 소유하며 기본 복사는 금지하므로 필요한 복사 연산을 직접 정의한다.
    std::unique_ptr<int> value_;
};

int main()
{
    // 원시 포인터 멤버를 기본 복사하면 주소만 복사된다.
    // 두 객체가 같은 자원을 소유한다고 오해하면 이중 해제나 댕글링 포인터가 생길 수 있다.
    // 자원 소유 클래스는 raw pointer 기본 복사를 피하고, Rule of 0/5를 고려한다.
    SharedCounter shared_original(10);
    SharedCounter shared_copy = shared_original; // 복사 초기화: 같은 int를 가리킨다.
    shared_copy.set(20);
    std::cout << "shared: original=" << shared_original.value()
              << ", copy=" << shared_copy.value() << '\n';

    DeepCopyCounter deep_original(10);
    DeepCopyCounter deep_copy = deep_original; // 복사 생성: 별도의 int를 만든다.
    deep_copy.set(20);
    std::cout << "deep:   original=" << deep_original.value()
              << ", copy=" << deep_copy.value() << '\n';

    DeepCopyCounter assigned(0);
    assigned = deep_original; // 복사 대입: 기존 객체에도 독립된 값을 복사한다.
    assigned.set(30);
    std::cout << "assign: original=" << deep_original.value()
              << ", copy=" << assigned.value() << '\n';
}
