#pragma once
#include <windows.h>
#include <string>
#include <thread>
#include <functional>
#include "Log.h"

enum class EventType : uint8_t
{
    None = 0,
    Connected,
    Disconnected,
    Close,
    Data
};

enum class IPC : uint8_t
{
    None = 0,
    Server,
    Client,
};

template<typename DataType>
struct Event
{
    explicit Event(const IPC& acIpc, const std::string& acName = "")
        : m_name(R"(\\.\pipe\)" + acName), m_ipc(acIpc)
    {
        switch (m_ipc)
        {
        case IPC::Server: Create(); break;
        case IPC::Client: Open(); break;
        case IPC::None: break;
        }
    }
    Event(const Event&) = default;
    Event(Event&&) = default;
    ~Event()
    {
        if (m_pipe != INVALID_HANDLE_VALUE)
        {
            CloseHandle(m_pipe);
            m_pipe = INVALID_HANDLE_VALUE;
        }
    }
    Event& operator=(const Event&) = default;
    Event& operator=(Event&&) = default;

    bool Send(DataType acData)
    {
        if (!m_connected)
        {
            Log::Get()->Warn("Attempted to send data while not connected");
            return false;
        }

        DWORD written = 0;
        return WriteFile(m_pipe, &acData, sizeof(DataType), &written, nullptr);
    }

    bool Receive(DataType& acBuffer)
    {
        if (!m_connected)
        {
            Log::Get()->Warn("Attempted to receive data while not connected");
            return false;
        }

        DWORD read = 0;
        return ReadFile(m_pipe, &acBuffer, sizeof(DataType), &read, nullptr);
    }

    [[nodiscard]] HANDLE GetHandle() const { return m_pipe; }
    [[nodiscard]] bool IsValid() const { return m_pipe != INVALID_HANDLE_VALUE; }

private:
    bool Create()
    {

        m_pipe = CreateNamedPipeA(
            m_name.c_str(), PIPE_ACCESS_DUPLEX,
            PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
            1, sizeof(DataType), sizeof(DataType), NMPWAIT_WAIT_FOREVER,
            nullptr);

        if (m_pipe == INVALID_HANDLE_VALUE)
        {
            char buf[256];
            FormatMessageA(
                FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, GetLastError(),
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buf, (sizeof(buf) / sizeof(char)), nullptr);
            Log::Get()->Error("Failed to create named pipe {} {}", m_name.c_str(), buf);
            return false;
        }

        if (!ConnectNamedPipe(m_pipe, nullptr))
        {
            if (const DWORD cErr = GetLastError(); cErr == ERROR_PIPE_CONNECTED)
            {
                Log::Get()->Print("Client connected early");
            }
        }
        else
        {
            m_connected = true;
            Log::Get()->Print("Server connected to pipe");
        }

        return true;
    }

    bool Open()
    {
        Log::Get()->Print("Attempting to connect to pipe...");

        while (true)
        {
            m_pipe = CreateFileA(
                m_name.c_str(),
                GENERIC_READ | GENERIC_WRITE,
                0, nullptr, OPEN_EXISTING, 0, nullptr);

            if (m_pipe != INVALID_HANDLE_VALUE)
                break;

            if (const DWORD cErr = GetLastError(); cErr != ERROR_PIPE_BUSY)
            {
                if (cErr == ERROR_FILE_NOT_FOUND)
                {
                    while (!WaitNamedPipeA(m_name.c_str(), NMPWAIT_WAIT_FOREVER))
                    {
                        if (GetLastError() != ERROR_FILE_NOT_FOUND)
                        {
                            char buf[256];
                            FormatMessageA(
                                FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, GetLastError(),
                                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buf, (sizeof(buf) / sizeof(char)), nullptr);
                            Log::Get()->Error("WaitNamedPipe failed {}", buf);
                            return false;
                        }
                    }
                }
                else
                {
                    char buf[256];
                    FormatMessageA(
                        FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, GetLastError(),
                        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buf, (sizeof(buf) / sizeof(char)), nullptr);
                    Log::Get()->Error("CreateFile failed {}", buf);
                return false;
                }
            }

        }

        DWORD mode = PIPE_READMODE_MESSAGE;
        SetNamedPipeHandleState(m_pipe, &mode, nullptr, nullptr);

        m_connected = true;
        Log::Get()->Print("Connected to pipe");
        return true;
    }

    HANDLE m_pipe = INVALID_HANDLE_VALUE;
    bool m_connected = false;
    std::string m_name{};
    IPC m_ipc{};
};

struct Custom
{
    char Name[32]{};
    bool WasSent = false;
};

struct ExampleEvent : Event<Custom>
{
    ExampleEvent(const IPC& acIpc, const std::string& acName = ""): Event(acIpc, acName)
    {
    }

    int Length{};
    std::string Data{};
};

template<typename DataType, typename Event>
class EventHandler
{
public:
    EventHandler(const EventHandler&) = delete;
    EventHandler(EventHandler&&) = delete;
    EventHandler& operator=(const EventHandler&) = delete;
    EventHandler& operator=(EventHandler&&) = delete;

    explicit EventHandler(const std::string& acPipeName) : m_event(IPC::Server, acPipeName) {}

    ~EventHandler() { Stop(); }

    void Listen(std::function<void(const DataType&)> aHandler)
    {
        if (!m_event.IsValid())
        {
            Log::Get()->Error("Invalid listening pipe");
            return;
        }

        m_listenThread = std::thread([this, handler = std::move(aHandler)]
        {
            DataType buffer{};

            while (m_running)
            {
                if (m_event.Receive(buffer))
                {
                    handler(buffer);
                }
                else
                {
                    Log::Get()->Warn("Pipe read failed");
                    break;
                }
            }
        });
    }

    void Stop()
    {
        m_running = false;
        if (m_listenThread.joinable())
            m_listenThread.join();
    }

private:
    Event m_event;
    std::thread m_listenThread;
    std::atomic<bool> m_running = true;
};