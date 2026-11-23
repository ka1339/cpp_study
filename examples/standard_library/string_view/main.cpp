#include <iostream>
#include <string>
#include <string_view>

void print_label(std::string_view label)
{
    // string_view는 문자를 복사하지 않고 원본 텍스트를 읽기만 한다.
    std::cout << label << '\n';
}

int main()
{
    // 원본 문자열보다 string_view의 생명주기가 길어지면 안 된다.
    const std::string owned_label = "Status";
    print_label(owned_label);
    print_label("Ready");
}
