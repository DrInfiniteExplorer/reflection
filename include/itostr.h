#pragma once
#include <inttypes.h>
#include <time.h>

template <typename... T>
void strprintf(std::string& appendTo, T&&... t)
{
    static char arr[1024];
    sprintf_s(arr, std::forward<T>(t)...);
    appendTo.append(arr);
}

template <typename... T>
std::string strprintf(T&&... t)
{
    static char arr[1024];
    sprintf_s(arr, std::forward<T>(t)...);
    return std::string(arr);
}

namespace itostr
{

    template <typename T, size_t n> struct hexSpecImpl{};
    template <typename T> struct hexSpecImpl < T, 1 > { const char* fmt() const { return "%" PRIx8;  }; };
    template <typename T> struct hexSpecImpl < T, 2 > { const char* fmt() const { return "%" PRIx16; }; };
    template <typename T> struct hexSpecImpl < T, 4 > { const char* fmt() const { return "%" PRIx32; }; };
    template <typename T> struct hexSpecImpl < T, 8 > { const char* fmt() const { return "%" PRIx64; }; };
    template <typename T> const char* hexSpec() { return hexSpecImpl<T, sizeof(T)>().fmt(); }

    template <typename T, size_t n, bool isSigned> struct decSpecImpl{};
    template <typename T> struct decSpecImpl < T, 1, true > { const char* fmt() const { return "%" PRIi8;  }; };
    template <typename T> struct decSpecImpl < T, 2, true > { const char* fmt() const { return "%" PRIi16; }; };
    template <typename T> struct decSpecImpl < T, 4, true > { const char* fmt() const { return "%" PRIi32; }; };
    template <typename T> struct decSpecImpl < T, 8, true > { const char* fmt() const { return "%" PRIi64; }; };
    template <typename T> struct decSpecImpl < T, 1, false> { const char* fmt() const { return "%" PRIu8;  }; };
    template <typename T> struct decSpecImpl < T, 2, false> { const char* fmt() const { return "%" PRIu16; }; };
    template <typename T> struct decSpecImpl < T, 4, false> { const char* fmt() const { return "%" PRIu32; }; };
    template <typename T> struct decSpecImpl < T, 8, false> { const char* fmt() const { return "%" PRIu64; }; };
    template <typename T> const char* decSpec() { return decSpecImpl<T, sizeof(T), std::is_signed<T>::value>().fmt(); }

    template <typename T>
    static inline std::string str(const T t)
    {
        return std::string(t);
    }
    template <>
    static inline std::string str(const time_t t)
    {
        static char arr[231];
        ctime_s(arr, sizeof(arr), &t);
        if (arr[strlen(arr) - 1] == '\n')
            arr[strlen(arr) - 1] = 0;
        return std::string(arr);
    }

    template <typename T, typename std::enable_if<std::is_integral<T>::value>::type* = nullptr >
    static inline std::string hex(T t)
    {
        char arr[32];
        sprintf_s(arr, hexSpec<T>(), t);
        return std::string(arr);
    }

    template <typename T, typename std::enable_if<std::is_integral<T>::value>::type* = nullptr>
    static inline std::string dec(T t)
    {
        char arr[32];
        sprintf_s(arr, decSpec<T>(), t);
        return std::string(arr);
    }


}