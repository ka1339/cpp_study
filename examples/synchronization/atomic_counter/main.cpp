#include <atomic>
#include <iostream>
#include <thread>

int main()
{
    // An atomic is a good fit when each update concerns one independent value.
    std::atomic<int> completed{0};
    constexpr int work_per_thread = 10000;
    constexpr int thread_count = 2;
    constexpr int total_work = work_per_thread * thread_count;

    const auto count_work = [&completed]
    {
        for (int i = 0; i < work_per_thread; ++i)
        {
            completed.fetch_add(1);
        }
        completed.notify_all();
    };

    std::thread workers[thread_count] = {
        std::thread(count_work),
        std::thread(count_work)};

    int observed = completed.load();
    while (observed < total_work)
    {
        completed.wait(observed);
        observed = completed.load();
    }

    for (std::thread &worker : workers)
    {
        worker.join();
    }

    std::cout << "Atomic updates: " << completed.load() << '\n';
}
