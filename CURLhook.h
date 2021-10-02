/*EraFN Copyright (C) 2021 Daniele "danii" Giompaolo

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.*/

#ifndef CURLHOOK_H
#define CURLHOOK_H
#include <Windows.h>
#include "LeoSpecial.h"
#include "Internals.h"
#include "OffsetTable.h"
#include "curl.h"
#include <regex>
#include "minhook/MinHook.h"
#pragma comment(lib, "minhook/libMinHook.x64.lib")
#include "Hooks.h"
#include "Functions.h"

namespace CURLhook
{
	bool bHookRestarted = false;
	bool bInitGlobals = false;
	bool bMatchmaking = false;
	GameManager ClassInstance;
	void RestartHook();

	namespace Hooks
	{


		CURLcode(*CurlSet)(struct Curl_easy*, CURLoption, va_list) = nullptr;
		CURLcode CurlSetHook2(struct Curl_easy* A, CURLoption B, ...)
		{
			va_list argument;
			va_start(argument, B);

			const auto result = CurlSet(A, B, argument);

			va_end(argument);
			return result;
		}

		CURLcode(*CurlEasy)(struct Curl_easy*, CURLoption, ...) = nullptr;
		CURLcode CurlEasyHook2(struct Curl_easy* A, CURLoption B, ...)
		{

			va_list argument2;
			va_list argumentCopied;
			va_start(argument2, B);

			CURLcode result = {};

			va_copy(argumentCopied, argument2);
			const regex EpicGamesAddress(XORSTRING("https:\\/\\/(.*)\\.ol\\.epicgames.com"));

			if (B == CURLOPT_SSL_VERIFYPEER)
			{
				result = CurlSetHook2(A, B, 0);
			}

			if (B == CURLOPT_URL)
			{
				std::string Url(va_arg(argumentCopied, PCHAR));
				if (regex_search(Url, EpicGamesAddress)) {
					Url = regex_replace(Url, EpicGamesAddress, "http://erafnbackend.herokuapp.com");
					std::string URLSTRING = Url.c_str();

					if (URLSTRING.find(XORSTRING("/account/api/oauth/token")) != string::npos && !bHookRestarted)
					{
#if defined(TESTINGVERSION)
						EraInit::Console();
#endif

						EraInit::SetOffsets();

						UniversalStructInstance->CURLhook.Unhook();
						DetourTransactionBegin();
						DetourUpdateThread(GetCurrentThread());

						DetourAttach(&(void*&)Hooks::CurlEasy, Hooks::CurlEasyHook2);

						DetourTransactionCommit();

						bHookRestarted = true;
					}

					if (URLSTRING.find(XORSTRING("/client/ClientQuestLogin")) != std::string::npos && !bInitGlobals) {
						bInitGlobals = true;
#ifndef SERVERCODE
						InitializeHooks();
						ClassInstance.InitializeGlobals();
						ClassInstance.HandleInitializeConsole();
#endif 

#if defined(RELEASEVERSION)
						EraInit::Console();
#endif
					}

					if (URLSTRING.find(XORSTRING("matchmakingservice")) != std::string::npos && !bMatchmaking) {
						bMatchmaking = true;
						ClassInstance.OpenAthenaTerrain();
						Globals::bPressedPlay = true;
					}

				}

				result = CurlSetHook2(A, B, Url.c_str());

			}
			else
			{
				result = CurlSet(A, B, argument2);
			}
			va_end(argument2);
			return result;
		}


	}

	void StartHook()
	{
		UniversalStructInstance->CurlPattern = FindPattern(XORSTRING("89 54 24 10 4C 89 44 24 18 4C 89 4C 24 20 48 83 EC 28 48 85 C9 75 08 8D 41 2B 48 83 C4 28 C3 4C"));
		UniversalStructInstance->CurlPatternSet = FindPattern(XORSTRING("48 89 5C 24 08 48 89 6C 24 10 48 89 74 24 18 57 48 83 EC 30 33 ED 49 8B F0 48 8B D9"));
		
		Hooks::CurlEasy = decltype(Hooks::CurlEasy)(UniversalStructInstance->CurlPattern);
		Hooks::CurlSet = decltype(Hooks::CurlSet)(UniversalStructInstance->CurlPatternSet);

		UniversalStructInstance->CURLhook.Hook((uintptr_t)Hooks::CurlEasy, (uintptr_t)Hooks::CurlEasyHook2);
	}

}

#endif // !CURLHOOK_H

