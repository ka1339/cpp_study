#include <iostream>
#include <string>
#include <variant>

int main()
{
    // variant owns exactly one of its listed value types at a time.
    const std::variant<int, std::string> product_id = std::string("BOOK-42");
    std::visit(
        [](const auto &value)
        {
            std::cout << "Product ID: " << value << '\n';
        },
        product_id);
}
