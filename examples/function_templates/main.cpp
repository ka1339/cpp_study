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

// 반복자 범위의 각 항목에 projection을 적용하고 그 결과를 합산한다.
// 반환형은 projection 결과에서 참조와 const 한정자를 제거해 정한다.
template <typename InputIt, typename Projection>
auto sum_by(InputIt first, InputIt last, Projection projection)
    -> typename std::decay<decltype(projection(*first))>::type
{
    using Result = typename std::decay<decltype(projection(*first))>::type;

    // 기본값 0에서 시작하므로 빈 범위이면 0을 반환한다.
    Result total{};
    for (; first != last; ++first)
    {
        // 원본 항목 자체가 아니라 projection이 뽑아낸 값만 더한다.
        total += projection(*first);
    }
    return total;
}

int main()
{
    // 서로 다른 상품의 주문 수량과 개당 가격을 저장한 예시 데이터.
    const std::vector<OrderLine> order{
        {"COFFEE", 2, 450},
        {"SANDWICH", 1, 825}};

    // 람다를 projection으로 전달해 각 주문 항목의 금액(수량 x 단가)을 합산한다.
    const int subtotal_cents = sum_by(
        order.begin(),
        order.end(),
        [](const OrderLine &line)
        {
            return line.quantity * line.unit_price_cents;
        });

    std::cout << "Order subtotal: " << subtotal_cents << " cents\n";
}
