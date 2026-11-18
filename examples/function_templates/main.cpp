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

template <typename InputIt, typename Projection>
auto sum_by(InputIt first, InputIt last, Projection projection)
    -> typename std::decay<decltype(projection(*first))>::type
{
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
    const std::vector<OrderLine> order{
        {"COFFEE", 2, 450},
        {"SANDWICH", 1, 825}};

    const int subtotal_cents = sum_by(
        order.begin(),
        order.end(),
        [](const OrderLine &line)
        {
            return line.quantity * line.unit_price_cents;
        });

    std::cout << "Order subtotal: " << subtotal_cents << " cents\n";
}
