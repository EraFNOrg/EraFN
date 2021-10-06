/*EraFN Copyright (C) 2021 Daniele "danii" Giompaolo

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.*/

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <Windows.h>
#include "UnrealEngineStructs.h"
#include "OffsetInitialize.h"
#include "OffsetTable.h"
#include "GameFunctions.h"
#include "Script.h"
#include <thread>

class GameManager
{
public:

	void InitializeGlobals()
	{
		OffsetTable::ViewPort = UObject::FindOffset(XORSTRING("ObjectProperty Engine.Engine.GameViewport"));
		OffsetTable::World = UObject::FindOffset(XORSTRING("ObjectProperty Engine.GameViewportClient.World"));
		OffsetTable::GameInstance = UObject::FindOffset(XORSTRING("ObjectProperty Engine.GameViewportClient.GameInstance"));
		OffsetTable::LocalPlayers = UObject::FindOffset(XORSTRING("ArrayProperty Engine.GameInstance.LocalPlayers"));
		OffsetTable::PlayerController = UObject::FindOffset(XORSTRING("ObjectProperty Engine.Player.PlayerController"));
		OffsetTable::GameMode = UObject::FindOffset(XORSTRING("ObjectProperty Engine.World.AuthorityGameMode"));
		OffsetTable::GameState = UObject::FindOffset(XORSTRING("ObjectProperty Engine.World.GameState"));
		OffsetTable::StrongMyHero = UObject::FindOffset(XORSTRING("ObjectProperty FortniteGame.FortPlayerControllerAthena.StrongMyHero"));
		OffsetTable::CharacterParts = UObject::FindOffset(XORSTRING("ArrayProperty FortniteGame.FortHero.CharacterParts"));
		OffsetTable::AdditionalData = UObject::FindOffset(XORSTRING("ObjectProperty FortniteGame.CustomCharacterPart.AdditionalData"));
		OffsetTable::PlayerState = UObject::FindOffset(XORSTRING("ObjectProperty Engine.Controller.PlayerState"));
		OffsetTable::PawnPlayerState = UObject::FindOffset(XORSTRING("ObjectProperty Engine.Pawn.PlayerState"));
		OffsetTable::Inventory = UObject::FindOffset(XORSTRING("ObjectProperty FortniteGame.FortPlayerController.WorldInventory"));
		OffsetTable::QuickBar = UObject::FindOffset(XORSTRING("ObjectProperty FortniteGame.FortPlayerController.QuickBars"));
		OffsetTable::ItemInstances = UObject::FindOffset(XORSTRING("ArrayProperty FortniteGame.FortItemList.ItemInstances"));
		OffsetTable::ItemEntries = UObject::FindOffset(XORSTRING("ArrayProperty FortniteGame.FortItemList.ReplicatedEntries"));
		OffsetTable::FortInventory = UObject::FindOffset(XORSTRING("StructProperty FortniteGame.FortInventory.Inventory"));
		OffsetTable::ItemEntry = UObject::FindOffset(XORSTRING("StructProperty FortniteGame.FortWorldItem.ItemEntry"));
		OffsetTable::PersistentLevel = UObject::FindOffset(XORSTRING("ObjectProperty Engine.World.PersistentLevel"));
		OffsetTable::WorldSettings = UObject::FindOffset(XORSTRING("ObjectProperty Engine.Level.WorldSettings"));
		OffsetTable::AbilitySystemComponent = UObject::FindOffset(XORSTRING("ObjectProperty FortniteGame.FortPawn.AbilitySystemComponent"));
		OffsetTable::ViewportConsole = UObject::FindOffset(XORSTRING("ObjectProperty Engine.GameViewportClient.ViewportConsole"));
		OffsetTable::RoleOffset = UObject::FindOffset(XORSTRING("ByteProperty Engine.Actor.Role"));
		OffsetTable::bAlreadySearchedOffset = UObject::FindOffset(XORSTRING("BoolProperty FortniteGame.BuildingContainer.bAlreadySearched"));
		OffsetTable::PrimaryPickupItemEntry = UObject::FindOffset(XORSTRING("StructProperty FortniteGame.FortPickup.PrimaryPickupItemEntry"));
		OffsetTable::ItemEntryDefinition = UObject::FindOffset(XORSTRING("ObjectProperty FortniteGame.FortItemEntry.ItemDefinition"));
		OffsetTable::PrimaryQuickbar = UObject::FindOffset(XORSTRING("StructProperty FortniteGame.FortQuickBars.PrimaryQuickBar"));
		OffsetTable::Slots = UObject::FindOffset(XORSTRING("ArrayProperty FortniteGame.QuickBar.Slots"));
		OffsetTable::ItemEntryGuid = UObject::FindOffset(XORSTRING("StructProperty FortniteGame.FortItemEntry.ItemGuid"));
		OffsetTable::ItemEntryCount = UObject::FindOffset(XORSTRING("IntProperty FortniteGame.FortItemEntry.Count"));
		OffsetTable::CurrentFocusedSlot = UObject::FindOffset(XORSTRING("IntProperty FortniteGame.QuickBar.CurrentFocusedSlot"));
		OffsetTable::CurrentBuildableClassOffset = UObject::FindOffset(XORSTRING("ClassProperty FortniteGame.FortPlayerController.CurrentBuildableClass"));
		OffsetTable::BuildPreviewMarker = UObject::FindOffset(XORSTRING("ObjectProperty FortniteGame.FortPlayerController.BuildPreviewMarker"));
		OffsetTable::LastBuildLocationOffset = UObject::FindOffset(XORSTRING("StructProperty FortniteGame.FortPlayerController.LastBuildPreviewGridSnapLoc"));
		OffsetTable::LastBuildRotationOffset = UObject::FindOffset(XORSTRING("StructProperty FortniteGame.FortPlayerController.LastBuildPreviewGridSnapRot"));
		OffsetTable::MultipleItemEntries = UObject::FindOffset(XORSTRING("ArrayProperty FortniteGame.FortPickup.MultiItemPickupEntries"));
		OffsetTable::CustomizationLoadout = UObject::FindOffset(XORSTRING("StructProperty FortniteGame.FortPlayerControllerAthena.CustomizationLoadout"));
		OffsetTable::bIsOutsideSafezone = UObject::FindOffset(XORSTRING("BoolProperty FortniteGame.FortPlayerPawnAthena.bIsOutsideSafeZone"));
		OffsetTable::GrantedAbilitiesOffset = UObject::FindOffset(XORSTRING("ArrayProperty GameplayAbilities.GameplayEffect.GrantedAbilities"));
		OffsetTable::DurationPolicy = UObject::FindOffset(XORSTRING("EnumProperty GameplayAbilities.GameplayEffect.DurationPolicy"));
		OffsetTable::WeaponData = UObject::FindOffset(XORSTRING("ObjectProperty FortniteGame.FortWeapon.WeaponData"));
		OffsetTable::FortWeaponGuid = UObject::FindOffset(XORSTRING("StructProperty FortniteGame.FortWeapon.ItemEntryGuid"));
		OffsetTable::EditActorOffset = UObject::FindOffset(XORSTRING("ObjectProperty FortniteGame.FortWeap_EditingTool.EditActor"));
		OffsetTable::bIsCurrentlyBeingEdited = UObject::FindOffset(XORSTRING("BoolProperty FortniteGame.BuildingSMActor.bCurrentlyBeingEdited"));
		OffsetTable::EditBuildingActorOffset = UObject::FindOffset(XORSTRING("ObjectProperty FortniteGame.FortPlayerController.EditBuildingActor"));
		OffsetTable::EditModeSupport = UObject::FindOffset(XORSTRING("ObjectProperty FortniteGame.BuildingSMActor.EditModeSupport"));
		OffsetTable::PreviewComponent = UObject::FindOffset(XORSTRING("ObjectProperty FortniteGame.BuildingEditModeSupport.PreviewComponent"));
		OffsetTable::NumItemsToDropRange = UObject::FindOffset(XORSTRING("StructProperty FortniteGame.BuildingContainer.NumItemsToDropRange"));
		OffsetTable::LootTierGroup = UObject::FindOffset(XORSTRING("NameProperty FortniteGame.BuildingContainer.SearchLootTierGroup"));
		OffsetTable::AmmoData = UObject::FindOffset(XORSTRING("SoftObjectProperty FortniteGame.FortWeaponItemDefinition.AmmoData"));
		OffsetTable::AmmoData416 = UObject::FindOffset(XORSTRING("AssetObjectProperty FortniteGame.FortWeaponItemDefinition.AmmoData"));
		OffsetTable::EmoteAsset = UObject::FindOffset(XORSTRING("SoftObjectProperty FortniteGame.FortMontageItemDefinitionBase.Animation"));
		OffsetTable::TossedFromContainer = UObject::FindOffset(XORSTRING("BoolProperty FortniteGame.FortPickup.bTossedFromContainer"));
		OffsetTable::LootPackages = UObject::FindOffset(XORSTRING("SoftObjectProperty FortniteGame.FortPlaylist.LootPackages"));
		OffsetTable::ItemDefinitionLoot = UObject::FindOffset(XORSTRING("SoftObjectProperty FortniteGame.FortLootPackageData.ItemDefinition"));
		OffsetTable::ItemDefinitionLoot416 = UObject::FindOffset(XORSTRING("AssetObjectProperty FortniteGame.FortLootPackageData.ItemDefinition"));
		OffsetTable::CountLoot = UObject::FindOffset(XORSTRING("IntProperty FortniteGame.FortLootPackageData.Count"));
		OffsetTable::Weight = UObject::FindOffset(XORSTRING("FloatProperty FortniteGame.FortLootPackageData.Weight"));
		OffsetTable::bPlayDestructionEffects = UObject::FindOffset(XORSTRING("BoolProperty FortniteGame.BuildingSMActor.bPlayDestructionEffects"));
#ifdef SERVERCODE
		OffsetTable::ClientConnectionsOffset = UObject::FindOffset(XORSTRING("ArrayProperty Engine.NetDriver.ClientConnections"));
		OffsetTable::NetDriverWorld = UObject::FindOffset(XORSTRING("ObjectProperty Engine.NetDriver.World"));
		OffsetTable::ReplicationDriverOffset = UObject::FindOffset(XORSTRING("ObjectProperty Engine.NetDriver.ReplicationDriver"));
		OffsetTable::OwningActor = UObject::FindOffset(XORSTRING("ObjectProperty Engine.NetConnection.OwningActor"));
		OffsetTable::ConnectionDriver = UObject::FindOffset(XORSTRING("ObjectProperty Engine.NetConnection.Driver"));
		OffsetTable::NetDriverTime = UObject::FindOffset(XORSTRING("FloatProperty Engine.NetDriver.Time"));
		OffsetTable::LastReceiveTime = UObject::FindOffset(XORSTRING("DoubleProperty Engine.NetConnection.LastReceiveTime"));
		OffsetTable::ConnectionViewTarget = UObject::FindOffset(XORSTRING("ObjectProperty Engine.NetConnection.ViewTarget"));
		OffsetTable::ChildrenConnection = UObject::FindOffset(XORSTRING("ArrayProperty Engine.NetConnection.Children"));
		OffsetTable::bIsActorBeingDestroyed = UObject::FindOffset(XORSTRING("BoolProperty Engine.Actor.bActorIsBeingDestroyed"));
		OffsetTable::ActorRemoteRole = UObject::FindOffset(XORSTRING("ByteProperty Engine.Actor.RemoteRole"));
		OffsetTable::ActorNetDriverName = UObject::FindOffset(XORSTRING("NameProperty Engine.Actor.NetDriverName"));
		OffsetTable::NetDriverName = UObject::FindOffset(XORSTRING("NameProperty Engine.NetDriver.NetDriverName"));
		// one's missing here
		OffsetTable::NetUpdateFrequency = UObject::FindOffset(XORSTRING("FloatProperty Engine.Actor.NetUpdateFrequency"));
		OffsetTable::MinNetUpdateFrequency = UObject::FindOffset(XORSTRING("FloatProperty Engine.Actor.MinNetUpdateFrequency"));
		OffsetTable::NetDriverNetObjects = UObject::FindOffset(XORSTRING("ObjectProperty Engine.NetDriver.ReplicationDriver")) + 8;
		OffsetTable::ActorNetDormancy = UObject::FindOffset(XORSTRING("ByteProperty Engine.Actor.NetDormancy"));
		OffsetTable::LevelOwningWorld = UObject::FindOffset(XORSTRING("ObjectProperty Engine.Level.OwningWorld"));
		OffsetTable::SpawnPrioritySeconds = UObject::FindOffset(XORSTRING("FloatProperty Engine.NetDriver.SpawnPrioritySeconds"));
#endif // SERVERCODE 

		UObject* Engine;
		Engine = UObject::GetObjectFromName(XORSTRING("FortEngine Transient.FortEngine_"));
		Globals::Viewport = *reinterpret_cast<UObject**>(reinterpret_cast<uintptr_t>(Engine) + OffsetTable::ViewPort);
		auto GameInstance = *reinterpret_cast<UObject**>(reinterpret_cast<uintptr_t>(Globals::Viewport) + OffsetTable::GameInstance);
		auto LocalPlayers = *reinterpret_cast<TArray<UObject*>*>(reinterpret_cast<uintptr_t>(GameInstance) + OffsetTable::LocalPlayers);
		if (LocalPlayers[0]) Globals::PlayerController = *reinterpret_cast<UObject**>(reinterpret_cast<uintptr_t>(LocalPlayers[0]) + OffsetTable::PlayerController);
		if (Globals::Viewport) Globals::World = *reinterpret_cast<UObject**>(reinterpret_cast<uintptr_t>(Globals::Viewport) + OffsetTable::World);
		if (Globals::World) Globals::PersistentLevel = *reinterpret_cast<UObject**>(reinterpret_cast<uintptr_t>(Globals::World) + OffsetTable::PersistentLevel);
		if (Globals::PersistentLevel) Globals::WorldSettings = *reinterpret_cast<UObject**>(reinterpret_cast<uintptr_t>(Globals::PersistentLevel) + OffsetTable::WorldSettings);
		if (Globals::World) Globals::GameMode = *reinterpret_cast<UObject**>(reinterpret_cast<uintptr_t>(Globals::World) + OffsetTable::GameMode);
		if (Globals::World) Globals::GameState = *reinterpret_cast<UObject**>(reinterpret_cast<uintptr_t>(Globals::World) + OffsetTable::GameState);
		if (Globals::PlayerController) Globals::PlayerState = *reinterpret_cast<UObject**>(reinterpret_cast<uintptr_t>(Globals::PlayerController) + OffsetTable::PlayerState);

		void* Result;
		string Receive = EngineVersion::GetVersion(&Result);
		Globals::CurrentFortniteVersion = Receive;
	}

