#pragma once
#include <iostream>
#include <Windows.h>
#include "Format.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"

#if defined(DEBUG) || defined(_DEBUG)
    #define LOG_ENABLED 1
#else
    #define LOG_ENABLED 0
#endif

#if LOG_ENABLED

#define LOG_PRINT(...)   ::Log::Get()->Print(__VA_ARGS__)
#define LOG_INFO(...)    ::Log::Get()->Info(__VA_ARGS__)
#define LOG_VERBOSE(...) ::Log::Get()->Verbose(__VA_ARGS__)
#define LOG_WARN(...)    ::Log::Get()->Warn(__VA_ARGS__)
#define LOG_ERROR(...)   ::Log::Get()->Error(__VA_ARGS__)
#define LOG_DEBUG(...)   ::Log::Get()->Debug(__VA_ARGS__)

#define STRINGIZE(x) #x
#define STATIC_ASSERT_MSG(msg) msg " " __FILE__ ":" STRINGIZE(__LINE__) " in function " __FUNCSIG__

#ifdef USE_STD_OUTPUT
#define LOG_OUTPUT(prefix, color, reset, msg, ...) \
    std::cout << std::format(prefix, color, reset) << std::vformat(msg, std::make_format_args(__VA_ARGS__)) << std::endl

#define LOG_WOUTPUT(prefix, color, reset, msg, ...)                                                          \
    std::wcout << std::format(prefix, color, reset) << std::vformat(msg, std::make_format_args(__VA_ARGS__)) \
               << std::endl
#else
#define LOG_OUTPUT(prefix, color, reset, msg, ...) \
    spdlog::info("[{}{}{}] {}{}", color, prefix, reset, fmt::format(fmt::runtime(msg), __VA_ARGS__), NarrowText::Reset)
#define LOG_OUTPUT_MSG(msg, ...) \
    spdlog::info("{}{}", fmt::format(fmt::runtime(msg), __VA_ARGS__), NarrowText::Reset)

#define LOG_WOUTPUT(prefix, color, reset, msg, ...) \
    spdlog::info(L"[{}{}{}] {}{}", color, prefix, reset, fmt::format(fmt::runtime(msg), __VA_ARGS__), WideText::Reset)
#define LOG_WOUTPUT_MSG(msg, ...) \
    spdlog::info(L"{}{}", fmt::format(fmt::runtime(msg), __VA_ARGS__), WideText::Reset)
#endif

// only valid in namespace scope
template <typename T>
constexpr bool IsSupportedNarrowType =
    std::is_same_v<T, int> || std::is_same_v<T, char> || std::is_same_v<T, bool> || std::is_same_v<T, int32_t> ||
    std::is_same_v<T, int64_t> || std::is_same_v<T, uintptr_t> || std::is_same_v<T, std::vector<std::string>>;
template <typename T> constexpr bool IsSupportedWideType = std::is_same_v<T, wchar_t>;

struct Log
{
    enum class Type : uint8_t
    {
        Print = 0,
        Info,
        Verbose,
        Warn,
        Error,
        Debug
    };

    struct Colors
    {
        struct Narrow
        {
            static inline const char* Print = NarrowText::Foreground::White;
            static inline const char* Info = NarrowText::Foreground::White;
            static inline const char* Verbose = NarrowText::Foreground::Gray;
            static inline const char* Warn = NarrowText::Foreground::Yellow;
            static inline const char* Error = NarrowText::Foreground::LightRed;
            static inline const char* Debug = NarrowText::Foreground::Cyan;
        };

        struct Wide
        {
            static inline const wchar_t* Info = WideText::Foreground::White;
            static inline const wchar_t* Print = WideText::Foreground::White;
            static inline const wchar_t* Verbose = WideText::Foreground::Gray;
            static inline const wchar_t* Warn = WideText::Foreground::Yellow;
            static inline const wchar_t* Error = WideText::Foreground::LightRed;
            static inline const wchar_t* Debug = WideText::Foreground::Cyan;
        };

        template <typename T> void Set(const Type& acType, const T* acColor)
        {
            static_assert(
                !std::is_same_v<T, char> && !std::is_same_v<T, wchar_t>, STATIC_ASSERT_MSG("Unsupported type"));

            if constexpr (std::is_same_v<T, char>)
            {
                switch (acType)
                {
                    case Type::Print: Narrow::Print = acColor; break;
                    case Type::Verbose: Narrow::Verbose = acColor; break;
                    case Type::Warn: Narrow::Warn = acColor; break;
                    case Type::Error: Narrow::Error = acColor; break;
                    case Type::Debug: Narrow::Debug = acColor; break;
                }
            }
            else if constexpr (std::is_same_v<T, wchar_t>)
            {
                switch (acType)
                {
                    case Type::Print: Wide::Print = acColor; break;
                    case Type::Verbose: Wide::Verbose = acColor; break;
                    case Type::Warn: Wide::Warn = acColor; break;
                    case Type::Error: Wide::Error = acColor; break;
                    case Type::Debug: Wide::Debug = acColor; break;
                }
            }
        }
    };

    static Log* Get()
    {
        static Log* s_instance = new Log();
        return s_instance;
    }

    template <typename T, typename... Args> void Print(const T* acMsg, Args&&... aArgs) noexcept
    {
        LogInternal_msg(acMsg, std::forward<Args>(aArgs)...);
    }

    template <typename T, typename... Args> void Info(const T* acMsg, Args&&... aArgs) noexcept
    {
        LogInternal("Info", acMsg, std::forward<Args>(aArgs)...);
    }

    template <typename T, typename... Args> void Verbose(const T* acMsg, Args&&... aArgs) noexcept
    {
        LogInternal("Verbose", acMsg, std::forward<Args>(aArgs)...);
    }

