#pragma once

#include <condition_variable>
#include <cstddef>
#include <functional>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

namespace acme::threading
{

// std::queue는 먼저 들어온 작업을 먼저 꺼내고, std::unordered_map은 이름으로 명령을 찾는다.
// mutex와 condition_variable은 큐/등록 정보에 대한 동시 접근과 작업자 대기를 조정한다.
class ThreadPool
{
public:
    // worker_count는 동시에 명령을 처리할 작업자 스레드의 수다.
    explicit ThreadPool(std::size_t worker_count);
    ~ThreadPool();

    ThreadPool(const ThreadPool &) = delete;
    ThreadPool &operator=(const ThreadPool &) = delete;

    // 같은 이름이 이미 있거나 풀이 종료 중이면 false를 반환한다.
    bool register_command(std::string name, std::function<void()> handler);

    // 등록된 명령을 작업 큐에 넣는다. 명령이 없거나 종료 중이면 false다.
    bool request_command(const std::string &name);

    // 새 명령을 받지 않고, 이미 큐에 들어간 작업을 마친 다음 작업자들을 기다린다.
    void shutdown();

private:
    void worker_loop();

    std::mutex mutex_;
    std::condition_variable condition_;
    std::queue<std::function<void()>> tasks_;
    std::unordered_map<std::string, std::function<void()>> commands_;
    std::vector<std::thread> workers_;
    bool stopping_{false};
};

} // namespace acme::threading
