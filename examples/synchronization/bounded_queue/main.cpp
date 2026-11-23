#include <condition_variable>
#include <cstddef>
#include <iostream>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <thread>

class BoundedQueue
{
public:
    explicit BoundedQueue(std::size_t capacity)
        : capacity_(capacity)
    {
        if (capacity_ == 0)
        {
            throw std::invalid_argument("capacity must be greater than zero");
        }
    }

    bool push(int value)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        not_full_.wait(lock, [this]
                       { return values_.size() < capacity_ || closed_; });
        if (closed_)
        {
            return false;
        }

        values_.push(value);
        lock.unlock();
        not_empty_.notify_one();
        return true;
    }

    bool pop(int &value)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        not_empty_.wait(lock, [this]
                        { return !values_.empty() || closed_; });
        if (values_.empty())
        {
            return false;
        }

        value = values_.front();
        values_.pop();
        lock.unlock();
        not_full_.notify_one();
        return true;
    }

    void close()
    {
        {
            const std::lock_guard<std::mutex> lock(mutex_);
            closed_ = true;
        }
        not_empty_.notify_all();
        not_full_.notify_all();
    }

private:
    const std::size_t capacity_;
    std::queue<int> values_;
    std::mutex mutex_;
    std::condition_variable not_empty_;
    std::condition_variable not_full_;
    bool closed_{false};
};

int main()
{
    // The mutex protects the queue and its closed flag as one shared state.
    // Condition variables let threads sleep until the corresponding predicate holds.
    BoundedQueue queue(2);
    std::thread producer([&queue]
                         {
                             for (int value = 1; value <= 5; ++value)
                             {
                                 queue.push(value);
                             }
                             queue.close();
                         });

    int value = 0;
    while (queue.pop(value))
    {
        std::cout << "Consumed: " << value << '\n';
    }
    producer.join();
}
