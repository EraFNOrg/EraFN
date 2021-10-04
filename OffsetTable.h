/*EraFN Copyright (C) 2021 Daniele "danii" Giompaolo

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.*/

#ifndef OFFSETTABLE_H
#define OFFSETTABLE_H

//FOR PUBLIC RELEASES JUST BUILD ERA WITH THAT 
#define RELEASEVERSION

//TO STRIP OUT FROM PUBLIC RELEASES
//#define TESTINGVERSION
//#define CUSTOMCHEATSCRIPTS
//#define SERVERCODE

#include <Windows.h>
#include "LeoSpecial.h"
#include "UnrealEngineStructs.h"
#include "source/duktape.h"

uintptr_t base_address = (uintptr_t)GetModuleHandle(NULL);

struct Universal
{
	uintptr_t addr_GetEngineVersion;
	LeoHook CURLhook;
	uintptr_t CurlPattern;
	uintptr_t CurlPatternSet;
	int CurrentVersion;
	void* Result;
};

namespace OffsetTable {
	int GetObjectName = 0;
	PBYTE ProcessEvent = 0;
	int UEngine = 0;
	int ViewPort = 0;
	int World = 0;
	int GameInstance = 0;
	int LocalPlayers = 0;
	int PlayerController = 0;
	int GameMode = 0;
	int GameState = 0;
	int MinimapBrush = 0;
	int MinimapSecondBrush = 0;
	int MinimapThirdBrush = 0;
	int MinimapFourthBrush = 0;
	int MinimapFifthBrush = 0;
	int MinimapSixthBrush = 0;
	int MinimapSeventhBrush = 0;
	int CharacterParts = 0;
	int AdditionalData = 0;
	int SprintingBool = 0;
	int MovementStyle = 0;
	int Inventory = 0;
	int QuickBar = 0;
	int ItemEntries = 0;
	int ItemInstances = 0;
	int ItemEntry = 0;
	int PlayerState = 0;
	int FortInventory = 0;
	int PersistentLevel = 0;
	int WorldSettings = 0;
	int AbilitySystemComponent = 0;
	int ViewportConsole = 0;
	int RoleOffset = 0;
	int bAlreadySearchedOffset = 0;
	int PrimaryPickupItemEntry = 0;
	int ItemEntryDefinition = 0;
	int ItemEntryGuid = 0;
	int PrimaryQuickbar = 0;
	int Slots = 0;
	int CurrentFocusedSlot = 0;
	int CurrentBuildableClassOffset = 0;
	int BuildPreviewMarker = 0;
	int LastBuildLocationOffset = 0;
	int LastBuildRotationOffset = 0;
	int ItemEntryCount = 0;
	int MultipleItemEntries = 0;
	int CustomizationLoadout = 0;
	int bIsOutsideSafezone = 0;
	int GrantedAbilitiesOffset = 0;
	int DurationPolicy = 0;
	int WeaponData = 0;
	int FortWeaponGuid = 0;
	int EditActorOffset = 0;
	int bIsCurrentlyBeingEdited = 0;
	int EditBuildingActorOffset = 0;
	int EditModeSupport = 0;
	int PreviewComponent = 0;
	int NumItemsToDropRange = 0;
	int LootTierGroup = 0;
	int AmmoData = 0;
	int AmmoData416 = 0;
	int EmoteAsset = 0;
	int TossedFromContainer = 0;
	int LootPackages = 0;
	int ItemDefinitionLoot = 0;
	int ItemDefinitionLoot416 = 0;
	int CountLoot = 0;
	int Weight = 0;
	int StrongMyHero = 0;
	int bPlayDestructionEffects = 0;
	PBYTE SpawnActor = 0;

	//EGS
#ifdef SERVERCODE
	int ClientConnectionsOffset = 0;
	int NetDriverWorld = 0;
	int ReplicationDriverOffset = 0;
	int OwningActor = 0;
	int ConnectionDriver = 0;
	int NetDriverTime = 0;
	int LastReceiveTime = 0;
	int ConnectionViewTarget = 0;
	int ChildrenConnection = 0;
	int bIsActorBeingDestroyed = 0;
	int ActorRemoteRole = 0;
	int ActorNetDriverName = 0;
	int NetDriverName = 0;
	int WorldTimeSeconds = 0; // need to do some reversing to get the offset
	int NetUpdateFrequency = 0;
	int MinNetUpdateFrequency = 0;
	int NetDriverNetObjects = 0; // need to confirm this one too
	int ActorNetDormancy = 0;
	int LevelOwningWorld = 0;
	int SpawnPrioritySeconds = 0;
#endif 
}

