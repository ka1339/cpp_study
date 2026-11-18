#include <iostream>
#include <sstream>
#include <string>
#include <utility>

enum class LogLevel
{
    info,
    warning,
    error
};

// 로그 레벨을 출력에 사용할 문자열로 변환한다.
const char *level_name(LogLevel level)
{
    switch (level)
    {
    case LogLevel::info:
        return "INFO";
    case LogLevel::warning:
        return "WARN";
    case LogLevel::error:
        return "ERROR";
    }
    return "UNKNOWN";
}

// 가변 템플릿으로 개수와 타입이 다른 인자를 받아 하나의 메시지로 조합한다.
template <typename... Args>
void log_message(LogLevel level, Args &&...args)
{
    std::ostringstream message;
    // 이진 fold expression이 모든 인자를 순서대로 스트림에 전달한다.
    (message << ... << std::forward<Args>(args));
    std::cout << '[' << level_name(level) << "] " << message.str() << '\n';
}

int main()
{
    // 문자열과 정수를 별도 변환 없이 한 번의 함수 호출에 전달할 수 있다.
    const std::string user_id = "user-1001";
    const int retry_count = 3;

    log_message(LogLevel::info, "User ", user_id, " signed in");
    log_message(LogLevel::warning, "Request timed out; retries=", retry_count);
}
