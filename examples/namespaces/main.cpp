#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

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

    void print_money(int amount_cents)
    {
        std::cout << '$' << amount_cents / 100 << '.'
                  << std::setw(2) << std::setfill('0') << amount_cents % 100
                  << std::setfill(' ');
    }

} // namespace

int main()
{
    namespace domain = shop::domain;

    const domain::Order order{
        {{"Coffee", 2, 450}, {"Sandwich", 1, 825}}};
    const int subtotal = shop::service::subtotal_cents(order);
    const int tax = shop::service::tax_cents(subtotal, 1000); // 10%

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
