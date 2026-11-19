#include "acme/tax/tax.hpp"

namespace acme::tax
{

int calculate(int subtotal_cents, int tax_rate_basis_points)
{
    // 정수 나눗셈 전에 5000을 더해 결과를 가장 가까운 센트로 반올림한다.
    return (subtotal_cents * tax_rate_basis_points + 5000) / 10000;
}

} // namespace acme::tax
