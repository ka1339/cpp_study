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

template <typename... Args>
void log_message(LogLevel level, Args &&...args)
{
    std::ostringstream message;
    (message << ... << std::forward<Args>(args));
    std::cout << '[' << level_name(level) << "] " << message.str() << '\n';
}

int main()
{
    const std::string user_id = "user-1001";
    const int retry_count = 3;

    log_message(LogLevel::info, "User ", user_id, " signed in");
    log_message(LogLevel::warning, "Request timed out; retries=", retry_count);
}
