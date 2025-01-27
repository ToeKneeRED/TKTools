#pragma once

#include <string>

namespace ToeKnee
{
struct String : std::string
{
    using std::string::string;

    bool operator==(const std::string& rhs) const { return this->c_str() == rhs.c_str(); }
    bool operator==(const String& rhs) const { return this->c_str() == rhs.c_str(); }
    String& operator=(std::string&& acString)
    {
        std::string::operator=(std::move(acString));
        return *this;
    }
    String& operator=(const std::string& acString)
    {
        std::string::operator=(acString);
        return *this;
    }
    String& operator=(const char* acString)
    {
        std::string::operator=(acString);
        return *this;
    }

    // match snake case
    bool starts_with(const std::string& acPrefix) const { return this->compare(0, acPrefix.length(), acPrefix) == 0; }
    bool starts_with(const char* acPrefix) const { return this->compare(0, strlen(acPrefix), acPrefix) == 0; }
};
} // namespace ToeKnee