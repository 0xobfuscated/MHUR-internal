#pragma once
#include "memory.h"

namespace Memory {
	template <typename OperandSize>
	static uintptr_t PatternScanRangeBytes(uintptr_t StartAddress, uint64_t ScanSize, const std::vector<int>& Bytes,
		bool Backwards, size_t Offset, bool RelativeAddress, bool WarnIfNotFound) {
		const size_t   PatternLength = Bytes.size();
		const uint8_t* ScanBytes = reinterpret_cast<uint8_t*>(StartAddress);

		auto CheckPattern = [&](const uint8_t* Address) {
			bool Found = true;

			for (size_t j = 0; j < PatternLength; j++) {
				if (Address[j] != Bytes[j] && Bytes[j] != -1) {
					Found = false;
					break;
				}
			}

			if (Found) {
				uintptr_t uAddress = (uintptr_t)Address;

				if (RelativeAddress) {
					if (Offset == -1)
						Offset = PatternLength;

					uAddress = ((uAddress + Offset + sizeof(OperandSize)) + *(OperandSize*)(uAddress + Offset));
				}

				return uAddress;
			}

			return uintptr_t();
			};

		if (Backwards) {
			for (uintptr_t i = PatternLength; i < ScanSize; i++) {
				uint8_t* Address = (uint8_t*)(ScanBytes - i);
				uintptr_t Result = CheckPattern(Address);
				if (Result)
					return Result;
			}
		}
		else {
			for (uintptr_t i = 0; i < ScanSize - PatternLength; i++) {
				uint8_t* Address = (uint8_t*)(ScanBytes + i);
				uintptr_t Result = CheckPattern(Address);
				if (Result)
					return Result;
			}
		}

#if CFG_DEBUGMEMORYOUTPUT
		if (WarnIfNotFound)
			LOG(LOG_WARN, "Failed to find pattern! (0x%p, 0x%p)", reinterpret_cast<void*>(StartAddress),
				reinterpret_cast<void*>(ScanSize));
#endif

		return 0;
	}

	template <typename OperandSize>
	static uintptr_t PatternScanRange(uintptr_t StartAddress, uint64_t ScanSize, const char* Pattern, bool Backwards,
		size_t Offset, bool RelativeAddress) {
		static auto PatternToByte = [](const char* Pattern) {
			std::vector<int> Result;
			const auto       PatternStart = const_cast<char*>(Pattern);
			const auto       PatternEnd = const_cast<char*>(Pattern) + strlen(Pattern);

			for (auto Current = PatternStart; Current < PatternEnd; ++Current) {
				if (*Current == '?') {
					++Current;

					// Account for if 2 question marks were used in a row
					// People generally use "?" and "??" interchangeably in patterns
					if (*Current == '?')
						++Current;

					Result.push_back(-1);
				}
				else {
					Result.push_back(strtoul(Current, &Current, 16));
				}
			}

			return Result;
			};

		std::vector<int> Bytes = PatternToByte(Pattern);
		return PatternScanRangeBytes<OperandSize>(StartAddress, ScanSize, Bytes, Backwards, Offset, RelativeAddress);
	}

	template <typename OperandSize> static uintptr_t PatternScan(const char* Pattern, size_t Offset, bool RelativeAddress) {
		return PatternScanRange<OperandSize>(GetImageBase(), GetImageSize(), Pattern, false, Offset, RelativeAddress);
	}

	template <typename StringType>
	static uintptr_t FindStringRefRange(StringType RefString, uint8_t* StartAddress, size_t ScanSize) {
		constexpr bool IsChar = std::is_same_v<StringType, const char*>;
		constexpr bool IsWChar = std::is_same_v<StringType, const wchar_t*>;
		static_assert(IsChar || IsWChar, "FindStringRefRange only supports `const char*` and `const wchar_t*`!");

		// Give head room of 7 to ensure no out of bounds issues ever occur.
		uint8_t* Data = StartAddress;
		for (size_t i = 0; i < ScanSize - 7; i++) {
			// Check if the instruction is a LEA REG REL32_IMM.
			if ((Data[i] == 0x4C || Data[i] == 0x48) && Data[i + 1] == 0x8D) {
				// Convert REL32_IMM to absolute address.
				const void* FoundStr = *(int32_t*)(Data + i + 3) + 7 + Data + i;
				if (IsAddressInsideImage(reinterpret_cast<uintptr_t>(FoundStr))) {
					if constexpr (IsChar) {
						if (strcmp(RefString, static_cast<const char*>(FoundStr)) == 0)
							return reinterpret_cast<uintptr_t>(Data + i);
					}
					else if constexpr (IsWChar) {
						if (wcscmp(RefString, static_cast<const wchar_t*>(FoundStr)) == 0)
							return reinterpret_cast<uintptr_t>(Data + i);
					}
				}

				i += 7;
			}
		}

#if CFG_DEBUGMEMORYOUTPUT
		if constexpr (IsChar)
			LOG(LOG_WARN, "Failed to find string! (%s)", RefString);
		else if constexpr (IsWChar)
			LOG(LOG_WARN, "Failed to find string! (%S)", RefString);
#endif

		return 0;
	}

	template <typename StringType> static uintptr_t FindStringRef(StringType RefString) {
		const PIMAGE_SECTION_HEADER TextSection = GetImageTextSection();
		if (!TextSection)
			return 0;

		uint8_t* TextSectionData = reinterpret_cast<uint8_t*>(GetImageBase() + TextSection->VirtualAddress);
		return FindStringRefRange<StringType>(RefString, TextSectionData, TextSection->Misc.VirtualSize);
	}

	template <typename T, int Increment>
	static uint32_t FindMatchingValueOffset(const std::vector<std::pair<void*, T>>& ObjectValuePairs, uint32_t MinOffset,
		uint32_t MaxOffset) {
		uint32_t Offset = MinOffset;
		for (uint32_t Offset = MinOffset; Offset <= MaxOffset; Offset += Increment) {
			size_t MatchingCount = 0;
			for (const auto& Pair : ObjectValuePairs) {
				T* Value = reinterpret_cast<T*>((uintptr_t)Pair.first + Offset);
				if (*Value == Pair.second)
					MatchingCount++;
			}

			if (MatchingCount == ObjectValuePairs.size())
				return Offset;
		}

#if CFG_DEBUGMEMORYOUTPUT
		LOG(LOG_WARN, "Failed to find matching value offset!");
#endif
		return 0;
	}

}