	void OpenAthenaTerrain()
	{
		SwitchLevel();
	}

	void Init()
	{
		InitializeGlobals();
		InitializeObjects();

#if defined(RELEASEVERSION)
		Globals::PlayerPawn = SpawnActorFromClass(XORSTRING("BlueprintGeneratedClass PlayerPawn_Athena.PlayerPawn_Athena_C"), FVector{ 0, 0, 5000 });
#elif defined(TESTINGVERSION)
		Globals::PlayerPawn = SpawnActorFromClass(XORSTRING("BlueprintGeneratedClass PlayerPawn_Athena.PlayerPawn_Athena_C"), FVector{ 0, 0, 1000 });
#endif
		Possess();
		MiniMap();
		DestroyLods();
		SetName();
		CustomizationLoadout();
		SetupBuildingPreviews();
		InitializeClasses();
		SetupItemDefinitions();

		DropLoadingScreen();
		EraScript::Init();
	}

	void OnLoadingScreenDropped()
	{
		EquipSkin();
		EquipPickaxe();

		PrepareArray();

		SetupPositioning();
		thread thread_inventory (Inventory);
		thread thread_pickup(SpawnPickupsAthena_Terrain);

		HideNetDebugUI();
		GrantDefaultAbilities();
		ApplyBattleBus();
		CustomizationLoadout();
		StartSkydiving();
		StartListening();
		ToggleGodMode();

		thread_pickup.join();
		thread_inventory.join();

		EquipWeapon(GetDefinition(GetQuickbarItem(EFortQuickBars::Primary, 0)), GetGuid(GetQuickbarItem(EFortQuickBars::Primary, 0)));

		Globals::DroppedLS = true;
	}

