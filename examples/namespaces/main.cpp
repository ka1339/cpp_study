#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

// namespace는 이름을 범위로 묶어 같은 이름 간 충돌을 줄인다.
namespace shop
{
    // 중첩 namespace는 shop::domain처럼 :: 연산자로 구분해 참조한다.
    namespace domain
    {

        struct OrderItem
        {
            std::string name;
            int quantity;
            int unit_price_cents;
        };

        struct Order
        {
            std::vector<OrderItem> items;
        };

    } // namespace domain

    namespace service
    {

        // 타입 이름 앞의 domain::은 shop::service 내부에서 형제 namespace를 지정한다.
        int subtotal_cents(const domain::Order &order)
        {
            int subtotal = 0;
            for (const domain::OrderItem &item : order.items)
            {
                subtotal += item.quantity * item.unit_price_cents;
            }
            return subtotal;
        }

        int tax_cents(int subtotal, int tax_rate_basis_points)
        {
            return (subtotal * tax_rate_basis_points + 5000) / 10000;
        }

    } // namespace service
} // namespace shop

namespace
{

    // 이름 없는 namespace의 이름은 이 번역 단위 내부 연결을 제공한다.
    void print_money(int amount_cents)
    {
        std::cout << '$' << amount_cents / 100 << '.'
                  << std::setw(2) << std::setfill('0') << amount_cents % 100
                  << std::setfill(' ');
    }

} // namespace

int main()
{
    // namespace 별칭으로 긴 한정 이름을 짧게 쓸 수 있다. 이 별칭의 범위는 main 내부다.
    namespace domain = shop::domain;

    const domain::Order order{
        {{"Coffee", 2, 450}, {"Sandwich", 1, 825}}};
    const int subtotal = shop::service::subtotal_cents(order);
    const int tax = shop::service::tax_cents(subtotal, 1000);

    std::cout << "Order summary\n";
    for (const domain::OrderItem &item : order.items)
    {
        std::cout << "- " << item.name << " x " << item.quantity << '\n';
    }

    std::cout << "Subtotal: ";
    print_money(subtotal);
    std::cout << "\nTax:      ";
    print_money(tax);
    std::cout << "\nTotal:    ";
    print_money(subtotal + tax);
    std::cout << '\n';
}
