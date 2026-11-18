#include <iostream>
#include <sstream>
#include <string>
#include <utility>

// enum class는 열거자 이름을 범위 안에 두는 강한 타입의 열거형을 선언한다.
enum class LogLevel
{
    info,
    warning,
    error
};

// const char*에서 const는 가리키는 문자를, *는 포인터 타입을 나타낸다.
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

// typename... Args는 타입 매개변수 팩, Args&&...는 함수 매개변수 팩이다.
template <typename... Args>
void log_message(LogLevel level, Args &&...args)
{
    std::ostringstream message;
    // std::forward는 전달 참조의 값 범주를 보존하고, ...는 <<에 대한 이항 fold expression을 만든다.
    (message << ... << std::forward<Args>(args));
    std::cout << '[' << level_name(level) << "] " << message.str() << '\n';
}

int main()
{
    // const는 객체 타입에 한정자를 붙여 이 이름을 통한 값 변경을 금지한다.
    const std::string user_id = "user-1001";
    const int retry_count = 3;

    log_message(LogLevel::info, "User ", user_id, " signed in");
    log_message(LogLevel::warning, "Request timed out; retries=", retry_count);
}
