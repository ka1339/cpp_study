#include "acme/orders/order.hpp"

namespace acme::orders
{

namespace
{

// 구현에만 필요한 계산은 공개 헤더에 노출하지 않는다.
int line_total_cents(const Line &line)
{
    return line.quantity * line.unit_price_cents;
}

} // namespace

int calculate(const Order &order)
{
    int subtotal = 0;
    for (const Line &line : order.lines)
    {
        subtotal += line_total_cents(line);
    }
    return subtotal;
}

} // namespace acme::orders
