#include <iostream>
#include <string>
#include <unordered_map>
#include <utility>

template <typename Key, typename Value>
class Repository
{
public:
    void save(Key key, Value value)
    {
        values_.insert_or_assign(std::move(key), std::move(value));
    }

    const Value *find(const Key &key) const
    {
        const auto item = values_.find(key);
        return item == values_.end() ? nullptr : &item->second;
    }

    bool erase(const Key &key)
    {
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
    Repository<std::string, User> users;
    users.save("user-1001", {"Mina", "mina@example.com"});

    if (const User *user = users.find("user-1001"))
    {
        std::cout << user->name << " <" << user->email << ">\n";
    }
}
