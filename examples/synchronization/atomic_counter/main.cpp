#include <atomic>
#include <iostream>
#include <thread>

int main()
{
    // An atomic is a good fit when each update concerns one independent value.
    std::atomic<int> completed{0};
    const auto count_work = [&completed]
    {
        for (int i = 0; i < 10000; ++i)
        {
            completed.fetch_add(1);
        }
    };

    std::thread first_worker(count_work);
    std::thread second_worker(count_work);
    first_worker.join();
    second_worker.join();

    std::cout << "Atomic updates: " << completed.load() << '\n';
}
