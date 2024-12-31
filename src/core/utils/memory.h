#pragma once
#include <Windows.h>
#include <vector>

namespace Memory {

bool IsValidAndWritable(void* Address);

PIMAGE_DOS_HEADER GetDosHeader(uintptr_t ModuleBase);

PIMAGE_NT_HEADERS64 GetNtHeaders(uintptr_t ModuleBase);

PIMAGE_NT_HEADERS64 GetNtHeaders(uintptr_t ModuleBase);

PIMAGE_SECTION_HEADER GetSectionFromName(const char* Name, uintptr_t ModuleBase);

uint64_t GetModuleSize(uintptr_t ModuleBase);
bool IsAddressInsideModule(uintptr_t Address, uintptr_t ModuleBase);

bool IsAddressInsideModule(uintptr_t Address, uintptr_t ModuleBase);

uintptr_t GetImageBase();
uint64_t GetImageSize();
PIMAGE_SECTION_HEADER GetImageTextSection();
bool IsAddressInsideImage(uintptr_t Address);

template <typename OperandSize = int32_t>
uintptr_t PatternScanRangeBytes(uintptr_t StartAddress, uint64_t ScanSize, const std::vector<int>& Bytes,
                                bool Backwards = false, size_t Offset = -1, bool RelativeAddress = false,
                                bool WarnIfNotFound = true);

template <typename OperandSize = int32_t>
uintptr_t PatternScanRange(uintptr_t StartAddress, uint64_t ScanSize, const char* Pattern, bool Backwards = false,
                           size_t Offset = -1, bool RelativeAddress = false);

template <typename OperandSize = int32_t>
uintptr_t PatternScan(const char* Pattern, size_t Offset = -1, bool RelativeAddress = false);

template <typename StringType>
static uintptr_t FindStringRefRange(StringType RefString, uint8_t* StartAddress, size_t ScanSize);

template <typename StringType = const char*> uintptr_t FindStringRef(StringType RefString);

template <typename T, int Increment = 4>
uint32_t FindMatchingValueOffset(const std::vector<std::pair<void*, T>>& ObjectValuePairs, uint32_t MinOffset = 0x28,
                                 uint32_t MaxOffset = 0x1A0);

}

#include "memory.inl"
