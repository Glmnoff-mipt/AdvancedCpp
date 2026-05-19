#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <unordered_map>

enum LogLevel 
{
    LOG_NORMAL,
    LOG_WARNING,
    LOG_ERROR
};

struct LogEvent 
{
    std::string time;
    LogLevel level;
    std::string text;
};

class Log 
{
private:
    std::vector<LogEvent> events_;

    Log() {}

    std::string getCurrentTime() const 
    {
        auto now = std::chrono::system_clock::now();
        std::time_t now_time = std::chrono::system_clock::to_time_t(now);
        char buffer[30];

        std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", std::localtime(&now_time));
        return std::string(buffer);
    }

    std::string levelToString(LogLevel level) const 
    {
        static const std::unordered_map<LogLevel, std::string> names = 
        {
            {LOG_NORMAL,  "NORMAL"},
            {LOG_WARNING, "WARNING"},
            {LOG_ERROR,   "ERROR"}
        };
        auto it = names.find(level);
        return (it != names.end()) ? it->second : "UNKNOWN";
    }

public:
    Log(const Log&) = delete;
    Log& operator=(const Log&) = delete;
    Log(Log&&) = delete;
    Log& operator=(Log&&) = delete;

    static Log* Instance() 
    {
        static Log instance; 
        return &instance;
    }

    void message(LogLevel level, const std::string& msg) 
    {
        events_.push_back({getCurrentTime(), level, msg});

        if (events_.size() > 10) { events_.erase(events_.begin()); }
    }

    void print() const 
    {
        std::cout << "   LOG OUTPUT (last " << events_.size() << " events)\n";
        for (const auto& ev : events_) 
        {
            std::cout << "[" << ev.time << "] [" 
                      << levelToString(ev.level) << "]: " 
                      << ev.text << "\n";
        }
    }
};

int main() 
{
    Log *log = Log::Instance();
    
    log->message(LOG_NORMAL, "program loaded");
    log->message(LOG_WARNING, "memory usage is high");
    log->message(LOG_NORMAL, "processing data");
    log->message(LOG_ERROR, "error happens! help me!");
    
    log->print();

    return 0;
}