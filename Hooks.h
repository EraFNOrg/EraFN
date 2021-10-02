/*EraFN Copyright (C) 2021 Daniele "danii" Giompaolo

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.*/

#ifndef PROCESSEVENT_H
#define PROCESSEVENT_H

#include <Windows.h>
#include "UnrealEngineStructs.h"
#include "minhook/MinHook.h"
#pragma comment(lib, "minhook/libMinHook.x64.lib")
#include "Internals.h"
#include "OffsetTable.h"
#include "Functions.h"
#include "Server/netcode.h"
#include "Script.h"
#include <Detours.h>

GameManager ClassInstance;
bool bMatchStarted = false;
bool bLoadingScreenDropped = false;

void* ProcessEventHook(UObject* A, UObject* B, PVOID C)
{
	if (B->GetFullName().find(XORSTRING("CheatScript")) != string::npos)
	{
#if defined (SERVERCODE)
		if (((FString*)C)->ToString().find(XORSTRING("StartListening")) != string::npos) ClassInstance.InitializeGlobals();
#endif
		if (((FString*)C)->ToString().find(XORSTRING("execute")) != string::npos) CreateThread(0, 0, (LPTHREAD_START_ROUTINE)EraScript::Execute, 0, 0, 0);
		if (((FString*)C)->ToString().find(XORSTRING("StartGame")) != string::npos && !Globals::bPressedPlay)
		{
			ClassInstance.OpenAthenaTerrain();
			Globals::bPressedPlay = true;
		}
		ClassInstance.ExecuteCheatScript(C);
	}
	if (B->GetObjectNameString() == "ServerHandlePickup" && bLoadingScreenDropped)
	{
		ClassInstance.HandlePickup(C);
	}
	if (B->GetObjectNameString().find(XORSTRING("ServerCreateBuilding")) != string::npos)
	{
		ClassInstance.Build();
	}
	if (B->GetObjectNameString().find(XORSTRING("ServerAttemptInventoryDrop")) != string::npos || B->GetObjectNameString().find(XORSTRING("ServerSpawnInventoryDrop")) != string::npos)
	{
		ClassInstance.HandleInventoryDrop(C);
	}
	if (B->GetObjectNameString().find(XORSTRING("ReadyToStartMatch")) != string::npos && Globals::bPressedPlay && !bMatchStarted)
	{
		ClassInstance.Init();
		bMatchStarted = true;
	}
	if (B->GetObjectNameString().find(XORSTRING("LoadingScreenDropped")) != string::npos && Globals::bPressedPlay && bMatchStarted && !bLoadingScreenDropped)
	{
		ClassInstance.OnLoadingScreenDropped();
		bLoadingScreenDropped = true;
	}
	if (B->GetObjectNameString().find(XORSTRING("Tick")) != string::npos && bLoadingScreenDropped)
	{
		ClassInstance.Tick();
	}
	if (B->GetObjectNameString().find(XORSTRING("ServerAttemptExitVehicle")) != string::npos && bLoadingScreenDropped)
	{
		ClassInstance.ExitVehicle();
	}
	if (B->GetObjectNameString().find(XORSTRING("ServerAttemptInteract")) != string::npos && bLoadingScreenDropped)
	{
		ClassInstance.InteractWithContainer(C);
	}
	if ((B->GetObjectNameString().find(XORSTRING("AttemptAircraftJump")) != string::npos && bLoadingScreenDropped) || (B->GetObjectNameString().find(XORSTRING("ExitedDropZone")) != string::npos && bLoadingScreenDropped))
	{
		if (!Globals::bJumpedFromBus)
		{
			ClassInstance.JumpFromAircraft();
			Globals::bJumpedFromBus = true;
		}
	}
	if ((B->GetObjectNameString().find(XORSTRING("ServerExecuteInventoryItem")) != string::npos ||
		B->GetObjectNameString().find(XORSTRING("ServerExecuteInventoryWeapon")) != string::npos) && bLoadingScreenDropped)
	{
#if defined(TESTINGVERSION)
		printf("Function name: %s\n", B->GetObjectNameString().c_str());
#endif
		if (B->GetObjectNameString().find(XORSTRING("ServerExecuteInventoryWeapon")) != string::npos) ClassInstance.EquipInventoryItem(*(FGuid*)C, *(UObject**)C);
		else ClassInstance.EquipInventoryItem(*(FGuid*)C);
	}
	if (B->GetObjectNameString().find(XORSTRING("BP_PlayButton")) != string::npos)
	{
		if (Globals::EngineVersionString.find("4.16") != string::npos) {
			ClassInstance.OpenAthenaTerrain();
			Globals::bPressedPlay = true;
		}
	}
	if (B->GetObjectNameString().find(XORSTRING("ServerPlayEmoteItem")) != string::npos && bLoadingScreenDropped)
	{
		if (Globals::EngineVersionString.find("4.16") == string::npos) {
			ClassInstance.HandleEmoting(C);
		}
	}
	if (B->GetObjectNameString().find(XORSTRING("ServerBeginEditingBuildingActor")) != string::npos && bLoadingScreenDropped)
	{
		ClassInstance.HandleServerBeginEditActor(C);
	}
	if (B->GetObjectNameString().find(XORSTRING("ServerEditBuildingActor")) != string::npos && bLoadingScreenDropped)
	{
		ClassInstance.HandleServerFinishEditingBuildingActor(C);
	}

	return Globals::ProcessEvent(A, B, C);
}


