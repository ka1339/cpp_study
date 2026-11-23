#include <iostream>
#include <string>
#include <unordered_map>

int main()
{
    const std::unordered_map<std::string, int> word_counts{
        {"C++", 2},
        {"STL", 1}};

    std::cout << "C++ mentions: " << word_counts.at("C++") << '\n';
}
