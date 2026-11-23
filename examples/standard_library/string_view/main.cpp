#include <iostream>
#include <string>
#include <string_view>

void print_label(std::string_view label)
{
    std::cout << label << '\n';
}

int main()
{
    const std::string owned_label = "Status";
    print_label(owned_label);
    print_label("Ready");
}
