#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>

int main()
{
    std::mutex mutex;
    std::condition_variable_any condition;
    bool ready = false;
    bool stopped = false;

    {
        std::jthread worker([&](std::stop_token stop_token)
                            {
                                std::unique_lock<std::mutex> lock(mutex);
                                const bool predicate_satisfied =
                                    condition.wait(lock, stop_token, [&ready]
                                                   { return ready; });
                                stopped = !predicate_satisfied;
                            });

        worker.request_stop();
    }

    std::cout << (stopped ? "Worker stopped cooperatively\n" : "Worker became ready\n");
}
