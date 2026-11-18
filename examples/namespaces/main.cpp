#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

// shop 아래에서 주문 데이터(domain)와 계산 로직(service)을 분리한다.
namespace shop
{
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

        // 주문의 각 항목에 대해 수량 x 단가를 계산해 소계를 구한다.
        int subtotal_cents(const domain::Order &order)
        {
            int subtotal = 0;
            for (const domain::OrderItem &item : order.items)
            {
                subtotal += item.quantity * item.unit_price_cents;
            }
            return subtotal;
        }

        // 세율은 1/100 퍼센트 단위(basis points)로 받고 센트 단위 세금을 반올림한다.
        int tax_cents(int subtotal, int tax_rate_basis_points)
        {
            return (subtotal * tax_rate_basis_points + 5000) / 10000;
        }

    } // namespace service
} // namespace shop

namespace
{

    // 이 소스 파일 안에서만 사용하는 금액 출력 함수.
    void print_money(int amount_cents)
    {
        std::cout << '$' << amount_cents / 100 << '.'
                  << std::setw(2) << std::setfill('0') << amount_cents % 100
                  << std::setfill(' ');
    }

} // namespace

int main()
{
    // 긴 네임스페이스 이름 대신 이 함수 안에서만 쓸 별칭을 선언한다.
    namespace domain = shop::domain;

    // 커피 2개와 샌드위치 1개로 주문을 구성하고 소계와 10% 세금을 계산한다.
    const domain::Order order{
        {{"Coffee", 2, 450}, {"Sandwich", 1, 825}}};
    const int subtotal = shop::service::subtotal_cents(order);
    const int tax = shop::service::tax_cents(subtotal, 1000); // 1000 basis points = 10%

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
