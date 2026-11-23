#include "acme/threading/thread_pool.hpp"

#include <stdexcept>
#include <utility>

namespace acme::threading
{

ThreadPool::ThreadPool(std::size_t worker_count)
{
    if (worker_count == 0)
    {
        throw std::invalid_argument("worker_count must be greater than zero");
    }

    try
    {
        for (std::size_t index = 0; index < worker_count; ++index)
        {
            workers_.emplace_back(&ThreadPool::worker_loop, this);
        }
    }
    catch (...)
    {
        {
            const std::lock_guard<std::mutex> lock(mutex_);
            stopping_ = true;
        }
        condition_.notify_all();
        for (std::thread &worker : workers_)
        {
            worker.join();
        }
        throw;
    }
}

ThreadPool::~ThreadPool()
{
    shutdown();
}

bool ThreadPool::register_command(std::string name, std::function<void()> handler)
{
    if (name.empty() || !handler)
    {
        return false;
    }

    const std::lock_guard<std::mutex> lock(mutex_);
    if (stopping_)
    {
        return false;
    }

    return commands_.emplace(std::move(name), std::move(handler)).second;
}

bool ThreadPool::request_command(const std::string &name)
{
    {
        const std::lock_guard<std::mutex> lock(mutex_);
        const auto command = commands_.find(name);
        if (stopping_ || command == commands_.end())
        {
            return false;
        }

        // 작업 큐에는 호출 가능한 객체의 복사본을 넣어 등록 정보와 독립적으로 실행한다.
        tasks_.push(command->second);
    }

    condition_.notify_one();
    return true;
}

void ThreadPool::shutdown()
{
    {
        const std::lock_guard<std::mutex> lock(mutex_);
        stopping_ = true;
    }
    condition_.notify_all();

    for (std::thread &worker : workers_)
    {
        if (worker.joinable())
        {
            worker.join();
        }
    }
}

void ThreadPool::worker_loop()
{
    while (true)
    {
        std::function<void()> task;
        {
            // unique_lock은 condition_variable의 wait가 잠금을 잠시 풀었다 다시 잡게 해준다.
            std::unique_lock<std::mutex> lock(mutex_);
            condition_.wait(lock, [this]
                            { return stopping_ || !tasks_.empty(); });

            if (stopping_ && tasks_.empty())
            {
                return;
            }

            task = std::move(tasks_.front());
            tasks_.pop();
        }

        // 잠금을 쥔 채 사용자 명령을 실행하지 않아 다른 작업자의 큐 접근을 막지 않는다.
        task();
    }
}

} // namespace acme::threading