    template <typename T, typename... Args> void Warn(const T* acMsg, Args&&... aArgs) noexcept
    {
        LogInternal("Warn", acMsg, std::forward<Args>(aArgs)...);
    }

    template <typename T, typename... Args> void Error(const T* acMsg, Args&&... aArgs) noexcept
    {
        LogInternal("Error", acMsg, std::forward<Args>(aArgs)...);
    }

    template <typename T, typename... Args> void Debug(const T* acMsg, Args&&... aArgs) noexcept
    {
        LogInternal("Debug", acMsg, std::forward<Args>(aArgs)...);
    }

    template <typename T> void SetColor(const Type& acLogType, const T* acColor)
    {
        static_assert(!IsSupportedNarrowType<T> && !IsSupportedWideType<T>, STATIC_ASSERT_MSG("Unsupported type"));

        if constexpr (IsSupportedNarrowType<T>)
        {
            switch (acLogType)
            {
                case Type::Print: Colors::Narrow::Print = acColor; break;
                case Type::Info: Colors::Narrow::Info = acColor; break;
                case Type::Verbose: Colors::Narrow::Verbose = acColor; break;
                case Type::Warn: Colors::Narrow::Warn = acColor; break;
                case Type::Error: Colors::Narrow::Error = acColor; break;
                case Type::Debug: Colors::Narrow::Debug = acColor; break;
            }
        }
        else if constexpr (IsSupportedWideType<T>)
        {
            switch (acLogType)
            {
                case Type::Print: Colors::Wide::Print = acColor; break;
                case Type::Info: Colors::Wide::Info = acColor; break;
                case Type::Verbose: Colors::Wide::Verbose = acColor; break;
                case Type::Warn: Colors::Wide::Warn = acColor; break;
                case Type::Error: Colors::Wide::Error = acColor; break;
                case Type::Debug: Colors::Wide::Debug = acColor; break;
            }
        }
    }

    ~Log() = default;
    Log(const Log&) = delete;
    Log(Log&&) = delete;
    Log& operator=(const Log&) = delete;
    Log& operator=(Log&&) = delete;

    HWND ConsoleHandle = nullptr;
    HANDLE ThreadHandle = nullptr;

private:
    Log()
    {
        if (AllocConsole())
        {
            SetConsoleTitleA("Debug Console");
            SetConsoleOutputCP(CP_UTF8);
            SetConsoleCP(CP_UTF8);

            ConsoleHandle = GetConsoleWindow();
            ThreadHandle = GetCurrentThread();

            FILE* stdinFile;
            FILE* stdoutFile;
            FILE* stderrFile;

            if (freopen_s(&stdinFile, "conin$", "r", stdin) != 0) std::cerr << "Error redirecting stdin\n";

            if (freopen_s(&stdoutFile, "conout$", "w", stdout) != 0) std::cerr << "Error redirecting stdout\n";

            if (freopen_s(&stderrFile, "conout$", "w", stderr) != 0) std::cerr << "Error redirecting stderr\n";

            HANDLE stdHandle = GetStdHandle(STD_OUTPUT_HANDLE);

            if (stdHandle == INVALID_HANDLE_VALUE)
            {
                std::cerr << "Error: unable to get handle to stdout.\n";
                return;
            }

            DWORD dwMode = 0;

            GetConsoleMode(stdHandle, &dwMode);
            dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            SetConsoleMode(stdHandle, dwMode);
            if (!SetConsoleMode(stdHandle, dwMode)) std::cerr << "Error: unable to set console mode.\n";

            CONSOLE_CURSOR_INFO cursorInfo;
            GetConsoleCursorInfo(stdHandle, &cursorInfo);
            cursorInfo.dwSize = 1;
            cursorInfo.bVisible = FALSE;
            SetConsoleCursorInfo(stdHandle, &cursorInfo);
            SetConsoleMode(stdHandle, ENABLE_EXTENDED_FLAGS);

            auto sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            sink->set_level(spdlog::level::debug);
            sink->set_pattern("[%H:%M:%S.%e] %v");

            const auto cLogger = std::make_shared<spdlog::logger>("", spdlog::sinks_init_list{sink});
            cLogger->set_level(spdlog::level::debug);

            set_default_logger(cLogger);
        }
    }

    template <typename T, typename... Args> void LogInternal_msg(const T* acMsg, Args&&... aArgs)
    {
        if constexpr (IsSupportedNarrowType<T>)
        {
            LOG_OUTPUT_MSG(acMsg, std::forward<Args>(aArgs)...);
        }
        else if constexpr (IsSupportedWideType<T>)
        {
            LOG_WOUTPUT_MSG(acMsg, std::forward<Args>(aArgs)...);
        }
    }
    template <typename T, typename... Args> void LogInternal(const char* acLabel, const T* acMsg, Args&&... aArgs)
    {
        if constexpr (IsSupportedNarrowType<T>)
        {
            LOG_OUTPUT(acLabel, Colors::Narrow::Print, NarrowText::Reset, acMsg, std::forward<Args>(aArgs)...);
        }
        else if constexpr (IsSupportedWideType<T>)
        {
            LOG_WOUTPUT(
                std::wstring(acLabel, acLabel + strlen(acLabel)).c_str(), Colors::Wide::Print, WideText::Reset, acMsg,
                std::forward<Args>(aArgs)...);
        }
    }
};

#else

#define LOG_PRINT(...)   ((void)0)
#define LOG_INFO(...)    ((void)0)
#define LOG_VERBOSE(...) ((void)0)
#define LOG_WARN(...)    ((void)0)
#define LOG_ERROR(...)   ((void)0)
#define LOG_DEBUG(...)   ((void)0)

struct Log;

#endif