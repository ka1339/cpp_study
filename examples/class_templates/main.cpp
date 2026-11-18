#include <iostream>
#include <string>
#include <unordered_map>
#include <utility>

// Key와 Value 타입을 매개변수로 받아 다양한 자료형에 재사용할 수 있는 저장소.
template <typename Key, typename Value>
class Repository
{
public:
    void save(Key key, Value value)
    {
        // 같은 키가 이미 있으면 값을 바꾸고, 없으면 새 항목을 저장한다.
        values_.insert_or_assign(std::move(key), std::move(value));
    }

    const Value *find(const Key &key) const
    {
        const auto item = values_.find(key);
        // 항목이 없으면 nullptr, 있으면 내부 값의 읽기 전용 주소를 반환한다.
        return item == values_.end() ? nullptr : &item->second;
    }

    bool erase(const Key &key)
    {
        // 실제로 항목 하나를 지웠는지 여부를 반환한다.
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
    // 이 인스턴스는 문자열 키와 User 값을 저장하도록 템플릿을 구체화한다.
    Repository<std::string, User> users;
    users.save("user-1001", {"Mina", "mina@example.com"});

    // find 결과를 확인해 키가 존재할 때만 사용자 정보를 출력한다.
    if (const User *user = users.find("user-1001"))
    {
        std::cout << user->name << " <" << user->email << ">\n";
    }
}
