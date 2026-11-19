#pragma once

#include <string>
#include <vector>

// 조직/제품 이름을 최상위 namespace로 두면 다른 라이브러리와의 전역 이름 충돌을 줄일 수 있다.
// 컴포넌트 팀은 자기 하위 namespace에 공개 타입과 함수 선언을 소유한다.
// namespace는 이름을 묶는 도구이지 접근 제어가 아니므로, 실제 비공개 구현은 헤더에 노출하지 않는다.
namespace acme::orders
{

struct Line
{
    std::string name;
    int quantity;
    int unit_price_cents;
};

struct Order
{
    std::vector<Line> lines;
};

// 헤더에는 다른 컴포넌트가 사용할 공개 계약을 선언하고, 구현은 .cpp 파일에 둔다.
int calculate(const Order &order);

} // namespace acme::orders
