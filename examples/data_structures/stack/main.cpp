#include <iostream>
#include <stack>
#include <string>

int main()
{
    // stack은 top과 pop만으로 가장 최근 원소를 관리하는 LIFO 자료구조다.
    std::stack<std::string> undo_history;
    undo_history.push("create");
    undo_history.push("rename");

    std::cout << "Undo: " << undo_history.top() << '\n';
    undo_history.pop();
}
