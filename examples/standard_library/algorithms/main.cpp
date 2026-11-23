#include <algorithm>
#include <iostream>
#include <vector>

int main()
{
    std::vector<int> scores{82, 95, 73, 88};
    std::sort(scores.begin(), scores.end());

    const auto passing = std::find_if(
        scores.begin(),
        scores.end(),
        [](int score)
        { return score >= 80; });

    std::cout << "Sorted scores:";
    for (const int score : scores)
    {
        std::cout << ' ' << score;
    }
    if (passing != scores.end())
    {
        std::cout << "\nFirst passing score: " << *passing;
    }
    std::cout << '\n';
}
