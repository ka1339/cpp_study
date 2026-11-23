#include <semaphore>
#include <iostream>
#include <syncstream>
#include <thread>
#include <vector>

int main()
{
    constexpr int worker_count = 5;
    std::counting_semaphore<2> available_slots(2);
    std::vector<std::thread> workers;

    for (int id = 1; id <= worker_count; ++id)
    {
        workers.emplace_back([id, &available_slots]
                             {
                                 available_slots.acquire();
                                 std::osyncstream(std::cout) << "Worker " << id << " acquired a slot\n";
                                 available_slots.release();
                             });
    }

    for (std::thread &worker : workers)
    {
        worker.join();
    }
}
