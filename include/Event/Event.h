#pragma once
#include <windows.h>
#include "IEventBase.h"

template <typename Type> struct Event : IEventBase
{
    using DataType = Type;

    using IEventBase::IEventBase;
    explicit Event(const char* acName = "")
        : IEventBase(acName)
    {
    }
    explicit Event(DataType acData, const char* acName = "")
        : IEventBase(acName)
        , m_data(std::move(acData))
    {
    }
    Event()
        : IEventBase()
    {
    }
    Event(const Event&) = default;
    Event(Event&&) = default;
    ~Event() override { IEventBase::~IEventBase(); };
    Event& operator=(const Event&) = default;
    Event& operator=(Event&&) = default;

    [[nodiscard]] HANDLE GetPipe() const { return m_pipe; }
    [[nodiscard]] bool IsValid() const { return m_pipe != INVALID_HANDLE_VALUE; }
    [[nodiscard]] const std::type_info& GetDataType() const override { return typeid(DataType); }
    [[nodiscard]] static size_t GetDataSize() { return sizeof(DataType); }
    [[nodiscard]] const void* GetData() const override { return &m_data; }
    [[nodiscard]] DataType& GetTypedData() { return m_data; }
    [[nodiscard]] const DataType& GetTypedData() const { return m_data; }
    void SetData(const DataType& acData) { m_data = acData; }

private:
    DataType m_data{};
    size_t m_length = sizeof(DataType);
};
