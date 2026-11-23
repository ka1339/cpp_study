#include <algorithm>
#include <iostream>
#include <optional>
#include <vector>

std::optional<int> find_score(const std::vector<int> &scores, int target)
{
    const auto found = std::find(scores.begin(), scores.end(), target);
    if (found == scores.end())
    {
        return std::nullopt;
    }

    return *found;
}

int main()
{
    const std::vector<int> scores{82, 95, 73, 88};
    const std::optional<int> score = find_score(scores, 95);
    if (score)
    {
        std::cout << "Found score: " << *score << '\n';
    }
    else
    {
        std::cout << "Score not found\n";
    }
}
