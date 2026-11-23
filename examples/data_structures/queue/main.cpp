#include <iostream>
#include <queue>

int main()
{
    // queue는 먼저 넣은 원소를 먼저 꺼내는 FIFO 자료구조다.
    std::queue<int> requests;
    requests.push(101);
    requests.push(102);

    std::cout << "Next request: " << requests.front() << '\n';
    requests.pop();
    std::cout << "Next request: " << requests.front() << '\n';
}
