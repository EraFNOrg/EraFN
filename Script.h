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

#define VALIDATE_PTR(ctx, ptr) {\
	if (!ptr || IsBadReadPtr(ptr)) {\
		duk_push_error_object(ctx, DUK_ERR_TYPE_ERROR, XORSTRING("Invalid Pointer."), (int)0);\
		return duk_throw(ctx);\
	}\
};

#include <filesystem>
#include "source/duktape.h"
#include "source/duk_console.h"
#include "source/duk_module_node.h"
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
	}

	static void Eval(std::string code) {
		duk_eval_string_noresult(Globals::DukContext, code.c_str());
	}

	static void Execute() {
		char* ENV;
		size_t len;
		_dupenv_s(&ENV, &len, XORSTRING("LOCALAPPDATA"));

		if (ENV == 0) ENV = (char*)".\\";

		std::string EraPath = (std::string)ENV + XORSTRING("\\ProjectEra\\");
		std::string ScriptPath = EraPath + XORSTRING("execute") + XORSTRING(".js");

		if (CreateDirectoryA(EraPath.c_str(), 0) || ERROR_ALREADY_EXISTS == GetLastError() && filesystem::exists(ScriptPath)) {
			Eval(readAllText(ScriptPath));
		}
		else {
			Say(XORSTRING(L"Failed to create a ProjectEra folder/Read the script"));
		}
	}

	static void Init() {
		duk_context* ctx = duk_create_heap(0, 0, 0, 0, ErrorHandler);
		Globals::DukContext = ctx;

		// EXTRAS

		duk_console_init(ctx, 0); // console

		duk_push_object(ctx); // modules
		duk_push_c_function(ctx, cb_resolve_module, DUK_VARARGS);
		duk_put_prop_string(ctx, -2, "resolve");
		duk_push_c_function(ctx, cb_load_module, DUK_VARARGS);
		duk_put_prop_string(ctx, -2, "load");
		duk_module_node_init(ctx);

		// FUNCTIONS

		duk_push_c_function(ctx, duk_fvector, DUK_VARARGS);
		duk_put_global_string(ctx, XORSTRING("FVector"));

		duk_push_c_function(ctx, duk_frotator, DUK_VARARGS);
		duk_put_global_string(ctx, XORSTRING("FRotator"));

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

		duk_push_c_function(ctx, duk_setscaleactor, DUK_VARARGS);
		duk_put_global_string(ctx, XORSTRING("setScaleActor"));

		duk_push_c_function(ctx, duk_spawnpickup, DUK_VARARGS);
		duk_put_global_string(ctx, XORSTRING("spawnPickup"));

		duk_push_c_function(ctx, duk_spawnactor, DUK_VARARGS);
		duk_put_global_string(ctx, XORSTRING("spawnActor"));

		duk_push_c_function(ctx, duk_teleport, DUK_VARARGS);
		duk_put_global_string(ctx, XORSTRING("teleport"));

		duk_push_c_function(ctx, duk_grantability, DUK_VARARGS);
		duk_put_global_string(ctx, XORSTRING("grantAbility"));

		duk_push_c_function(ctx, duk_messagebox, DUK_VARARGS);
		duk_put_global_string(ctx, XORSTRING("messageBox"));

		duk_push_c_function(ctx, duk_destroyactor, DUK_VARARGS);
		duk_put_global_string(ctx, XORSTRING("destroyActor"));

		duk_push_c_function(ctx, duk_findactorsofclass, DUK_VARARGS);
		duk_put_global_string(ctx, XORSTRING("findActorsOfClass"));

		duk_push_c_function(ctx, duk_addtoinventory, DUK_VARARGS);
		duk_put_global_string(ctx, XORSTRING("addToInventory"));

		duk_push_c_function(ctx, duk_isinvehicle, DUK_VARARGS);
		duk_put_global_string(ctx, XORSTRING("isInVehicle"));

		duk_push_c_function(ctx, duk_getvehicle, DUK_VARARGS);
		duk_put_global_string(ctx, XORSTRING("getVehicle"));

		duk_push_c_function(ctx, duk_playsound, DUK_VARARGS);
		duk_put_global_string(ctx, XORSTRING("playSound"));

                duk_push_c_function(ctx, duk_getlocalplayer, DUK_VARARGS);
		duk_put_global_string(ctx, XORSTRING("getLocalPlayer"));

		// GLOBAL VARIABLES

		duk_push_tarray(ctx, &Globals::AmmoItemCount);
		duk_put_global_string(ctx, XORSTRING("ammoItemCount"));

		duk_push_tarray(ctx, &Globals::AmmoItemDefs);
		duk_put_global_string(ctx, XORSTRING("ammoItemDefs"));

		duk_push_tarray(ctx, &Globals::ConsumablesItemCount);
		duk_put_global_string(ctx, XORSTRING("consumablesItemCount"));

		duk_push_tarray(ctx, &Globals::ConsumablesItemDefs);
		duk_put_global_string(ctx, XORSTRING("consumablesItemDefs"));

		duk_push_tarray(ctx, &Globals::ResourceItemCount);
		duk_put_global_string(ctx, XORSTRING("resourceItemCount"));

		duk_push_tarray(ctx, &Globals::ResourceItemDefs);
		duk_put_global_string(ctx, XORSTRING("resourceItemDefs"));

		duk_push_tarray(ctx, &Globals::TrapsItemCount);
		duk_put_global_string(ctx, XORSTRING("trapsItemCount"));

		duk_push_tarray(ctx, &Globals::TrapsItemDefs);
		duk_put_global_string(ctx, XORSTRING("trapsItemDefs"));

		duk_push_tarray(ctx, &Globals::WeaponItemCount);
		duk_put_global_string(ctx, XORSTRING("weaponItemCount"));

		duk_push_tarray(ctx, &Globals::WeaponItemDefs);
		duk_put_global_string(ctx, XORSTRING("weaponItemDefs"));

		duk_push_pointer(ctx, &Globals::GameMode);
		duk_put_global_string(ctx, XORSTRING("gameMode"));

		duk_push_pointer(ctx, &Globals::World);
		duk_put_global_string(ctx, XORSTRING("world"));

		duk_push_string(ctx, Globals::CurrentFortniteVersion.c_str());
		duk_put_global_string(ctx, XORSTRING("fortniteVersion"));
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
