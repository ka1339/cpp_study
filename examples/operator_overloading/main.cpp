#include <iostream>

// 연산자 오버로드는 새 기호를 만들거나 우선순위를 바꾸지 못한다.
// 기존 연산자의 익숙한 의미와 맞을 때만 오버로드해야 한다.
// 이 예제는 같은 통화끼리 계산한다고 가정하며, 실제 코드에서는 범위 초과도 처리해야 한다.
class Money
{
public:
    // explicit은 정수에서 Money로의 의도하지 않은 암시적 변환을 막는다.
    explicit Money(int cents)
        : cents_(cents)
    {
    }

    // 복합 대입 연산자는 현재 객체를 변경하고, *this의 참조를 반환해 연쇄 대입을 지원한다.
    Money &operator+=(const Money &other)
    {
        cents_ += other.cents_;
        return *this;
    }

    // 비교 연산자는 객체를 바꾸지 않으므로 const 멤버 함수로 선언하고 bool을 반환한다.
    bool operator==(const Money &other) const
    {
        return cents_ == other.cents_;
    }

    int cents() const
    {
        return cents_;
    }

private:
    int cents_;
};

// +는 좌변을 값으로 받아 복사본을 수정하고 반환하므로 원래 좌변은 바뀌지 않는다.
// 비멤버로 정의하면 좌우 피연산자의 역할이 대칭적이며, +=의 구현을 재사용할 수 있다.
Money operator+(Money left, const Money &right)
{
    left += right;
    return left;
}

// <<의 왼쪽 피연산자는 ostream이므로 Money의 멤버 함수로 만들 수 없다.
// 비멤버 함수는 ostream 참조를 반환해 출력 연쇄를 가능하게 한다.
std::ostream &operator<<(std::ostream &out, const Money &money)
{
    return out << money.cents() << " cents";
}

int main()
{
    const Money item_price(450);
    const Money shipping(125);

    const Money total = item_price + shipping;
    std::cout << "Total: " << total << '\n';

    Money adjusted_total = total;
    adjusted_total += Money(25);
    std::cout << "Adjusted: " << adjusted_total << '\n';

    const Money expected(600);
    std::cout << "Matches expected: "
              << (adjusted_total == expected ? "yes" : "no") << '\n';
}
