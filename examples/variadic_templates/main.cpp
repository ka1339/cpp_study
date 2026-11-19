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
// 이 함수는 enum의 모든 열거자를 처리하고, 마지막 반환은 예상 밖의 값에 대한 안전망이다.
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
// 인자 개수와 형식이 달라도 각 값이 ostream에 출력 가능하면 사용할 수 있다.
template <typename... Args>
void log_message(LogLevel level, Args &&...args)
{
    std::ostringstream message;
    // std::forward는 전달 참조의 값 범주를 보존하고, fold expression은 각 인자를 순서대로 <<에 전달한다.
    // 이진 fold는 인자가 0개여도 초기값인 message가 남으므로 빈 인자 목록도 허용한다.
    (message << ... << std::forward<Args>(args));
    std::cout << '[' << level_name(level) << "] " << message.str() << '\n';
}

int main()
{
    // const는 객체 타입에 한정자를 붙여 이 이름을 통한 값 변경을 금지한다.
    const std::string user_id = "user-1001";
    const int retry_count = 3;

    // 문자열 리터럴, std::string, 정수 등 서로 다른 타입의 인자를 한 번에 전달한다.
    log_message(LogLevel::info, "User ", user_id, " signed in");
    log_message(LogLevel::warning, "Request timed out; retries=", retry_count);
}