	void EquipQuickbarItem(EFortQuickBars QuickbarIndex, int Slot)
	{
		EquipWeapon(GetDefinition(GetQuickbarItem(QuickbarIndex, Slot)), GetGuid(GetQuickbarItem(QuickbarIndex, Slot)));
	}

	void JumpFromAircraft()
	{
#if defined(RELEASEVERSION)
		Summon(XORSTRING(TEXT("PlayerPawn_Athena_C")));
		Globals::PlayerPawn = FindActorsFromClass(classes::PlayerPawnClass);
		Possess();
		AdjustRotation();
		CustomizationLoadout();
		EquipSkin();
		EquipWeapon(GetDefinition(GetQuickbarItem(EFortQuickBars::Primary, 0)), GetGuid(GetQuickbarItem(EFortQuickBars::Primary, 0)));
		ToggleGodMode();
#endif
	}

	void Build()
	{
		auto CurrentBuildableClass = *reinterpret_cast<UObject**>(__int64(Globals::PlayerController) + OffsetTable::CurrentBuildableClassOffset);
		auto LastPreviewLocation = *reinterpret_cast<FVector*>(__int64(Globals::PlayerController) + OffsetTable::LastBuildLocationOffset);
		auto LastPreviewRotation = *reinterpret_cast<FRotator*>(__int64(Globals::PlayerController) + OffsetTable::LastBuildRotationOffset);
		auto BuildingActor = SpawnActorFromClassObj(CurrentBuildableClass, LastPreviewLocation, LastPreviewRotation);
		InitializeBuildingActor(BuildingActor);
	}