void BuildExec(__int64 A, __int64* B, __int64 C)
{
	Globals::BuildingOffset = A;

	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	DetourDetach(&(void*&)Globals::Build, BuildExec);

	DetourTransactionCommit();
}

#if defined (SERVERCODE)
void WelcomePlayerHook(UObject* World, UObject* Connection)
{
	printf(XORSTRING("Welcome player was called with connection: %s, world: %s\n"), Connection->GetObjectNameString().c_str(), GetWorld()->GetObjectNameString().c_str());
	return Globals::WelcomePlayer(GetWorld(), Connection);
}

void BeaconNotifyControlMessageHook(UObject* BeaconHost, UObject* Connection, uint8_t MessageType, class Globals::FInBunch& Bunch)
{
	printf(XORSTRING("WorldNotifyControlMessage called. World: %s, Connection: %s, MessageType: %d\n"), GetWorld()->GetObjectNameString().c_str(), Connection->GetObjectNameString().c_str(), MessageType);
	return Globals::WorldNotifyControlMessage(GetWorld(), Connection, MessageType, Bunch);
}

UObject* SpawnPlayActorHook(UObject* World, UObject* NewPlayer, uint8_t RemoteRole, FURL& InURL, FUniqueNetIdRepl& UniqueId, FString& Error, uint8_t InNetPlayerIndex)
{
	printf(XORSTRING("SpawnPlayActor called. NewPlayer: %s\n"), NewPlayer->GetFullName().c_str());
	return Globals::SpawnPlayActor(GetWorld(), NewPlayer, RemoteRole, InURL, UniqueId, Error, InNetPlayerIndex);
}

void* SeamlessTravelHandlerHook(UObject* Engine, UObject* World)
{
	return Globals::SeamlessTravelHandlerForWorld(Engine, GetWorld());
}

char NotifyAcceptingConnectionHook(UObject* BeaconHost)
{
	auto result = Globals::WorldNotifyAcceptingConnection(GetWorld());
	printf(XORSTRING("NotifyAcceptingConnection called. Beacon: %s. World: %s. RESULT: %d\n"), BeaconHost->GetObjectNameString().c_str(), GetWorld()->GetObjectNameString().c_str(), result);
	return result;
}

// THESE 2 FUNCTIONS CAN't BE HOOKED BACK WITHOUT A CRASH

void NotifyAcceptedConnectionHook(UObject* beacon, UObject* Connection)
{
	printf(XORSTRING("NotifyAcceptedConnection called. beacon: %s, world: %s, Connection: %s\n"), beacon->GetObjectNameString().c_str(), GetWorld()->GetObjectNameString().c_str(), Connection->GetObjectNameString().c_str());
}

bool BeaconNotifyAcceptingChannelHook(UObject* Beacon, UObject* Channel)
{
	printf(XORSTRING("BeaconNotifyAcceptingChannel called. beacon: %s, world: %s, Channel: %s\n"), Beacon->GetObjectNameString().c_str(), GetWorld()->GetObjectNameString().c_str(), Channel->GetObjectNameString().c_str());
}

void TickFlushNetDriverHook(UObject* NetDriver, float DeltaSeconds)
{
	Globals::TickFlushNetDriver(NetDriver, DeltaSeconds);

	static UObject* StaticNetDriverClass = UObject::GetObjectFromName(XORSTRING("Class OnlineSubsystemUtils.IpNetDriver"));

	if (NetDriver->IsA(StaticNetDriverClass)) {
		Server::ServerReplicateActors(NetDriver, DeltaSeconds);
	}


}
#endif

void InitializeHooks()
{
	Globals::Build = decltype(Globals::Build)(FindPattern(XORSTRING("48 89 5C 24 ? 57 48 83 EC 30 48 8B FA 48 8B D9 48 83 E9 80 49 8B D0")));

	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	DetourAttach(&(void*&)Globals::ProcessEvent, ProcessEventHook);
	DetourAttach(&(void*&)Globals::Build, BuildExec);

#if defined (SERVERCODE)
	DetourAttach(&(void*&)Globals::WelcomePlayer, WelcomePlayerHook);
	DetourAttach(&(void*&)Globals::BeaconNotifyControlMessage, BeaconNotifyControlMessageHook);
	DetourAttach(&(void*&)Globals::SpawnPlayActor, SpawnPlayActorHook);
	DetourAttach(&(void*&)Globals::SeamlessTravelHandlerForWorld, SeamlessTravelHandlerHook);
	DetourAttach(&(void*&)Globals::BeaconNotifyAcceptingConnection, NotifyAcceptingConnectionHook);
	DetourAttach(&(void*&)Globals::BeaconNotifyAcceptedConnection, NotifyAcceptedConnectionHook);
	DetourAttach(&(void*&)Globals::BeaconNotifyAcceptingChannel, BeaconNotifyAcceptingChannelHook);
	DetourAttach(&(void*&)Globals::TickFlushNetDriver, TickFlushNetDriverHook);
#endif 

	DetourTransactionCommit();
}




#endif // !PROCESSEVENT_H

