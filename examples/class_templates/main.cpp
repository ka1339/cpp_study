#include <iostream>
#include <string>
#include <unordered_map>
#include <utility>

// template <typename ...>은 타입 매개변수로 클래스의 인스턴스화를 일반화한다.
// 이 템플릿은 Key가 해시 가능하고, Value가 복사/이동 및 대입 가능해야 한다.
template <typename Key, typename Value>
class Repository
{
public:
    // 값으로 받은 뒤 move하면 lvalue 인자는 복사되고 rvalue 인자는 이동될 수 있다.
    void save(Key key, Value value)
    {
        values_.insert_or_assign(std::move(key), std::move(value));
    }

    // const Key&는 키 복사를 피하고, 반환형 const Value*는 값을 수정하지 못하게 한다.
    // nullptr은 키가 없음을 나타낸다. 반환 포인터는 해당 항목을 지우거나 저장소가 파괴되면 사용할 수 없다.
    const Value *find(const Key &key) const
    {
        const auto item = values_.find(key);
        // ?:는 조건 연산자이며, nullptr은 유효한 객체를 가리키지 않는 포인터 값이다.
        return item == values_.end() ? nullptr : &item->second;
    }

    bool erase(const Key &key)
    {
        // erase는 실제로 원소를 지운 개수를 반환한다. 여기서는 성공 여부를 bool로 바꿔 돌려준다.
        return values_.erase(key) != 0;
    }

private:
    std::unordered_map<Key, Value> values_;
};

struct User
{
    std::string name;
    std::string email;
};

int main()
{
    // Repository<std::string, User>는 템플릿 매개변수에 실제 타입을 대입해 구체화한다.
    Repository<std::string, User> users;
    // insert_or_assign은 키가 새 것이면 삽입하고, 이미 있으면 값을 교체한다.
    users.save("user-1001", {"Mina", "mina@example.com"});

    // if의 조건식 안에서 선언한 포인터는 if 문 범위에서 사용할 수 있다.
    if (const User *user = users.find("user-1001"))
    {
        std::cout << user->name << " <" << user->email << ">\n";
    }
}