	void Tick()
	{
#ifndef SERVERCODE
		UObject* EditBuildingActor = *(UObject**)(__int64(Globals::PlayerController) + OffsetTable::EditBuildingActorOffset);
		if (!IsBadReadPtr(EditBuildingActor) && EditBuildingActor) {
			UObject* EditModeSupport = *(UObject**)(__int64(EditBuildingActor) + OffsetTable::EditModeSupport);
			Globals::PreviewComponent = *(UObject**)(__int64(EditModeSupport) + OffsetTable::PreviewComponent);
			Globals::ComponentLocation = GetComponentLocation(Globals::PreviewComponent);
			Globals::ComponentRotation = GetComponentRotation(Globals::PreviewComponent);
		}

		if (Globals::CurrentEngineVersion >= EEngineVersion::UE_4_21 && Globals::bJumpedFromBus)
		{
			bool bIsInVehicle = IsInVehicle();
			*reinterpret_cast<ENetRole*>(__int64(Globals::PlayerPawn) + OffsetTable::RoleOffset) = (bIsInVehicle ? ENetRole::ROLE_AutonomousProxy : ENetRole::ROLE_Authority);

			if (bIsInVehicle) {
				auto CurrentVehicle = GetVehicle();

				if (CurrentVehicle)
				{
					*reinterpret_cast<ENetRole*>(__int64(CurrentVehicle) + OffsetTable::RoleOffset) = ENetRole::ROLE_AutonomousProxy;
				}
			}
		}

		if (IsInBuildMode() && !strstr(Globals::CurrentFortniteVersion.c_str(), "3.") &&
			(Globals::EngineVersionString.find("4.16") == string::npos &&
				Globals::EngineVersionString.find("4.19") == string::npos) &&
			!bIsInEditMode())
		{
			if (Globals::BuildingOffset != 0)
			{
				Globals::bCanBuild = *reinterpret_cast<bool*>(Globals::BuildingOffset + 0x20);
				Globals::_bCanBuild = *reinterpret_cast<bool*>(Globals::BuildingOffset + 0x28);

				if (Globals::DroppedLS && GetAsyncKeyState(VK_LBUTTON) & 0x8000 && Globals::_bCanBuild && !Globals::bCanBuild)
				{
					auto CurrentBuildableClass = *reinterpret_cast<UObject**>(__int64(Globals::PlayerController) + OffsetTable::CurrentBuildableClassOffset);
					auto LastPreviewLocation = *reinterpret_cast<FVector*>(__int64(Globals::PlayerController) + OffsetTable::LastBuildLocationOffset);
					auto LastPreviewRotation = *reinterpret_cast<FRotator*>(__int64(Globals::PlayerController) + OffsetTable::LastBuildRotationOffset);

					if (Globals::BuildingActorLast && Globals::LastClass && !IsBadReadPtr(Globals::BuildingActorLast) && !IsBadReadPtr(Globals::LastClass))
					{
						auto CurrentLoc = GetLocation(Globals::BuildingActorLast);
						if (!IsBadReadPtr(&CurrentLoc.X) && !IsBadReadPtr(&CurrentLoc.Z) && !IsBadReadPtr(&CurrentLoc.Z) && !IsBadReadPtr(&LastPreviewLocation.X) && !IsBadReadPtr(&LastPreviewLocation.Z) && !IsBadReadPtr(&LastPreviewLocation.Z) && !IsBadReadPtr(CurrentBuildableClass))
						{
							if (CurrentLoc.X == LastPreviewLocation.X && CurrentLoc.Y == LastPreviewLocation.Y && CurrentLoc.Z == LastPreviewLocation.Z && Globals::LastClass == CurrentBuildableClass)
							{
								return;
							}
						}
					}

					auto BuildingActor = SpawnActorFromClassObj(CurrentBuildableClass, LastPreviewLocation, LastPreviewRotation);
					Globals::BuildingActorLast = BuildingActor;
					Globals::LastClass = CurrentBuildableClass;
					InitializeBuildingActor(BuildingActor);
				}
			}
		}

		if (Globals::bIsEmoting)
		{
			auto CurrentLocation = GetLocation(Globals::PlayerPawn);

			if (CurrentLocation.X != Globals::EmotePosition.X ||
				CurrentLocation.Y != Globals::EmotePosition.Y ||
				CurrentLocation.Z != Globals::EmotePosition.Z)
			{
				Globals::bIsEmoting = false;
				if (Globals::CurrentEmote) StopMontage(Globals::CurrentEmote);
				Globals::CurrentEmote = nullptr;
			}
		}
#endif
	}

