#include <iostream>
#include <vector>

int main()
{
    // vector는 원소를 메모리에 연속으로 저장하므로 인덱스 접근과 끝 원소 추가가 빠르다.
    std::vector<int> scores{82, 95, 73};
    scores.push_back(88);

    // []는 범위를 검사하지 않으므로 유효한 인덱스를 전달해야 한다.
    std::cout << "Second score: " << scores[1] << '\n';
    std::cout << "Score count: " << scores.size() << '\n';
}
