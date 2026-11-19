#include "acme/orders/order.hpp"
#include "acme/tax/tax.hpp"

#include <iomanip>
#include <iostream>

// 별칭은 자주 쓰는 긴 이름을 줄이지만, 소스 파일 안에만 선언해 다른 파일로 새지 않게 한다.
namespace orders = acme::orders;

// using namespace 지시문은 다른 팀의 이름을 현재 범위로 가져와 충돌을 숨길 수 있어 사용하지 않는다.
namespace
{

    // 이름 없는 namespace는 이 구현 파일에서만 쓰는 함수를 다른 파일에 노출하지 않는다.
    void print_money(int amount_cents)
    {
        std::cout << '$' << amount_cents / 100 << '.'
                  << std::setw(2) << std::setfill('0') << amount_cents % 100
                  << std::setfill(' ');
    }

} // namespace

int main()
{
    // 애플리케이션은 각 컴포넌트의 공개 API만 포함해 업무 흐름을 조합한다.
    const orders::Order order{
        {{"Coffee", 2, 450}, {"Sandwich", 1, 825}}};

    // 같은 이름 calculate라도 완전 한정 이름으로 호출하면 담당 컴포넌트와 의미가 분명하다.
    const int subtotal = orders::calculate(order);
    const int tax = acme::tax::calculate(subtotal, 1000);

    std::cout << "Order summary\n";
    for (const orders::Line &line : order.lines)
    {
        std::cout << "- " << line.name << " x " << line.quantity << '\n';
    }

    std::cout << "Subtotal: ";
    print_money(subtotal);
    std::cout << "\nTax:      ";
    print_money(tax);
    std::cout << "\nTotal:    ";
    print_money(subtotal + tax);
    std::cout << '\n';
}