	void ExitVehicle()
	{
		UObject* Vehicle = GetVehicle();

		*reinterpret_cast<ENetRole*>(__int64(Globals::PlayerPawn) + __int64(OffsetTable::RoleOffset)) = ENetRole::ROLE_Authority;
		*reinterpret_cast<ENetRole*>(__int64(Vehicle) + __int64(OffsetTable::RoleOffset)) = ENetRole::ROLE_Authority;
	}

	void HandleInitializeConsole()
	{
		InitializeConsole();
	}

	void InteractWithContainer(PVOID Params)
	{
		InteractWith(Params);
	}

	static void EquipInventoryItem(FGuid Guid = {}, UObject* FortWeapon = nullptr)
	{
		Equip(Guid, FortWeapon);
	}

	static void HandleInventoryDrop(void* Params)
	{
		InventoryDrop(Params);
	}

	void HandlePickup(PVOID Params)
	{
		Pickup(Params);
	}

	void ExecuteCheatScript(PVOID Params)
	{
		CheatScript(Params);
	}

	static void HandleEmoting(PVOID Params)
	{
		Emote(Params);
	}

	static void HandleServerBeginEditActor(PVOID Params)
	{
		BeginEditingBuildingActor(Params);
	}

	static void HandleServerFinishEditingBuildingActor(PVOID Params)
	{
		ServerFinishEditingBuildingActor(Params);
	}
};


#endif // !FUNCTIONS_H
