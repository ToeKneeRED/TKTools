#pragma once
#include <functional>

template <typename Ret, typename... Args> struct HandlerImpl
{
    using FuncType_t = std::function<Ret(Args...)>;

    void operator()(Args&&... aArgs) const
    {
        auto args = std::make_tuple(std::forward<Args>(aArgs)...);
        for (auto& func : m_funcs)
        {
            if (func)
                std::apply(func, args);
        }
    }

    void operator+=(const FuncType_t& acFunc) { m_funcs.push_back(acFunc); }
    void Add(const FuncType_t& acFunc) { m_funcs.push_back(acFunc); }

private:
    std::vector<FuncType_t> m_funcs;
};

template <typename Ret = void, typename... Args> struct Handler : HandlerImpl<Ret, Args...>
{
};