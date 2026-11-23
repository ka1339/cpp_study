#include <iostream>
#include <vector>

int main()
{
    std::vector<int> scores{82, 95, 73};
    scores.push_back(88);
    std::cout << "Second score: " << scores[1] << '\n';
    std::cout << "Score count: " << scores.size() << '\n';
}
