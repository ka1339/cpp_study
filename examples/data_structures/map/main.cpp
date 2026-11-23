#include <iostream>
#include <map>
#include <string>

int main()
{
    const std::map<std::string, int> inventory{
        {"book", 12},
        {"pen", 30}};

    // Iteration follows key order.
    for (const auto &item : inventory)
    {
        std::cout << item.first << ": " << item.second << '\n';
    }
}
