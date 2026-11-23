#include <future>
#include <iostream>
#include <thread>

int main()
{
    std::future<int> calculation = std::async(std::launch::async, []
                                              { return 6 * 7; });

    std::promise<int> result_promise;
    std::future<int> result = result_promise.get_future();
    std::thread worker([promise = std::move(result_promise)]() mutable
                       { promise.set_value(100); });

    std::cout << "Async result: " << calculation.get() << '\n'
              << "Promised result: " << result.get() << '\n';
    worker.join();
}
