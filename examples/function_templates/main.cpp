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
// 반복자는 순회 가능해야 하고 projection은 각 원소에서 더할 수 있는 결과를 돌려줘야 한다.
template <typename InputIt, typename Projection>
// -> 뒤의 후행 반환형은 매개변수 이름을 decltype에서 사용할 수 있게 한다.
auto sum_by(InputIt first, InputIt last, Projection projection)
    -> typename std::decay<decltype(projection(*first))>::type
{
    // decltype의 *first는 타입만 알아내는 식이라 실제로 반복자를 역참조하지 않는다.
    // using은 별칭을 선언하고, decay는 참조/const 한정자를 제거해 누적 가능한 값 타입을 만든다.
    using Result = typename std::decay<decltype(projection(*first))>::type;

    // {}는 숫자형 결과를 0으로 초기화한다. 합산 타입은 기본 초기화 및 += 연산을 지원해야 한다.
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

    // projection을 매개변수로 받으므로 합산할 필드를 호출 지점에서 선택할 수 있다.
    // []는 람다의 캡처 목록, 매개변수 뒤의 { }는 함수 본문이다.
    const int subtotal_cents = sum_by(
        order.begin(),
        order.end(),
        // const OrderLine&는 복사 없는 읽기 전용 참조 매개변수다.
        [](const OrderLine &line)
        {
            // 각 품목의 수량과 단가를 곱해 정수 센트 단위로 반환한다.
            return line.quantity * line.unit_price_cents;
        });

    std::cout << "Order subtotal: " << subtotal_cents << " cents\n";
}
