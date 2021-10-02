/*EraFN Copyright (C) 2021 Daniele "danii" Giompaolo

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.*/

#ifndef SCRIPTFUNCTIONS_H
#define SCRIPTFUNCTIONS_H

//UObject* findObject("", false);
static duk_ret_t duk_findobject(duk_context* ctx)
{
	ARGS_LENGTH(ctx, 1);

	auto Object = UObject::GetObjectFromName(duk_get_string(ctx, 0), duk_get_boolean(ctx, 1));
	duk_push_pointer(ctx, Object);

	return 1;
}

//String getObjectName(Object);
static duk_ret_t duk_getobjectname(duk_context* ctx)
{
	ARGS_LENGTH(ctx, 1);

	UObject* Object = (UObject*)duk_get_pointer(ctx, 0);

	if (!Object || IsBadReadPtr(Object)) {
		duk_push_error_object(ctx, DUK_ERR_TYPE_ERROR, XORSTRING("Invalid Pointer."), (int)0);
		return duk_throw(ctx);
	}

	duk_push_string(ctx, Object->GetFullName().c_str());

	return 1;
}

//void* processEvent(Actor, Function);
static duk_ret_t duk_processevent(duk_context* ctx)
{
	ARGS_LENGTH(ctx, 2);

	Globals::ProcessEvent((UObject*)duk_get_pointer(ctx, 0), (UObject*)duk_get_pointer(ctx, 1), nullptr);

	return 0;
}

//UObject* getLocalPlayer();
static duk_ret_t duk_getlocalplayer(duk_context* ctx)
{
	duk_push_pointer(ctx, Globals::PlayerPawn);
	return 1;
}

//void setScaleActor(Actor, new FVector(1, 1, 1));
static duk_ret_t duk_setscaleactor(duk_context* ctx)
{
	ARGS_LENGTH(ctx, 2);

	UObject* Actor = (UObject*)duk_get_pointer(ctx, 0);

	if (!Actor || IsBadReadPtr(Actor)) {
		duk_push_error_object(ctx, DUK_ERR_TYPE_ERROR, XORSTRING("Invalid Pointer."), (int)0);
		return duk_throw(ctx);
	}

	if (!duk_is_fvector(ctx, 1)) {
		duk_push_error_object(ctx, DUK_ERR_TYPE_ERROR, XORSTRING("Invalid FVector."), (int)0);
		return duk_throw(ctx);
	}
	
	FVector Vector = duk_get_fvector(ctx, 1);
	SetActorScale3D(Actor, Vector);

	return 1;
}

//FVector getLocation(Actor);
static duk_ret_t duk_getlocation(duk_context* ctx)
{
	ARGS_LENGTH(ctx, 1);

	UObject* Actor = (UObject*)duk_get_pointer(ctx, 0);

	if (!Actor || IsBadReadPtr(Actor)) {
		duk_push_error_object(ctx, DUK_ERR_TYPE_ERROR, XORSTRING("Invalid Pointer."), (int)0);
		return duk_throw(ctx);
	}

	FVector Vector = GetLocation(Actor);

	if (!Vector.X || !Vector.Y || !Vector.Z) {
		return 0;
	}

	duk_push_fvector(ctx, Vector);

	return 1;
}

//void spawnPickup(Object, new FVector(1, 1, 1), 1);
static duk_ret_t duk_spawnpickup(duk_context* ctx)
{
	ARGS_LENGTH(ctx, 1);

	UObject* Object = (UObject*)duk_get_pointer(ctx, 0);

	if (!Object || IsBadReadPtr(Object)) {
		duk_push_error_object(ctx, DUK_ERR_TYPE_ERROR, XORSTRING("Invalid Pointer."), (int)0);
		return duk_throw(ctx);
	}

	int Count = duk_is_number(ctx, 2) ? duk_get_int(ctx, 2) : 1;
	FVector Vector = duk_is_fvector(ctx, 1) ? duk_get_fvector(ctx, 1) : GetLocation(Globals::PlayerPawn);

	SpawnPickupAtLocation(Object, Count, Vector);

	return 0;
}

//void executeConsoleCommand("slomo 2");
static duk_ret_t duk_executeconsolecommand(duk_context* ctx)
{
	ARGS_LENGTH(ctx, 1);

	std::string Cmd = duk_get_string(ctx, 0);
	std::wstring CmdW(Cmd.begin(), Cmd.end());

	ExecuteConsoleCommand(CmdW.c_str());

	return 0;
}

#endif // SCRIPTFUNCTIONS_H