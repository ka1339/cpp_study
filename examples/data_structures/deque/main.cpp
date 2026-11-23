#include <deque>
#include <iostream>
#include <string>

int main()
{
    // deque는 양쪽 끝 삽입과 삭제가 효율적이며, vector와 달리 처음 원소도
    // 메모리를 재할당하지 않고 O(1)로 삽입/삭제할 수 있다.
    std::deque<std::string> tasks{"review", "test"};

    tasks.push_front("fix");  // 앞쪽에 추가
    tasks.push_back("release"); // 뒤쪽에 추가

    // deque는 []와 at으로 인덱스 접근을 지원한다.
    std::cout << "Tasks: ";
    for (std::size_t index = 0; index < tasks.size(); ++index)
    {
        if (index != 0)
        {
            std::cout << " -> ";
        }
        std::cout << tasks[index];
    }
    std::cout << '\n';

    std::cout << "First: " << tasks.front()
              << ", last: " << tasks.back() << '\n';

    tasks.pop_front();
    tasks.pop_back();

    std::cout << "Remaining: ";
    for (const std::string &task : tasks)
    {
        std::cout << task << ' ';
    }
    std::cout << '\n';
}
