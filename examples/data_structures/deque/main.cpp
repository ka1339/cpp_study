#include <deque>
#include <iostream>
#include <string>

int main()
{
    std::deque<std::string> tasks{"review"};
    tasks.push_front("fix");
    tasks.push_back("release");

    std::cout << "First task: " << tasks.front() << '\n';
    tasks.pop_front();
    std::cout << "Last task: " << tasks.back() << '\n';
}
