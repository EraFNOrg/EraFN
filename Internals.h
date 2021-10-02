/*EraFN Copyright (C) 2021 Daniele "danii" Giompaolo

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.*/

#ifndef INTERNALS_H
#define INTERNALS_H

#include "xorstr.hpp"
#define XORSTRING(STR) xorstr(STR).crypt_get()
#include <Windows.h>
#include <psapi.h>
#include "UnrealEngineStructs.h"
#include "OffsetTable.h"
#include <vector>
#include <algorithm>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>
#include <fstream>

static __forceinline uintptr_t FindPattern(const char* signature, bool bRelative = false, uint32_t offset = 0)
{
	uintptr_t base_address = reinterpret_cast<uintptr_t>(GetModuleHandle(NULL));
	static auto patternToByte = [](const char* pattern)
	{
		auto bytes = std::vector<int>{};
		const auto start = const_cast<char*>(pattern);
		const auto end = const_cast<char*>(pattern) + strlen(pattern);

		for (auto current = start; current < end; ++current)
		{
			if (*current == '?')
			{
				++current;
				if (*current == '?') ++current;
				bytes.push_back(-1);
			}
			else { bytes.push_back(strtoul(current, &current, 16)); }
		}
		return bytes;
	};

	const auto dosHeader = (PIMAGE_DOS_HEADER)base_address;
	const auto ntHeaders = (PIMAGE_NT_HEADERS)((std::uint8_t*)base_address + dosHeader->e_lfanew);

	const auto sizeOfImage = ntHeaders->OptionalHeader.SizeOfImage;
	auto patternBytes = patternToByte(signature);
	const auto scanBytes = reinterpret_cast<std::uint8_t*>(base_address);

	const auto s = patternBytes.size();
	const auto d = patternBytes.data();

	for (auto i = 0ul; i < sizeOfImage - s; ++i)
	{
		bool found = true;
		for (auto j = 0ul; j < s; ++j)
		{
			if (scanBytes[i + j] != d[j] && d[j] != -1)
			{
				found = false;
				break;
			}
		}
		if (found)
		{
			uintptr_t address = reinterpret_cast<uintptr_t>(&scanBytes[i]);
			if (bRelative)
			{
				address = ((address + offset + 4) + *(int32_t*)(address + offset));
				return address;
			}
			return address;
		}
	}
	return NULL;
}

namespace EngineVersion
{
	void InitEngineVersion()
	{
		UniversalStructInstance->addr_GetEngineVersion = FindPattern(XORSTRING("40 53 48 83 EC 20 48 8B D9 E8 ? ? ? ? 48 8B C8 41 B8 04 ? ? ? 48 8B D3"));
	}

	string GetVersion(void* Result)
	{
		InitEngineVersion();
		auto ToConvert = reinterpret_cast<FString * (__fastcall*)(void*)>(UniversalStructInstance->addr_GetEngineVersion)(Result);
		string Converted;
		Converted = ToConvert->ToString();
		if (Converted.starts_with(XORSTRING("4.19")))
		{
			return "2.";
		}
		else
		{
			return Converted.substr(34, 4);
		}
	}

	string EngineVer(PVOID result)
	{
		InitEngineVersion();
		auto ToConvert = reinterpret_cast<FString * (__fastcall*)(void*)>(UniversalStructInstance->addr_GetEngineVersion)(result);
		string Converted = ToConvert->ToString();
		return Converted;
	}
}


#endif // !INTERNALS_H