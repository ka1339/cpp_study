#include <iostream>
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <vector>

int main()
{
    int counter = 0;
    std::mutex counter_mutex;
    std::vector<std::thread> workers;

    for (int i = 0; i < 4; ++i)
    {
        workers.emplace_back([&counter, &counter_mutex]
                             {
                                 for (int j = 0; j < 1000; ++j)
                                 {
                                     const std::lock_guard<std::mutex> lock(counter_mutex);
                                     ++counter;
                                 }
                             });
    }
    for (std::thread &worker : workers)
    {
        worker.join();
    }

    std::mutex first_mutex;
    std::mutex second_mutex;
    int first_value = 10;
    int second_value = 20;
    {
        const std::scoped_lock lock(first_mutex, second_mutex);
        first_value -= 5;
        second_value += 5;
    }

    std::shared_mutex settings_mutex;
    int setting = 0;
    {
        const std::unique_lock<std::shared_mutex> lock(settings_mutex);
        setting = 42;
    }
    int read_setting = 0;
    {
        const std::shared_lock<std::shared_mutex> lock(settings_mutex);
        read_setting = setting;
    }

    std::once_flag initialization_flag;
    int initialized_value = 0;
    workers.clear();
    for (int i = 0; i < 4; ++i)
    {
        workers.emplace_back([&initialization_flag, &initialized_value]
                             {
                                 std::call_once(initialization_flag, [&initialized_value]
                                                { initialized_value = 7; });
                             });
    }
    for (std::thread &worker : workers)
    {
        worker.join();
    }

    std::cout << "Mutex-protected counter: " << counter << '\n'
              << "Scoped-lock values: " << first_value << ", " << second_value << '\n'
              << "Shared-mutex setting: " << read_setting << '\n'
              << "call_once initialization: " << initialized_value << '\n';
}
