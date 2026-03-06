#pragma once
#include <Windows.h>
#include <string>
#include <type_traits> // remove and compilation perishes

struct IEventBase
{
    IEventBase() = default;
    explicit IEventBase(const std::string& acName)
        : m_name(R"(\\.\pipe\)" + acName)
    {
    }
    IEventBase(const IEventBase&) = default;
    IEventBase(IEventBase&&) = default;
    IEventBase& operator=(const IEventBase&) = default;
    IEventBase& operator=(IEventBase&&) = default;
    virtual ~IEventBase()
    {
        if ((m_pipe != INVALID_HANDLE_VALUE && m_pipe != nullptr) && m_connected)
        {
            m_connected = false;
            CloseHandle(m_pipe);
            m_pipe = INVALID_HANDLE_VALUE;
        }
    }

    [[nodiscard]] virtual const std::type_info& GetDataType() const = 0;
    [[nodiscard]] virtual const void* GetData() const = 0;

    bool WriteBuffer(const void* apData, size_t aSize)
    {
        DWORD written = 0;
        return WriteFile(m_pipe, apData, (DWORD)aSize, &written, nullptr);
    }

    bool ReadBuffer(void* apData, size_t aSize)
    {
        DWORD read = 0;
        return ReadFile(m_pipe, apData, (DWORD)aSize, &read, nullptr);
    }

    [[nodiscard]] const std::string& GetName() const { return m_name; }

protected:
    HANDLE m_pipe = INVALID_HANDLE_VALUE;
    bool m_connected = false;
    std::string m_name{};
};