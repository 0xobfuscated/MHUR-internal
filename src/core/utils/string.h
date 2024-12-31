#pragma once
#include <string>

namespace String {

std::wstring NarrowToWide(const std::string& String);
std::string  WideToNarrow(const std::wstring& WideString);

}
