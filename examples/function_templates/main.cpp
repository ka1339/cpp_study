#include <iostream>
#include <string>
#include <type_traits>
#include <vector>

struct OrderLine
{
    std::string sku;
    int quantity;
    int unit_price_cents;
};

// InputIt와 Projection은 함수 템플릿의 타입 매개변수이며, auto는 반환형 추론을 시작한다.
template <typename InputIt, typename Projection>
// -> 뒤의 후행 반환형은 매개변수 이름을 decltype에서 사용할 수 있게 한다.
auto sum_by(InputIt first, InputIt last, Projection projection)
    -> typename std::decay<decltype(projection(*first))>::type
{
    // using은 별칭을 선언하고, decltype은 식의 타입을 얻으며, decay는 참조/한정자를 정리한다.
    using Result = typename std::decay<decltype(projection(*first))>::type;

    Result total{};
    for (; first != last; ++first)
    {
        total += projection(*first);
    }
    return total;
}

int main()
{
    // 중괄호를 이용한 집합 초기화로 vector에 원소를 구성한다.
    const std::vector<OrderLine> order{
        {"COFFEE", 2, 450},
        {"SANDWICH", 1, 825}};

    // []는 람다의 캡처 목록, 매개변수 뒤의 { }는 함수 본문이다.
    const int subtotal_cents = sum_by(
        order.begin(),
        order.end(),
        // const OrderLine&는 복사 없는 읽기 전용 참조 매개변수다.
        [](const OrderLine &line)
        {
            return line.quantity * line.unit_price_cents;
        });

    std::cout << "Order subtotal: " << subtotal_cents << " cents\n";
}
