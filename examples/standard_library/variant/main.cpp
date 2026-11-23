#include <iostream>
#include <string>
#include <variant>

int main()
{
    // variant는 나열된 타입 중 정확히 하나만 소유하며 타입 안전한 분기를 제공한다.
    const std::variant<int, std::string> product_id = std::string("BOOK-42");
    std::visit(
        [](const auto &value)
        {
            std::cout << "Product ID: " << value << '\n';
        },
        product_id);
}
