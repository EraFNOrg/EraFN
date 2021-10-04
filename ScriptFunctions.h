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

static __forceinline std::string readAllText(const std::string& path);

duk_ret_t cb_resolve_module(duk_context* ctx) {
	const char* requested_id = duk_get_string(ctx, 0);
	const char* parent_id = duk_get_string(ctx, 1);

	duk_push_string(ctx, requested_id);
	return 1;
}

duk_ret_t cb_load_module(duk_context* ctx) {
	std::string module_id = duk_require_string(ctx, 0);
	duk_get_prop_string(ctx, 2, "filename");
	std::string filename = duk_require_string(ctx, -1);

	char* ENV;
	size_t len;
	_dupenv_s(&ENV, &len, XORSTRING("LOCALAPPDATA"));

	if (ENV == 0) ENV = (char*)".";

	auto module_source = readAllText((std::string)ENV + "\\ProjectEra\\" + filename);

	if (!module_source.c_str()) {
		duk_push_error_object(ctx, DUK_ERR_TYPE_ERROR, XORSTRING("Module not found."), (int)0);\
		return duk_throw(ctx);\
	}

	duk_push_string(ctx, module_source.c_str());

	return 1;
}

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

	VALIDATE_PTR(ctx, Object);

	duk_push_string(ctx, Object->GetFullName().c_str());

	return 1;
}

//void* processEvent(Actor, Function);
static duk_ret_t duk_processevent(duk_context* ctx)
{
	ARGS_LENGTH(ctx, 2);

	auto ReturnValue = Globals::ProcessEvent((UObject*)duk_get_pointer(ctx, 0), (UObject*)duk_get_pointer(ctx, 1), nullptr);
	duk_push_pointer(ctx, ReturnValue);

	return 1;
}

//void setScaleActor(Actor, new FVector(1, 1, 1));
static duk_ret_t duk_setscaleactor(duk_context* ctx)
{
	ARGS_LENGTH(ctx, 2);

	UObject* Actor = (UObject*)duk_get_pointer(ctx, 0);

	VALIDATE_PTR(ctx, Actor);

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

	VALIDATE_PTR(ctx, Actor);

	FVector Vector = GetLocation(Actor);
	duk_push_fvector(ctx, Vector);

	return 1;
}

//void spawnPickup(Object, new FVector(1, 1, 1), 1);
static duk_ret_t duk_spawnpickup(duk_context* ctx)
{
	ARGS_LENGTH(ctx, 1);

	UObject* Object = (UObject*)duk_get_pointer(ctx, 0);

	VALIDATE_PTR(ctx, Object);

	int Count = duk_is_number(ctx, 2) ? duk_get_int(ctx, 2) : 1;
	FVector Vector = duk_is_fvector(ctx, 1) ? duk_get_fvector(ctx, 1) : GetLocation(Globals::PlayerPawn);

	SpawnPickupAtLocation(Object, Count, Vector);

	return 0;
}

//UObject* spawnActor(Object, new FVector(1, 1, 1), new FRotator(1, 1, 1));
static duk_ret_t duk_spawnactor(duk_context* ctx)
{
	ARGS_LENGTH(ctx, 2);

	UObject* Object = (UObject*)duk_get_pointer(ctx, 0);

	VALIDATE_PTR(ctx, Object);

	if (!duk_is_fvector(ctx, 1)) {
		duk_push_error_object(ctx, DUK_ERR_TYPE_ERROR, XORSTRING("Invalid FVector."), (int)0);
		return duk_throw(ctx);
	}

	FVector Vector = duk_get_fvector(ctx, 1);
	FRotator Rotator = duk_is_frotator(ctx, 2) ? duk_get_frotator(ctx, 2) : FRotator{ 0, 0, 0 };

	auto Actor = SpawnActorFromClassObj(Object, Vector, Rotator);
	Actor ? duk_push_pointer(ctx, Actor) : duk_push_null(ctx);

	return 1;
}

