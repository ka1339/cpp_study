#include <iostream>
#include <queue>

int main()
{
    std::queue<int> requests;
    requests.push(101);
    requests.push(102);

    std::cout << "Next request: " << requests.front() << '\n';
    requests.pop();
    std::cout << "Next request: " << requests.front() << '\n';
}
