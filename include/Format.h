#pragma once

template <typename T> struct Text;

using NarrowText = Text<char>;
using WideText = Text<wchar_t>;

template <> struct Text<char>
{
    static constexpr const char* Reset = "\x1B[0m";
    static constexpr const char* Bold = "\x1B[1m";
    static constexpr const char* Italics = "\x1B[3m";
    static constexpr const char* Underline = "\x1B[4m";
    static constexpr const char* Blink = "\x1B[5m";
    static constexpr const char* Reverse = "\x1B[7m";
    static constexpr const char* Hidden = "\x1B[8m";
    static constexpr const char* Strikethrough = "\x1B[9m";
    static constexpr const char* ShowCursor = "\x1B[?25h";
    static constexpr const char* HideCursor = "\x1B[?25l";

    struct Foreground
    {
        static constexpr const char* Black = "\x1B[30m";
        static constexpr const char* Red = "\x1B[31m";
        static constexpr const char* Green = "\x1B[32m";
        static constexpr const char* Gold = "\x1B[33m";
        static constexpr const char* Blue = "\x1B[34m";
        static constexpr const char* Magenta = "\x1B[35m";
        static constexpr const char* Cyan = "\x1B[36m";
        static constexpr const char* LightGray = "\x1B[37m";
        static constexpr const char* LightBlack = "\x1B[90m";
        static constexpr const char* LightRed = "\x1B[91m";
        static constexpr const char* LightGreen = "\x1B[92m";
        static constexpr const char* LightYellow = "\x1B[93m";
        static constexpr const char* LightBlue = "\x1B[94m";
        static constexpr const char* Pink = "\x1B[95m";
        static constexpr const char* LightCyan = "\x1B[96m";
        static constexpr const char* White = "\x1B[97m";
        static constexpr const char* Gray = "\x1B[38;5;8m";
        static constexpr const char* Yellow = "\x1B[38;5;220m";
        static constexpr const char* Purple = "\x1B[128;80;254m";
    };

    struct Background
    {
        static constexpr const char* Black = "\x1B[40m";
        static constexpr const char* Red = "\x1B[41m";
        static constexpr const char* Green = "\x1B[42m";
        static constexpr const char* Gold = "\x1B[43m";
        static constexpr const char* Blue = "\x1B[44m";
        static constexpr const char* Magenta = "\x1B[45m";
        static constexpr const char* Cyan = "\x1B[46m";
        static constexpr const char* Gray = "\x1B[47m";
        static constexpr const char* LightBlack = "\x1B[100m";
        static constexpr const char* LightRed = "\x1B[101m";
        static constexpr const char* LightGreen = "\x1B[102m";
        static constexpr const char* Yellow = "\x1B[103m";
        static constexpr const char* LightBlue = "\x1B[104m";
        static constexpr const char* Pink = "\x1B[105m";
        static constexpr const char* LightCyan = "\x1B[106m";
        static constexpr const char* White = "\x1B[107m";
    };
};

template <> struct Text<wchar_t>
{
    static constexpr const wchar_t* Reset = L"\x1B[0m";
    static constexpr const wchar_t* Bold = L"\x1B[1m";
    static constexpr const wchar_t* Italics = L"\x1B[3m";
    static constexpr const wchar_t* Underline = L"\x1B[4m";
    static constexpr const wchar_t* Blink = L"\x1B[5m";
    static constexpr const wchar_t* Reverse = L"\x1B[7m";
    static constexpr const wchar_t* Hidden = L"\x1B[8m";
    static constexpr const wchar_t* Strikethrough = L"\x1B[9m";
    static constexpr const wchar_t* ShowCursor = L"\x1B[?25h";
    static constexpr const wchar_t* HideCursor = L"\x1B[?25l";

    struct Foreground
    {
        static constexpr const wchar_t* Black = L"\x1B[30m";
        static constexpr const wchar_t* Red = L"\x1B[31m";
        static constexpr const wchar_t* Green = L"\x1B[32m";
        static constexpr const wchar_t* Gold = L"\x1B[33m";
        static constexpr const wchar_t* Blue = L"\x1B[34m";
        static constexpr const wchar_t* Magenta = L"\x1B[35m";
        static constexpr const wchar_t* Cyan = L"\x1B[36m";
        static constexpr const wchar_t* LightGray = L"\x1B[37m";
        static constexpr const wchar_t* LightBlack = L"\x1B[90m";
        static constexpr const wchar_t* LightRed = L"\x1B[91m";
        static constexpr const wchar_t* LightGreen = L"\x1B[92m";
        static constexpr const wchar_t* LightYellow = L"\x1B[93m";
        static constexpr const wchar_t* LightBlue = L"\x1B[94m";
        static constexpr const wchar_t* Pink = L"\x1B[95m";
        static constexpr const wchar_t* LightCyan = L"\x1B[96m";
        static constexpr const wchar_t* White = L"\x1B[97m";
        static constexpr const wchar_t* Gray = L"\x1B[38;5;8m";
        static constexpr const wchar_t* Yellow = L"\x1B[38;5;220m";
        static constexpr const wchar_t* Purple = L"\x1B[128;80;254m";
    };

    struct Background
    {
        static constexpr const wchar_t* Black = L"\x1B[40m";
        static constexpr const wchar_t* Red = L"\x1B[41m";
        static constexpr const wchar_t* Green = L"\x1B[42m";
        static constexpr const wchar_t* Gold = L"\x1B[43m";
        static constexpr const wchar_t* Blue = L"\x1B[44m";
        static constexpr const wchar_t* Magenta = L"\x1B[45m";
        static constexpr const wchar_t* Cyan = L"\x1B[46m";
        static constexpr const wchar_t* Gray = L"\x1B[47m";
        static constexpr const wchar_t* LightBlack = L"\x1B[100m";
        static constexpr const wchar_t* LightRed = L"\x1B[101m";
        static constexpr const wchar_t* LightGreen = L"\x1B[102m";
        static constexpr const wchar_t* Yellow = L"\x1B[103m";
        static constexpr const wchar_t* LightBlue = L"\x1B[104m";
        static constexpr const wchar_t* Pink = L"\x1B[105m";
        static constexpr const wchar_t* LightCyan = L"\x1B[106m";
        static constexpr const wchar_t* White = L"\x1B[107m";
    };
};