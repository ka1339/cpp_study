#include <iostream>
#include <string>
#include <unordered_map>

int main()
{
    // unordered_map은 키로 평균 상수 시간 조회를 제공하지만 반복 순서는 지정되지 않는다.
    const std::unordered_map<std::string, int> word_counts{
        {"C++", 2},
        {"STL", 1}};

    std::cout << "C++ mentions: " << word_counts.at("C++") << '\n';
}
