#pragma once
#include <iostream>
#include <Windows.h>
#include "Format.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"

#ifdef USE_STD_OUTPUT
#define LOG_OUTPUT(prefix, color, reset, msg, ...) \
    std::cout << std::format(prefix, color, reset) << std::vformat(msg, std::make_format_args(__VA_ARGS__)) << std::endl

#define LOG_WOUTPUT(prefix, color, reset, msg, ...) \
    std::wcout << std::format(prefix, color, reset) << std::vformat(msg, std::make_format_args(__VA_ARGS__)) << std::endl
#else
#define LOG_OUTPUT(prefix, color, reset, msg, ...) spdlog::info("[{}{}{}] {}", color, prefix, reset, std::vformat(msg, std::make_format_args(__VA_ARGS__)))

#define LOG_WOUTPUT(prefix, color, reset, msg, ...) spdlog::info(L"[{}{}{}] {}", color, prefix, reset, std::vformat(msg, std::make_wformat_args(__VA_ARGS__)))
#endif

struct Log
{
    enum class Type : uint8_t
    {
        Print = 0,
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
            static inline const char* Verbose = NarrowText::Foreground::Gray;
            static inline const char* Warn = NarrowText::Foreground::Yellow;
            static inline const char* Error = NarrowText::Foreground::LightRed;
            static inline const char* Debug = NarrowText::Foreground::Cyan;
        };

        struct Wide
        {
            static inline const wchar_t* Print = WideText::Foreground::White;
            static inline const wchar_t* Verbose = WideText::Foreground::Gray;
            static inline const wchar_t* Warn = WideText::Foreground::Yellow;
            static inline const wchar_t* Error = WideText::Foreground::LightRed;
            static inline const wchar_t* Debug = WideText::Foreground::Cyan;
        };

