#include <barrier>
#include <iostream>
#include <latch>
#include <syncstream>
#include <thread>
#include <vector>

int main()
{
    constexpr int worker_count = 3;
    std::latch initialized(worker_count);
    std::barrier phase_boundary(worker_count);
    std::vector<std::thread> workers;

    for (int id = 1; id <= worker_count; ++id)
    {
        workers.emplace_back([id, &initialized, &phase_boundary]
                             {
                                 std::osyncstream(std::cout) << "Worker " << id << " initialized\n";
                                 initialized.count_down();
                                 phase_boundary.arrive_and_wait();

                                 std::osyncstream(std::cout) << "Worker " << id << " completed phase one\n";
                                 phase_boundary.arrive_and_wait();

                                 std::osyncstream(std::cout) << "Worker " << id << " completed phase two\n";
                             });
    }

    initialized.wait();
    std::cout << "All workers initialized\n";

    for (std::thread &worker : workers)
    {
        worker.join();
    }
}
