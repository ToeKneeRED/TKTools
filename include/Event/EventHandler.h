#pragma once
#include <typeindex>
#include <unordered_map>
#include <functional>
#include <thread>
#include <atomic>
#include <memory>

#include "IEventBase.h"
#include "Event.h"
#include "Log.h"

#pragma pack(push, 1)
struct EventHeader
{
    size_t TypeHash;
    size_t DataSize;
};
#pragma pack(pop)

struct EventPipe
{
    EventPipe() = default;
    explicit EventPipe(const std::string& acName)
        : Name(acName)
    {
    }

    std::string Name{""};
    HANDLE Handle{nullptr};
    std::thread Thread{};
    std::atomic<bool> Connected{false};
};

struct EventHandler
{
    // funky stuff to extract argument type from lambda/function
    template <typename F> struct FuncArg;
    template <typename R, typename C, typename Arg> struct FuncArg<R (C::*)(Arg) const>
    {
        using Type = Arg;
    };
    template <typename F> using FuncArgType = typename FuncArg<decltype(&F::operator())>::Type;

    explicit EventHandler(
        const std::string& acWritePipeName = "", const std::string& acReadPipeName = "", const bool acFirst = false)
    {
        WritePipe = std::make_unique<EventPipe>(R"(\\.\pipe\)" + acWritePipeName);
        ReadPipe = std::make_unique<EventPipe>(R"(\\.\pipe\)" + acReadPipeName);

        acFirst ? CreateFirst() : CreateSecond();
    }
    EventHandler(const EventHandler&) = delete;
    EventHandler(EventHandler&&) = delete;
    EventHandler& operator=(const EventHandler&) = delete;
    EventHandler& operator=(EventHandler&&) = delete;
    ~EventHandler() { Stop(); }

    void CreateFirst() const
    {
        WritePipe->Handle = CreateNamedPipeA(
            WritePipe->Name.c_str(), PIPE_ACCESS_OUTBOUND, PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT, 1,
            1024, 1024, 0, nullptr);
        ConnectNamedPipe(WritePipe->Handle, nullptr);
        WritePipe->Connected = true;

        WaitNamedPipeA(ReadPipe->Name.c_str(), NMPWAIT_WAIT_FOREVER);
        ReadPipe->Handle = CreateFileA(ReadPipe->Name.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, 0, nullptr);
        ReadPipe->Connected = true;
    }

    void CreateSecond() const
    {
        ReadPipe->Handle = CreateFileA(ReadPipe->Name.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, 0, nullptr);
        ReadPipe->Connected = true;

        WritePipe->Handle = CreateNamedPipeA(
            WritePipe->Name.c_str(), PIPE_ACCESS_OUTBOUND, PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT, 1,
            1024, 1024, 0, nullptr);
        ConnectNamedPipe(WritePipe->Handle, nullptr);
        WritePipe->Connected = true;
    }

    void Start()
    {
        if (!WritePipe->Connected)
        {
            std::thread(
                [&]()
                {
                    if (!ConnectNamedPipe(WritePipe->Handle, nullptr))
                    {
                        if (const DWORD cErr = GetLastError(); cErr == ERROR_PIPE_CONNECTED)
                        {
                            WritePipe->Connected = true;
                        }
                    }
                    else { WritePipe->Connected = true; }
                })
                .detach();
        }

        if (!ReadPipe->Connected)
        {
            std::thread(
                [&]()
                {
                    if (!ConnectNamedPipe(ReadPipe->Handle, nullptr))
                    {
                        if (const DWORD cErr = GetLastError(); cErr == ERROR_PIPE_CONNECTED)
                        {
                            ReadPipe->Connected = true;
                        }
                    }
                    else { ReadPipe->Connected = true; }
                })
                .detach();
        }

        m_started = true;
    }

    bool Run()
    {
        if (!m_started) { Start(); }

        ReadPipe->Thread = std::thread(
            [&]()
            {
                while (ReadPipe->Connected)
                {
                    EventHeader header{};
                    DWORD read = 0;

                    if (!ReadFile(ReadPipe->Handle, &header, sizeof(EventHeader), &read, nullptr))
                    {
                        if (m_retryCount > MaxRetryAttempts)
                        {
                            LOG_WARN("Max retry attempts reached, aborting...");

                            Stop();
                            return false;
                        }

                        m_retryCount++;
                        LOG_WARN("Retry {}", m_retryCount);

                        std::this_thread::sleep_for(std::chrono::milliseconds(SleepDuration));
                        continue;
                    }

                    std::vector<uint8_t> buffer(header.DataSize);

                    if (!ReadFile(ReadPipe->Handle, buffer.data(), header.DataSize, &read, nullptr))
                    {
                        LOG_WARN("Failed to read data");
                        continue;
                    }

                    if (auto it = m_callbacks.find(header.TypeHash); it != m_callbacks.end())
                    {
                        auto callback = it->second;
                        auto buf = buffer;

                        std::thread([callback, data = std::move(buf)]() { callback(data.data(), data.size()); })
                            .detach();
                    }
                    else { LOG_WARN("No callbacks for event type {}", header.TypeHash); }

                    std::this_thread::sleep_for(std::chrono::milliseconds(SleepDuration));
                }

                return true;
            });
        ReadPipe->Thread.detach();

        Running = true;

        return true;
    }

    void Stop()
    {
        Running = false;
        m_started = false;
    }

    template <typename T = Event> void AddCallback(std::function<void(const typename T::DataType&)> aCallback)
    {
        const size_t cHash = std::hash<std::string>{}(typeid(T).name());

        m_callbacks[cHash] = [aCallback](const void* apData, size_t acSize)
        {
            typename T::DataType data;
            memcpy(&data, apData, sizeof(T::DataType));
            aCallback(data);
        };
    }

    template <typename T = IEventBase> void Send(const T& acEvent) const
    {
        const size_t cTypeHash = std::hash<std::string>{}(typeid(T).name());
        const void* pData;
        size_t dataSize;

        if constexpr (std::is_base_of_v<IEventBase, T>)
        {
            pData = acEvent.GetData();
            dataSize = acEvent.GetDataSize();
        }
        else
        {
            pData = &acEvent;
            dataSize = sizeof(T);
        }

        if (!pData || !dataSize) return;

        const EventHeader cHeader{.TypeHash = cTypeHash, .DataSize = static_cast<uint32_t>(dataSize)};
        DWORD written = 0;

        if (!WritePipe)
        {
            LOG_ERROR("Attempting to send without pipe connected");
            return;
        }

        WriteFile(WritePipe->Handle, &cHeader, sizeof(EventHeader), &written, nullptr);
        WriteFile(WritePipe->Handle, pData, static_cast<DWORD>(dataSize), &written, nullptr);
    }

    uint32_t SleepDuration = 250;
    uint16_t MaxRetryAttempts = 3;
    std::atomic<bool> Running{false};
    std::unique_ptr<EventPipe> WritePipe{};
    std::unique_ptr<EventPipe> ReadPipe{};

private:
    std::unordered_map<size_t, std::function<void(const void* acpData, size_t aDataSize)>> m_callbacks{};
    uint16_t m_retryCount{0};
    std::atomic<bool> m_started{false};
};