namespace Globals
{
	UObject* PlayerController;
	UObject* World;
	UObject* PlayerPawn;
	UObject* GameMode;
	UObject* GameState;
	UObject* CheatManager;
	UObject* PlayerState;
	UObject* GameplayStatics;
	UObject* Quickbar;
	UObject* Inventory;
	UObject* InventoryContext; 
	UObject* PersistentLevel;
	UObject* WorldSettings;
	UObject* AbilitySystemComponent;
	UObject* Viewport;
	UObject* AmmoBoxSound;
	UObject* ChestSound;
	UObject* BuildingActorLast;
	UObject* LastClass;
	UObject* PickaxeItemDefinition;

	//dynamic globals
	UObject* CurrentEmote;
	FVector EmotePosition;

	//Building globals
	UObject* WallPreview;
	UObject* FloorPreview;
	UObject* StairsPreview;
	UObject* RoofPreview;

	//edit
	UObject* EditTool;
	FGuid EditToolGUID;
	UObject* PreviewComponent;
	FVector ComponentLocation;
	FRotator ComponentRotation;

	//EraScript
	duk_context* DukContext;

	//Gameplay abilities
	UObject* DefaultGameplayEffect;
	UObject* GameplayEffectClass;

	//looting
	TArray<UObject*> GItemDefs;
	UObject* DataTableLibrary;
	UObject* AthenaLoot;
	TArray<UObject*> AmmoItemDefs;
	TArray<UObject*> WeaponItemDefs;
	TArray<UObject*> ConsumablesItemDefs;
	TArray<UObject*> TrapsItemDefs;
	TArray<UObject*> ResourceItemDefs;
	TArray<int> AmmoItemCount;
	TArray<int> WeaponItemCount;
	TArray<int> ConsumablesItemCount;
	TArray<int> TrapsItemCount;
	TArray<int> ResourceItemCount;
	UObject* LastItemDefinition;
	UObject* CurrentPlaylist;
	bool bPreparedLootArrays = false;

	//libs 
	UObject* kismetMathLib;
	UObject* HudContext;

	//skins
	UObject* Body;
	UObject* Head;
	UObject* Hat;
	UObject* BackPack;
	bool bInitializedSkins;

	bool bCanBuild;
	bool _bCanBuild;
	__int64 BuildingOffset;
	void (*Build)(__int64 A, __int64* B, __int64 C) = nullptr;
	void*(*ProcessEvent)(UObject* A, UObject* B, PVOID C) = nullptr;
	void* (*FindRowUnchecked)(UObject* DataTable, FName RowName, bool A) = nullptr;
	static UObject* (*StaticLoadObject)(UObject* ObjectClass, UObject* InOuter, const TCHAR* InName, const TCHAR* Filename, uint32_t LoadFlags, UObject* Sandbox, bool bAllowObjectReconciliation);
	bool bPressedPlay = false;
	bool bJumped = false;
	bool bJumpedFromBus = false;
	bool DroppedLS = false;
	bool bIsEmoting = false;
	bool OutsideSafezone = false;\
	EEngineVersion CurrentEngineVersion;
	string CurrentFortniteVersion;
	string EngineVersionString;

	//EGS
#if defined(SERVERCODE)
	bool bIsListening;

	UObject* Beacon;

	inline bool (*InitHost)(UObject* beacon);
	inline void (*PauseBeaconRequests)(UObject* Beacon, bool bPause);
	inline void (*WelcomePlayer)(UObject* World, UObject* Connection);
	inline void (*BeaconNotifyControlMessage)(UObject* OnlineBeaconHost, UObject* Connection, uint8_t MessageType, class FInBunch& InBunch);
	inline void (*WorldNotifyControlMessage)(UObject* World, UObject* Connection, uint8_t MessageType, class FInBunch& Bunch);
	inline UObject* (*SpawnPlayActor)(UObject* World, UObject* NewPlayer, uint8_t RemoteRole, FURL& InURL, FUniqueNetIdRepl& UniqueId, FString& Error, uint8_t InNetPlayerIndex);
	inline void* (*SeamlessTravelHandlerForWorld)(UObject* Engine, UObject* World);
	inline char (*BeaconNotifyAcceptingConnection)(UObject* beacon);
	inline char (*WorldNotifyAcceptingConnection)(UObject* world);
	inline void (*BeaconNotifyAcceptedConnection)(UObject* beacon, UObject* Connection);
	inline void (*WorldNotifyAcceptedConnection)(UObject* world, UObject* Connection);
	inline bool (*BeaconNotifyAcceptingChannel)(UObject* Beacon, UObject* Channel);
	inline bool (*WorldNotifyAcceptingChannel)(UObject* world, UObject* Channel);
	inline void (*TickFlushNetDriver)(UObject* NetDriver, float DeltaSeconds);
	inline void (*ActorCallPreReplication)(AActor*, UObject*);
	inline bool (*ActorIsNetStartupActor)(AActor*);
	inline void (*RemoveNetworkActor)(UObject*, AActor*);
#endif
}

//Needed to avoid freezes
namespace classes
{
	UObject* PlayerPawnClass;
}

auto UniversalStructInstance = new Universal;

#endif // !OFFSETTABLE_H