//void destroyActor(Actor);
static duk_ret_t duk_destroyactor(duk_context* ctx)
{
	ARGS_LENGTH(ctx, 1);

	UObject* Actor = (UObject*)duk_get_pointer(ctx, 0);

	VALIDATE_PTR(ctx, Actor);

	Destroy(Actor);

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

//void teleport(Actor, new FVector(1, 1, 1), new FRotator(1, 1, 1));
static duk_ret_t duk_teleport(duk_context* ctx)
{
	ARGS_LENGTH(ctx, 2);

	UObject* Object = (UObject*)duk_get_pointer(ctx, 0);

	VALIDATE_PTR(ctx, Object);

	if (!duk_is_fvector(ctx, 1)) {
		duk_push_error_object(ctx, DUK_ERR_TYPE_ERROR, XORSTRING("Invalid FVector."), (int)0);
		return duk_throw(ctx);
	}

	FVector Vector = duk_get_fvector(ctx, 1);
	FRotator Rotator = duk_is_frotator(ctx, 2) ? duk_get_frotator(ctx, 2) : FRotator{ 0, 0, 0 };

	TeleportTo(Rotator, Vector, Object);

	return 0;
}

//void grantAbility(Object);
static duk_ret_t duk_grantability(duk_context* ctx)
{
	ARGS_LENGTH(ctx, 1);

	UObject* Object = (UObject*)duk_get_pointer(ctx, 0);

	VALIDATE_PTR(ctx, Object);

	GrantAbility(Object);

	return 0;
}

//void messageBox("Hello, World!");
static duk_ret_t duk_messagebox(duk_context* ctx)
{
	ARGS_LENGTH(ctx, 1);

	std::string Msg = duk_get_string(ctx, 0);
	std::wstring MsgW(Msg.begin(), Msg.end());

	MessageBox(0, (LPCWSTR)MsgW.c_str(), (LPCWSTR)XORSTRING(L"Project Era"), MB_OK);

	return 0;
}

//Array<UObject*>/UObject* findActorsOfClass(Class, idx);
static duk_ret_t duk_findactorsofclass(duk_context* ctx)
{
	ARGS_LENGTH(ctx, 1);

	UObject* Class = (UObject*)duk_get_pointer(ctx, 0);

	VALIDATE_PTR(ctx, Class);

	if (duk_is_number(ctx, 1)) {
		duk_push_pointer(ctx, FindActorsFromClass(Class, duk_get_int(ctx, 1)));
	}
	else {
		auto arrIdx = duk_push_array(ctx);
		auto Actors = ArrayFindActorsFromClass(Class);

		for (int i = 0; i < Actors.Num(); i++) {
			duk_push_pointer(ctx, Actors[i]);
			duk_put_prop_index(ctx, arrIdx, i);
		}
	}

	return 1;
}

//void addToInventory(Object, idx);
static duk_ret_t duk_addtoinventory(duk_context* ctx)
{
	ARGS_LENGTH(ctx, 1);

	UObject* Object = (UObject*)duk_get_pointer(ctx, 0);
	auto slot = duk_is_number(ctx, 1) ? duk_get_number(ctx, 1) : 1;

	VALIDATE_PTR(ctx, Object);

	AddToInventory(CreateItem(Object, 0), EFortQuickBars::Primary, slot);
	UpdateInventory();

	return 1;
}

//bool isInVehicle();
static duk_ret_t duk_isinvehicle(duk_context* ctx)
{
	duk_push_boolean(ctx, IsInVehicle());
	return 1;
}

//UObject* getVehicle();
static duk_ret_t duk_getvehicle(duk_context* ctx)
{
	duk_push_pointer(ctx, GetVehicle());
	return 1;
}

//void playSound(Sound, new FVector(1, 1, 1), 1, 1);
static duk_ret_t duk_playsound(duk_context* ctx)
{
	ARGS_LENGTH(ctx, 1);

	UObject* Sound = (UObject*)duk_get_pointer(ctx, 0);
	FVector Location = duk_is_fvector(ctx, 1) ? duk_get_fvector(ctx, 1) : GetLocation(Globals::PlayerPawn);
	float Volume = duk_is_number(ctx, 2) ? duk_get_number(ctx, 2) : 1;
	float Pitch = duk_is_number(ctx, 3) ? duk_get_number(ctx, 3) : 1;

	VALIDATE_PTR(ctx, Sound);

	ClientPlaySoundAtLocation(Globals::PlayerController, Sound, Location, Volume, Pitch);

	return 0;
}


//UObject* getLocalPlayer()
static duk_ret_t duk_getlocalplayer(duk_context* ctx)
{
    duk_push_pointer(ctx, Globals::PlayerPawn);
    return 1;
}

#endif // SCRIPTFUNCTIONS_H
