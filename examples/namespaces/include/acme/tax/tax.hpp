#pragma once

// tax 컴포넌트는 orders 헤더에 의존하지 않고, 필요한 입력만 공개 API로 받는다.
namespace acme::tax
{

// tax_rate_basis_points는 10000이 100%인 세율 단위다.
int calculate(int subtotal_cents, int tax_rate_basis_points);

} // namespace acme::tax