        template<typename T>
        void Set(const Type& acType, const T* acColor)
        {
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
            else
            {
                static_assert(false, "Incorrect type given to Colors::Set");
            }
        }
    };
    
    static Log& Get()
    {
        static Log s_instance;

        return s_instance;
    }
    
    template <typename T, typename... Args> void Print(const T* acMsg, Args&&... aArgs) noexcept
    {
        if constexpr (std::is_same_v<T, char>)
        {
            LOG_OUTPUT("Log", Colors::Narrow::Print, Text<T>::Reset, acMsg, std::forward<Args>(aArgs)...);
        }
        else if constexpr (std::is_same_v<T, wchar_t>)
        {
            LOG_WOUTPUT(L"Log", Colors::Wide::Print, Text<T>::Reset, acMsg, std::forward<Args>(aArgs)...);
        }
    }

    template <typename T, typename... Args> void Verbose(const T* acMsg, Args&&... aArgs) noexcept
    {
        if constexpr (std::is_same_v<T, char>)
        {
            LOG_OUTPUT("Verbose", Colors::Narrow::Verbose, Text<T>::Reset, acMsg, std::forward<Args>(aArgs)...);
        }
        else if constexpr (std::is_same_v<T, wchar_t>)
        {
            LOG_WOUTPUT(L"Verbose", Colors::Wide::Verbose, Text<T>::Reset, acMsg, std::forward<Args>(aArgs)...);
        }
    }

    template <typename T, typename... Args> void Warn(const T* acMsg, Args&&... aArgs) noexcept
    {
        if constexpr (std::is_same_v<T, char>)
        {
            LOG_OUTPUT("Warn", Colors::Narrow::Warn, Text<T>::Reset, acMsg, std::forward<Args>(aArgs)...);
        }
        else if constexpr (std::is_same_v<T, wchar_t>)
        {
            LOG_WOUTPUT(L"Warn", Colors::Wide::Warn, Text<T>::Reset, acMsg, std::forward<Args>(aArgs)...);
        }
    }

    template <typename T, typename... Args> void Error(const T* acMsg, Args&&... aArgs) noexcept
    {
        if constexpr (std::is_same_v<T, char>)
        {
            LOG_OUTPUT("Error", Colors::Narrow::Error, Text<T>::Reset, acMsg, std::forward<Args>(aArgs)...);
        }
        else if constexpr (std::is_same_v<T, wchar_t>)
        {
            LOG_WOUTPUT(L"Error", Colors::Wide::Error, Text<T>::Reset, acMsg, std::forward<Args>(aArgs)...);
        }
    }

    template <typename T, typename... Args> void Debug(const T* acMsg, Args&&... aArgs) noexcept
    {
        if constexpr (std::is_same_v<T, char>)
        {
            LOG_OUTPUT("Debug", Colors::Narrow::Debug, Text<T>::Reset, acMsg, std::forward<Args>(aArgs)...);
        }
        else if constexpr (std::is_same_v<T, wchar_t>)
        {
            LOG_WOUTPUT(L"Debug", Colors::Wide::Debug, Text<T>::Reset, acMsg, std::forward<Args>(aArgs)...);
        }
    }

    template <typename T> void SetColor(const Type& acLogType, const T* acColor)
    {
        if constexpr (std::is_same_v<T, char>)
        {
            switch (acLogType)
            {
            case Type::Print: Colors::Narrow::Print = acColor; break;
            case Type::Verbose: Colors::Narrow::Verbose = acColor; break;
            case Type::Warn: Colors::Narrow::Warn = acColor; break;
            case Type::Error: Colors::Narrow::Error = acColor; break;
            case Type::Debug: Colors::Narrow::Debug = acColor; break;
            }
        }
        else if constexpr (std::is_same_v<T, wchar_t>)
        {
            switch (acLogType)
            {
            case Type::Print: Colors::Wide::Print = acColor; break;
            case Type::Verbose: Colors::Wide::Verbose = acColor; break;
            case Type::Warn: Colors::Wide::Warn = acColor; break;
            case Type::Error: Colors::Wide::Error = acColor; break;
            case Type::Debug: Colors::Wide::Debug = acColor; break;
            }
        }
        else
        {
            static_assert(false, "Incorrect type given to Colors::Set");
        }
    }

    ~Log() = default;
    Log(const Log&) = delete;
    Log(Log&&) = delete;
    Log& operator=(const Log&) = delete;
    Log& operator=(Log&&) = delete;

    HWND ConsoleHandle;
    HANDLE ThreadHandle;

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

            if (freopen_s(&stdinFile, "conin$", "r", stdin) != 0)
                std::cerr << "Error redirecting stdin\n";

            if (freopen_s(&stdoutFile, "conout$", "w", stdout) != 0)
                std::cerr << "Error redirecting stdout\n";

            if (freopen_s(&stderrFile, "conout$", "w", stderr) != 0)
                std::cerr << "Error redirecting stderr\n";

            // console output handling
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
            if (!SetConsoleMode(stdHandle, dwMode))
                std::cerr << "Error: unable to set console mode.\n";

            CONSOLE_CURSOR_INFO cursorInfo;
            GetConsoleCursorInfo(stdHandle, &cursorInfo);
            cursorInfo.dwSize = 1;
            cursorInfo.bVisible = FALSE;
            SetConsoleCursorInfo(stdHandle, &cursorInfo);

            stdHandle = GetStdHandle(STD_INPUT_HANDLE);

            if (stdHandle == INVALID_HANDLE_VALUE)
            {
                std::cerr << "Error: unable to get handle to stdout.\n";
                return;
            }

            SetConsoleMode(stdHandle, ENABLE_EXTENDED_FLAGS);
            
            // logger
            auto narrow = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            narrow->set_level(spdlog::level::debug);
            narrow->set_pattern("[%H:%M:%S.%e] %v");

            auto narrowLogger = std::make_shared<spdlog::logger>("", spdlog::sinks_init_list{narrow});
            narrowLogger->set_level(spdlog::level::debug);

            spdlog::set_default_logger(narrowLogger);
        }
    }
};
