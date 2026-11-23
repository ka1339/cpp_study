#include <iostream>
#include <stack>
#include <string>

int main()
{
    std::stack<std::string> undo_history;
    undo_history.push("create");
    undo_history.push("rename");

    std::cout << "Undo: " << undo_history.top() << '\n';
    undo_history.pop();
}
