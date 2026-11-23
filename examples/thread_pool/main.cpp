#include "acme/threading/thread_pool.hpp"

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>

int main()
{
    // std::thread는 운영체제 스레드를 제공하고, 이 풀은 두 스레드를 재사용한다.
    acme::threading::ThreadPool pool(2);
    std::mutex output_mutex;
    std::mutex wait_mutex;
    std::condition_variable stop_condition;
    // atomic은 입력 스레드와 메인 스레드가 공유하는 종료 상태를 안전하게 읽고 쓴다.
    std::atomic<bool> stop_requested{false};

    // std::function<void()>는 인자와 반환값이 없는 호출 가능한 동작을 보관한다.
    // 여러 작업자가 동시에 출력할 수 있으므로 std::mutex로 출력 구간을 보호한다.
    const auto print = [&output_mutex](const char *message)
    {
        const std::lock_guard<std::mutex> lock(output_mutex);
        std::cout << message << '\n';
    };

    // 문자열 이름과 동작을 연결한다. 같은 이름을 다시 등록할 수 없다.
    if (!pool.register_command("work", [&print]
                               { print("Periodic command executed."); }) ||
        !pool.register_command("exit", [&print]
                               { print("Exit command received; waiting for queued work."); }))
    {
        std::cerr << "Could not register all commands.\n";
        return 1;
    }

    std::cout << "Requesting work every second. Type 'exit' and press Enter to stop.\n";

    // getline은 터미널 입력을 기다리는 동안 막히므로 별도 스레드에서 exit 입력을 확인한다.
    std::thread input_thread([&stop_requested, &stop_condition, &output_mutex]
    {
        std::string input;
        while (std::getline(std::cin, input))
        {
            if (input == "exit")
            {
                stop_requested.store(true);
                stop_condition.notify_one();
                return;
            }

            const std::lock_guard<std::mutex> lock(output_mutex);
            std::cout << "Unknown input. Type 'exit' to stop.\n";
        }

        // 입력 스트림이 닫힌 경우도 종료 요청으로 처리한다.
        stop_requested.store(true);
        stop_condition.notify_one();
    });

    // 매초 요청을 큐에 넣고, 종료 입력이 오면 조건 변수에서 즉시 깨어난다.
    while (!stop_requested.load())
    {
        if (!pool.request_command("work"))
        {
            std::cerr << "Could not queue the periodic command.\n";
            break;
        }

        std::unique_lock<std::mutex> lock(wait_mutex);
        stop_condition.wait_for(
            lock,
            std::chrono::seconds(1),
            [&stop_requested]
            { return stop_requested.load(); });
    }

    input_thread.join();

    // 터미널 종료 요청을 풀의 명령으로도 처리한 뒤, 큐를 비우고 작업자들을 기다린다.
    if (!pool.request_command("exit"))
    {
        std::cerr << "Could not queue the exit command.\n";
    }
    pool.shutdown();

    std::cout << "Thread pool stopped.\n";
}
