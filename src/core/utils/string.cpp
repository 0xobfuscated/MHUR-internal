#include "string.h"

#include <Windows.h>

namespace String {

std::wstring NarrowToWide(const std::string& String) {
    if (String.empty())
        return L"";

#if 1
    return std::wstring(String.begin(), String.end());
#else
    int Length = MultiByteToWideChar(CP_UTF8, 0, String.c_str(), static_cast<int>(String.length()), nullptr, 0);
    if (Length <= 0)
        return L"";

    std::wstring WideString(Length, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, String.c_str(), static_cast<int>(String.length()), &WideString[0], Length);

    return WideString;
#endif
}

std::string WideToNarrow(const std::wstring& WideString) {
    if (WideString.empty())
        return "";

#if 1
    return std::string(WideString.begin(), WideString.end());
#else
    int Length = WideCharToMultiByte(CP_UTF8, 0, WideString.c_str(), static_cast<int>(WideString.length()), nullptr, 0,
                                     nullptr, nullptr);
    if (Length <= 0)
        return "";

    std::string NarrowString(Length, '\0');
    WideCharToMultiByte(CP_UTF8, 0, WideString.c_str(), static_cast<int>(WideString.length()), &NarrowString[0], Length,
                        nullptr, nullptr);

    return NarrowString;
#endif
}

}
