/*EraFN Copyright (C) 2021 Daniele "danii" Giompaolo

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.*/

#ifndef SCRIPT_H
#define SCRIPT_H

#define ARGS_LENGTH(ctx, len) {\
	int ArgsLength = duk_get_top(ctx);\
	if (ArgsLength < len)\
	{\
		duk_push_error_object(ctx, DUK_ERR_TYPE_ERROR, XORSTRING("Invalid argument amount."), (int)ArgsLength);\
		return duk_throw(ctx);\
	}\
}

#define ARRAY_LENGTH(ctx, obj, len) {\
	int ArrLength = duk_get_length(ctx, obj);\
	if (ArrLength < len)\
	{\
		duk_push_error_object(ctx, DUK_ERR_TYPE_ERROR, XORSTRING("Invalid array length."), (int)ArrLength);\
		return duk_throw(ctx);\
	}\
}

#include <filesystem>
#include "source/duktape.h"
#include "source/duk_console.h"
#include "ScriptClasses.h"
#include "ScriptFunctions.h"

std::string readAllText(const std::string& path);

namespace EraScript {
	static void ErrorHandler(void* udata, const char* msg) {
		(void)udata;
		std::string message = XORSTRING("*** FATAL ERROR: ");
		message.append(msg ? msg : XORSTRING("NO MESSAGE"));
		std::wstring messageW(message.begin(), message.end());

		printf("\n%s\n\n", message.c_str());
		Say(messageW.c_str());
		return;
	}

	static void Eval(std::string code) {
		duk_eval_string_noresult(Globals::DukContext, code.c_str());
		return;
	}

	static void Execute() {
		std::string name = XORSTRING("execute");

		char* ENV;
		size_t len;
		_dupenv_s(&ENV, &len, XORSTRING("LOCALAPPDATA"));

		if (ENV == 0) ENV = (char*)".\\";

		std::string EraPath = ENV + (std::string)XORSTRING("\\ProjectEra\\");
		std::wstring EraPathW(EraPath.begin(), EraPath.end());
		std::string ScriptPath = EraPath + name + XORSTRING(".js");

		if (
			(CreateDirectory(EraPathW.c_str(), 0) || ERROR_ALREADY_EXISTS == GetLastError()) &&
			filesystem::exists(ScriptPath)
		) {
			Eval(readAllText(ScriptPath));
		}
		else {
			Say(XORSTRING(L"Failed to create a ProjectEra folder/Read the script"));
		}
	}

	static void Init() {
		duk_context* ctx = duk_create_heap(0, 0, 0, 0, ErrorHandler);
		Globals::DukContext = ctx;

		duk_console_init(ctx, 0);

		duk_push_c_function(ctx, duk_executeconsolecommand, DUK_VARARGS);
		duk_put_global_string(ctx, XORSTRING("executeConsoleCommand"));

		duk_push_c_function(ctx, duk_findobject, DUK_VARARGS);
		duk_put_global_string(ctx, XORSTRING("findObject"));

		duk_push_c_function(ctx, duk_getobjectname, DUK_VARARGS);
		duk_put_global_string(ctx, XORSTRING("getObjectName"));

		duk_push_c_function(ctx, duk_processevent, DUK_VARARGS);
		duk_put_global_string(ctx, XORSTRING("processEvent"));

		duk_push_c_function(ctx, duk_getlocation, DUK_VARARGS);
		duk_put_global_string(ctx, XORSTRING("getLocation"));

		duk_push_c_function(ctx, duk_getlocalplayer, DUK_VARARGS);
		duk_put_global_string(ctx, XORSTRING("getLocalPlayer"));

		duk_push_c_function(ctx, duk_setscaleactor, DUK_VARARGS);
		duk_put_global_string(ctx, XORSTRING("setScaleActor"));

		duk_push_c_function(ctx, duk_spawnpickup, DUK_VARARGS);
		duk_put_global_string(ctx, XORSTRING("spawnPickup"));

		duk_push_c_function(ctx, duk_fvector, DUK_VARARGS);
		duk_put_global_string(ctx, XORSTRING("FVector"));
		
		duk_push_c_function(ctx, duk_frotator, DUK_VARARGS);
		duk_put_global_string(ctx, XORSTRING("FRotator"));
	}
}

// https://github.com/FortniteModding/Ultimanite/blob/main/Ultimanite/util.h#L48
static __forceinline std::string readAllText(const std::string& path)
{
	std::ifstream input_file(path);
	if (!input_file.is_open()) return std::string();

	return std::string((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
}

#endif // SCRIPT_H