#include <iostream>
#include <set>
#include <string>

int main()
{
    // set은 중복을 허용하지 않고 원소를 키의 오름차순으로 유지한다.
    std::set<std::string> roles{"viewer", "editor"};

    roles.insert("admin");
    roles.insert("editor"); // 이미 존재하므로 아무 변화가 없다.

    const auto found = roles.find("editor");
    if (found != roles.end())
    {
        // find가 돌려준 반복자는 해당 원소를 읽을 수 있지만 set에서는 변경할 수 없다.
        std::cout << "Found role: " << *found << '\n';
    }

    std::cout << "Roles:";
    for (const std::string &role : roles)
    {
        std::cout << ' ' << role;
    }
    std::cout << '\n';

    roles.erase("viewer");
    std::cout << "Role count after erase: " << roles.size() << '\n';
}
