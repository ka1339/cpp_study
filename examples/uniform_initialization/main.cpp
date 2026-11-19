#include <initializer_list>
#include <iostream>
#include <vector>

// 유니폼 초기화(중괄호 초기화)는 여러 타입에 일관된 초기화 문법을 제공하고,
// 기본형의 미초기화 상태와 의도치 않은 좁아지는 변환을 줄이기 위해 사용한다.
// 다만 괄호 초기화와 의미가 항상 같지는 않으므로 생성자 선택을 확인해야 한다.
struct Point
{
    int x;
    int y;
};

class ConstructorChoice
{
public:
    explicit ConstructorChoice(int value)
        : constructor_("int constructor"), value_(value)
    {
    }

    // 중괄호 초기화에서 유효한 initializer_list 생성자가 있으면 다른 생성자보다 우선 선택된다.
    ConstructorChoice(std::initializer_list<int> values)
        : constructor_("initializer_list constructor"), value_(values.size())
    {
    }

    void print() const
    {
        std::cout << constructor_ << ", value=" << value_ << '\n';
    }

private:
    const char *constructor_;
    int value_;
};

void print_values(const std::vector<int> &values)
{
    for (int value : values)
    {
        std::cout << value << ' ';
    }
    std::cout << '\n';
}

int main()
{
    // 기본형도 괄호, 복사 초기화, 직접 목록 초기화, 복사 목록 초기화로 초기화할 수 있다.
    int parentheses(7);
    int copy = 7;
    int direct_list{7};
    int copy_list = {7};
    // 빈 중괄호는 기본형을 0으로 값 초기화해, 초기값을 빠뜨려 미정 값이 되는 실수를 예방한다.
    int zero{};

    std::cout << "integers: " << parentheses << ' ' << copy << ' '
              << direct_list << ' ' << copy_list << ", zero=" << zero << '\n';

    // 집합체는 멤버 선언 순서대로 중괄호 안의 값을 대응시킨다.
    const Point point{3, 4};
    std::cout << "point: (" << point.x << ", " << point.y << ")\n";

    // 값의 손실이 생길 수 있는 변환을 컴파일 단계에서 막아, 타입 실수를 일찍 발견하는 데 유용하다.
    int whole_number{42};
    double fraction{0.5};
    // int invalid{3.14}; // double에서 int로의 좁아지는 변환이므로 컴파일되지 않는다.
    std::cout << "numbers: " << whole_number << ", " << fraction << '\n';

    // 괄호는 vector의 (개수, 값) 생성자를 호출해 7을 세 번 저장한다.
    // 이처럼 생성자의 인자 의미가 중요하면 괄호를 선택해 initializer_list 오버로드를 피한다.
    const std::vector<int> repeated_values(3, 7);
    // 중괄호는 initializer_list 생성자를 선택해 3과 7을 각각 원소로 저장한다.
    const std::vector<int> listed_values{3, 7};
    std::cout << "parentheses vector: ";
    print_values(repeated_values);
    std::cout << "braces vector: ";
    print_values(listed_values);

    // 괄호는 int 생성자를, 중괄호는 우선순위가 높은 initializer_list 생성자를 고른다.
    const ConstructorChoice parentheses_choice(10);
    const ConstructorChoice braces_choice{10};
    parentheses_choice.print();
    braces_choice.print();
}
