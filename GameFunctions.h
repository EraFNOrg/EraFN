/*EraFN Copyright (C) 2021 Daniele "danii" Giompaolo

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.*/

#ifndef GAMEFUNCTIONS_H
#define GAMEFUNCTIONS_H

#include "UnrealEngineStructs.h"
#include "Internals.h"
#include "OffsetTable.h"
#include <ctime>

UObject* GetWorld()
{
	static auto Engine = UObject::GetObjectFromName(XORSTRING("FortEngine Transient.FortEngine_"));
	Globals::Viewport = *reinterpret_cast<UObject**>(reinterpret_cast<uintptr_t>(Engine) + OffsetTable::ViewPort);
	return Globals::World = *reinterpret_cast<UObject**>(reinterpret_cast<uintptr_t>(Globals::Viewport) + OffsetTable::World);
}

static UObject* SpawnObject(UObject* Class, UObject* Outer)
{
	static auto object = UObject::GetObjectFromName(XORSTRING("GameplayStatics Engine.Default__GameplayStatics"));

	static auto fn = UObject::GetObjectFromName(XORSTRING("Function Engine.GameplayStatics.SpawnObject"));
	
	struct
	{
		UObject* Class;
		UObject* outer;
		UObject* ReturnValue;
	} params;

	params.Class = Class;
	params.outer = Outer;

	Globals::ProcessEvent(object, fn, &params);

	return params.ReturnValue;
}


FVector GetComponentLocation(UObject* Target)
{
	static auto fn = UObject::GetObjectFromName(XORSTRING("Function Engine.SceneComponent.K2_GetComponentLocation"));

	FVector ReturnValue;

	Globals::ProcessEvent(Target, fn, &ReturnValue);

	return ReturnValue;
}

FRotator GetComponentRotation(UObject* Target)
{
	static auto fn = UObject::GetObjectFromName(XORSTRING("Function Engine.SceneComponent.K2_GetComponentRotation"));

	FRotator ReturnValue;

	Globals::ProcessEvent(Target, fn, &ReturnValue);

	return ReturnValue;
}

void SwitchLevel()
{
	static auto SwitchLevel = UObject::GetObjectFromName(XORSTRING("Function Engine.PlayerController.SwitchLevel"));

	struct SwitchLevel_Params
	{
		FString URL;
	};

	SwitchLevel_Params params;
#if defined(RELEASEVERSION)
	params.URL = XORSTRING(L"Athena_Terrain");
#elif defined(TESTINGVERSION)
	params.URL = XORSTRING(L"Athena_Faceoff");
#endif
	Globals::ProcessEvent(Globals::PlayerController, SwitchLevel, &params);
}

UObject* SpawnActorFromClass(string ClassName, FVector pPosition, FRotator Rot = *new FRotator{})
{
	auto Class = UObject::GetObjectFromName(ClassName);
	auto Position = pPosition;
	auto Rotation = Rot;

	SpawnActorParams params{};

	auto Actor = SpawnActor(Globals::World, Class, &Position, &Rotation, params);
	return Actor;
}

UObject* SpawnActorFromClassObj(UObject* _Class, FVector pPosition, FRotator Rot = *new FRotator{})
{
	auto Class = _Class;
	auto Position = pPosition;
	auto Rotation = Rot;

	SpawnActorParams params{};

	auto Actor = SpawnActor(Globals::World, Class, &Position, &Rotation, params);
	return Actor;
}

void Possess(UObject* Controller = Globals::PlayerController, UObject* pawn = Globals::PlayerPawn)
{
	static auto Possess = UObject::GetObjectFromName(XORSTRING("Function Engine.Controller.Possess"));

	struct Possess_Params
	{
		UObject* Pawn;
	};

	Possess_Params params;
	params.Pawn = pawn;

	Globals::ProcessEvent(Controller, Possess, &params);
}

static void SetActorHiddenInGame(UObject* Actor, bool Hidden)
{
	static UObject* SetActorHiddenInGame = UObject::GetObjectFromName(XORSTRING("Function Engine.Actor.SetActorHiddenInGame"));

	Globals::ProcessEvent(Actor, SetActorHiddenInGame, &Hidden);
}

void _Possess(UObject* Target, UObject* Pawn)
{
	static auto Possess = UObject::GetObjectFromName(XORSTRING("Function Engine.Controller.Possess"));
	Globals::ProcessEvent(Target, Possess, &Pawn);
}

static void ShowBuildingFoundation(UObject* BuildingFoundation, EDynamicFoundationType DynamicFoundationType)
{
	if (BuildingFoundation)
	{
		EDynamicFoundationType* CurrentFoundationType = reinterpret_cast<EDynamicFoundationType*>(__int64(BuildingFoundation) + UObject::FindOffset("EnumProperty FortniteGame.BuildingFoundation.DynamicFoundationType"));

		*CurrentFoundationType = DynamicFoundationType;
	}
}

void SetHasFinishedLoading()
{
	*reinterpret_cast<bool*>(__int64(Globals::PlayerController) + UObject::FindOffset("BoolProperty FortniteGame.FortPlayerController.bHasClientFinishedLoading")) = true;
	*reinterpret_cast<bool*>(__int64(Globals::PlayerController) + UObject::FindOffset("BoolProperty FortniteGame.FortPlayerController.bHasServerFinishedLoading")) = true;
}

void DropLoadingScreen()
{
	Globals::ProcessEvent(Globals::PlayerController, UObject::GetObjectFromName(XORSTRING("Function FortniteGame.FortPlayerController.ServerReadyToStartMatch")), nullptr);

#if defined (SERVERCODE)
	EAthenaGamePhase* CurrentGamePhase = reinterpret_cast<EAthenaGamePhase*>(__int64(Globals::GameState) + UObject::FindOffset(XORSTRING("EnumProperty FortniteGame.FortGameStateAthena.GamePhase")));
	*CurrentGamePhase = EAthenaGamePhase::Aircraft;
	auto OldGamePhase = EAthenaGamePhase::None;

	static auto OnRep_GamePhase = UObject::GetObjectFromName(XORSTRING("Function FortniteGame.FortGameStateAthena.OnRep_GamePhase"));
	Globals::ProcessEvent(Globals::GameState, OnRep_GamePhase, &OldGamePhase);
	SetHasFinishedLoading();
#endif 

#ifndef SERVERCODE
	if (UObject::GetObjectFromName(XORSTRING("Function FortniteGame.FortPlayerStateAthena.OnRep_SquadId")))
	{
		Globals::ProcessEvent(Globals::PlayerState, UObject::GetObjectFromName(XORSTRING("Function FortniteGame.FortPlayerStateAthena.OnRep_SquadId")), nullptr);
	}
	else if (UObject::GetObjectFromName(XORSTRING("Function FortniteGame.FortPlayerStateAthena.OnRep_TeamIndex")))
	{
		enum class EFortTeam : uint8_t
		{
			HumanCampaign = 0,
			Monster = 1,
			HumanPvP_Team1 = 2
		};

		*(EFortTeam*)(__int64(Globals::GameState) + UObject::FindOffset(XORSTRING("ByteProperty FortniteGame.FortPlayerStateAthena.TeamIndex"))) = EFortTeam::HumanPvP_Team1;
		Globals::ProcessEvent(Globals::PlayerState, UObject::GetObjectFromName(XORSTRING("Function FortniteGame.FortPlayerStateAthena.OnRep_TeamIndex")), nullptr);
	}

	void* Result;
	string Receive = EngineVersion::GetVersion(&Result);

	EAthenaGamePhase* CurrentGamePhase = reinterpret_cast<EAthenaGamePhase*>(__int64(Globals::GameState) + UObject::FindOffset(XORSTRING("EnumProperty FortniteGame.FortGameStateAthena.GamePhase")));
	*CurrentGamePhase = EAthenaGamePhase::Warmup;
	auto OldGamePhase = EAthenaGamePhase::None;

	static auto OnRep_GamePhase = UObject::GetObjectFromName(XORSTRING("Function FortniteGame.FortGameStateAthena.OnRep_GamePhase"));
	Globals::ProcessEvent(Globals::GameState, OnRep_GamePhase, &OldGamePhase);
	SetHasFinishedLoading();

	
	if (strstr(Receive.c_str(), "3.") || 
		strstr(Receive.c_str(), "4.") ||
		strstr(Receive.c_str(), "5.") ||
		strstr(Receive.c_str(), "6.") ||
		strstr(Receive.c_str(), "7.") ||
		strstr(Receive.c_str(), "8.") ||
		strstr(Receive.c_str(), "9."))
	{
		auto OnRep_CurrentPlaylistData = UObject::GetObjectFromName(XORSTRING("Function FortniteGame.FortGameStateAthena.OnRep_CurrentPlaylistData"));
		auto OnRep_CurrentPlaylistInfo = UObject::GetObjectFromName(XORSTRING("Function FortniteGame.FortGameStateAthena.OnRep_CurrentPlaylistInfo"));
		auto PlayGround = UObject::GetObjectFromName(XORSTRING("FortPlaylistAthena Playlist_Playground.Playlist_Playground"));
		if (PlayGround) Globals::CurrentPlaylist = PlayGround;
		if (OnRep_CurrentPlaylistData)
		{
			if (PlayGround)
			{
				*reinterpret_cast<UObject**>(__int64(Globals::GameState) + UObject::FindOffset(XORSTRING("ObjectProperty FortniteGame.FortGameStateAthena.CurrentPlaylistData"))) = PlayGround;
				Globals::ProcessEvent(Globals::GameState, OnRep_CurrentPlaylistData, nullptr);
			}
		}
		else if (OnRep_CurrentPlaylistInfo)
		{		
			if (PlayGround)
			{
				*reinterpret_cast<UObject**>(__int64(Globals::GameState) + UObject::FindOffset(XORSTRING("StructProperty FortniteGame.FortGameStateAthena.CurrentPlaylistInfo")) + UObject::FindOffset(XORSTRING("ObjectProperty FortniteGame.PlaylistPropertyArray.BasePlaylist"))) = PlayGround;
				*reinterpret_cast<UObject**>(__int64(Globals::GameState) + UObject::FindOffset(XORSTRING("StructProperty FortniteGame.FortGameStateAthena.CurrentPlaylistInfo")) + UObject::FindOffset(XORSTRING("ObjectProperty FortniteGame.PlaylistPropertyArray.OverridePlaylist"))) = PlayGround;
				Globals::ProcessEvent(Globals::GameState, OnRep_CurrentPlaylistInfo, nullptr);
			}
		}
	}
	
	if (strstr(Receive.c_str(), "6."))
	{
		UObject* FloatingIsland = UObject::GetObjectFromName(XORSTRING("LF_Athena_POI_15x15_C Athena_POI_Foundations.Athena_POI_Foundations.PersistentLevel.LF_FloatingIsland"));
		UObject* Lake = UObject::GetObjectFromName(XORSTRING("LF_Athena_POI_75x75_C Athena_POI_Foundations.Athena_POI_Foundations.PersistentLevel.LF_Lake1"));

		ShowBuildingFoundation(FloatingIsland, EDynamicFoundationType::Static);
		ShowBuildingFoundation(Lake, EDynamicFoundationType::Static);
	}
	else if (strstr(Receive.c_str(), "7."))
	{
		ShowBuildingFoundation(UObject::GetObjectFromName(XORSTRING("LF_Athena_POI_25x25_C Athena_POI_Foundations.Athena_POI_Foundations.PersistentLevel.LF_Athena_POI_25x36")), EDynamicFoundationType::Static);
	}
	else if (strstr(Receive.c_str(), "8."))
	{
		ShowBuildingFoundation(UObject::GetObjectFromName(XORSTRING("LF_Athena_POI_50x50_C Athena_POI_Foundations.Athena_POI_Foundations.PersistentLevel.LF_Athena_POI_50x53_Volcano")), EDynamicFoundationType::Static);
	}

	*(int*)(__int64(Globals::PlayerController) + UObject::FindOffset(XORSTRING("IntProperty FortniteGame.FortPlayerController.OverriddenBackpackSize"))) = 999;
#endif
}

void SetToSkydiving(UObject* pawn, bool newval)
{
	static auto fn = UObject::GetObjectFromName(XORSTRING("Function FortniteGame.FortPlayerPawn.OnRep_IsSkydiving"));

	bool previousval = !newval;

	*(bool*)(__int64(pawn) + UObject::FindOffset(XORSTRING("BoolProperty FortniteGame.FortPlayerPawn.bIsSkydiving"))) = newval;

	Globals::ProcessEvent(pawn, fn, &previousval);
}

FRotator GetRotation(UObject* Target)
{
	static auto GetRotation = UObject::GetObjectFromName(XORSTRING("Function Engine.Actor.K2_GetActorRotation"));

	struct GetRotation_Params
	{
		FRotator ReturnValue;
	};

	GetRotation_Params params;

	Globals::ProcessEvent(Target, GetRotation, &params);

	return params.ReturnValue;
}

static void TeleportTo(FRotator rotation, FVector location, UObject* Object = Globals::PlayerPawn)
{
	static auto TeleportTo = UObject::GetObjectFromName(XORSTRING("Function Engine.Actor.K2_TeleportTo"));

	struct
	{
		FVector Location;
		FRotator Rotation;
	} Params;

	Params.Location = location;
	Params.Rotation = rotation;

	Globals::ProcessEvent(Object, TeleportTo, &Params);
}


void StartSkydiving()
{
#ifndef SERVERCODE
	void* Result;
	string Receive = EngineVersion::GetVersion(&Result);

	if (strstr(Receive.c_str(), "4.2") ||
		strstr(Receive.c_str(), "8.") ||
		strstr(Receive.c_str(), "9.") ||
		strstr(Receive.c_str(), "10."))
	{
		EAthenaGamePhase* CurrentGamePhase = reinterpret_cast<EAthenaGamePhase*>(__int64(Globals::GameState) + UObject::FindOffset(XORSTRING("EnumProperty FortniteGame.FortGameStateAthena.GamePhase")));
		*CurrentGamePhase = EAthenaGamePhase::Aircraft;
		auto OldGamePhase = EAthenaGamePhase::Warmup;

		static auto OnRep_GamePhase = UObject::GetObjectFromName(XORSTRING("Function FortniteGame.FortGameStateAthena.OnRep_GamePhase"));
		Globals::ProcessEvent(Globals::GameState, OnRep_GamePhase, &OldGamePhase);

		TeleportTo(GetRotation(Globals::PlayerController), FVector{0,0,22500});

		SetToSkydiving(Globals::PlayerPawn, true);
	}
#endif
}

TArray<FName> GetTableRowNames(UObject* TargetTable)
{
	static auto fn = UObject::GetObjectFromName(XORSTRING("Function Engine.DataTableFunctionLibrary.GetDataTableRowNames"));

	struct
	{
		UObject* Table;
		TArray<FName> out;
	} Params;

	Params.Table = TargetTable;

	Globals::ProcessEvent(Globals::DataTableLibrary, fn, &Params);

	return Params.out;
}

FVector GetRightVectorFromRotation(FRotator Rotation)
{
	static auto fn = UObject::GetObjectFromName(XORSTRING("Function Engine.KismetMathLibrary.GetRightVector"));
	static auto object = Globals::kismetMathLib;

	struct
	{
		FRotator in;
		FVector out;
	} params;

	params.in = Rotation;

	Globals::ProcessEvent(object, fn, &params);

	return params.out;
}

FVector GetForwardVectorFromRotation(FRotator Rotation)
{
	static auto fn = UObject::GetObjectFromName(XORSTRING("Function Engine.KismetMathLibrary.GetForwardVector"));
	static auto object = Globals::kismetMathLib;

	struct
	{
		FRotator in;
		FVector out;
	} params;

	params.in = Rotation;

	Globals::ProcessEvent(object, fn, &params);

	return params.out;
}

FVector SumVectors(FVector A, FVector B)
{
	static auto fn = UObject::GetObjectFromName(XORSTRING("Function Engine.KismetMathLibrary.Add_VectorVector"));
	static auto object = Globals::kismetMathLib;

	struct
	{
		FVector in_1;
		FVector in_2;
		FVector out;
	} params;

	params.in_1 = A;
	params.in_2 = B;

	Globals::ProcessEvent(object, fn, &params);

	return params.out;
}

FVector MultiplyVector(FVector A, int B)
{
	static auto fn = UObject::GetObjectFromName(XORSTRING("Function Engine.KismetMathLibrary.Multiply_VectorInt"));
	static auto object = Globals::kismetMathLib;

	struct
	{
		FVector in_1;
		int in_2;
		FVector out;
	} params;

	params.in_1 = A;
	params.in_2 = B;

	Globals::ProcessEvent(object, fn, &params);

	return params.out;
}

FName StringToName(FString text)
{
	static auto fn = UObject::GetObjectFromName(XORSTRING("Function Engine.KismetStringLibrary.Conv_StringToName"));
	static auto object = UObject::GetObjectFromName(XORSTRING("KismetStringLibrary Engine.Default__KismetStringLibrary"));

	struct
	{
		FString in;
		FName Out;
	} Params;

	Params.in = text;

	if (fn) Globals::ProcessEvent(object, fn, &Params);
	else
	{
		int ObjectCount = NewGObjectsPointer ? NewGObjectsPointer->ObjectCount : GObjectsPointer->ObjObjects.NumElements;

		for (auto i = 0; i < ObjectCount; i++)
		{
			UObject* CurrentObject = NewGObjectsPointer ? FindObjectById(i) : GObjectsPointer->ObjObjects.GetByIndex(i);

			if (CurrentObject == nullptr)
			{
				continue;
			}

			if (CurrentObject->GetFullName().find(XORSTRING("FortniteGame.FortLootPackageData.")) != string::npos)
			{
				if (CurrentObject->GetObjectNameString().find(text.ToString()) != string::npos)
				{
					return CurrentObject->Name;
				}
			}
		}
	}

	return Params.Out;
}

FText StringToText(FString text)
{
	static auto fn = UObject::GetObjectFromName(XORSTRING("Function Engine.KismetTextLibrary.Conv_StringToText"));
	static auto object = UObject::GetObjectFromName(XORSTRING("KismetTextLibrary Engine.Default__KismetTextLibrary"));

	struct
	{
		FString in;
		FText Out;
	} Params;

	Params.in = text;

	Globals::ProcessEvent(object, fn, &Params);

	return Params.Out;
}

UObject* SoftObjectPtrToObject(SoftObjectPtr SoftObject)
{
	static auto fn = UObject::GetObjectFromName(XORSTRING("Function Engine.KismetSystemLibrary.Conv_SoftObjectReferenceToObject"));
	static auto object = UObject::GetObjectFromName(XORSTRING("KismetSystemLibrary Engine.Default__KismetSystemLibrary"));

	struct
	{
		SoftObjectPtr in;
		UObject* out;
	} params;

	params.in = SoftObject;

	Globals::ProcessEvent(object, fn, &params);

	return params.out;
}

static void Log(FString text)
{
	static auto fn = UObject::GetObjectFromName(XORSTRING("Function Engine.VisualLoggerKismetLibrary.LogText"));
	static auto object = UObject::GetObjectFromName(XORSTRING("VisualLoggerKismetLibrary Engine.Default__VisualLoggerKismetLibrary"));

	struct
	{
		UObject* World;
		FString text;
		FName category;
		bool printToLog;
	} Params;

	Params.World = GetWorld();
	Params.text = text;
	Params.category = StringToName(L"LogFort");
	Params.printToLog = true;

	Globals::ProcessEvent(object, fn, &Params);
}

TArray<FString> GetDataTableColumnAsString(UObject* TargetTable, FName columnname)
{
	static auto fn = UObject::GetObjectFromName(XORSTRING("Function Engine.DataTableFunctionLibrary.GetDataTableColumnAsString"));

	struct
	{
		UObject* Table;
		FName name;
		TArray<FString> out;
	} Params;

	Params.Table = TargetTable;
	Params.name = columnname;

	if (fn) {
		Globals::ProcessEvent(Globals::DataTableLibrary, fn, &Params);

		return Params.out;
	}

	TArray<FString> empty;

	return empty;
}

void InitializeObjects()
{
	Globals::GameplayStatics = UObject::GetObjectFromName(XORSTRING("GameplayStatics Engine.Default__GameplayStatics"));
	Globals::InventoryContext = UObject::GetObjectFromName(XORSTRING("FortInventoryContext Transient.FortEngine_"));
	Globals::DataTableLibrary = UObject::GetObjectFromName(XORSTRING("DataTableFunctionLibrary Engine.Default__DataTableFunctionLibrary"));
	Globals::kismetMathLib = UObject::GetObjectFromName(XORSTRING("KismetMathLibrary Engine.Default__KismetMathLibrary"));
	Globals::HudContext = UObject::GetObjectFromName(XORSTRING("FortHUDContext Transient.FortEngine_"));

	if (*reinterpret_cast<UObject**>(__int64(Globals::PlayerController) + UObject::FindOffset(XORSTRING("ObjectProperty Engine.PlayerController.CheatManager"))))
	{
		Globals::CheatManager = *reinterpret_cast<UObject**>(__int64(Globals::PlayerController) + UObject::FindOffset(XORSTRING("ObjectProperty Engine.PlayerController.CheatManager")));
	}
	else
	{
		Globals::CheatManager = SpawnObject(UObject::GetObjectFromName(XORSTRING("Class Engine.CheatManager")), Globals::PlayerController);
		*reinterpret_cast<UObject**>(__int64(Globals::PlayerController) + UObject::FindOffset(XORSTRING("ObjectProperty Engine.PlayerController.CheatManager"))) = Globals::CheatManager;
	}

	Globals::ChestSound = UObject::GetObjectFromName(XORSTRING("SoundCue Tiered_Chest_Open_T01_Cue.Tiered_Chest_Open_T01_Cue"));
	Globals::AmmoBoxSound = UObject::GetObjectFromName(XORSTRING("SoundCue Toolbox_SearchEnd_Cue.Toolbox_SearchEnd_Cue"));
}

void DestroyLods()
{
	struct DestroyLods_Params
	{
		UObject* Class;
	};
	DestroyLods_Params params;
	params.Class = UObject::GetObjectFromName(XORSTRING("Class FortniteGame.FortHLODSMActor"));
		
	if (params.Class)
	{
		Globals::ProcessEvent(Globals::CheatManager, UObject::GetObjectFromName(XORSTRING("Function Engine.CheatManager.DestroyAll")), &params);
	}
	
}

void MiniMap()
{
	//Game Version in Receive
	void* Result;
	string Receive = EngineVersion::GetVersion(&Result);

	auto MinimapAthena = *reinterpret_cast<UObject**>(__int64(Globals::WorldSettings) + UObject::FindOffset(XORSTRING("StructProperty FortniteGame.FortWorldSettings.AthenaMapImage")) + UObject::FindOffset(XORSTRING("ObjectProperty SlateCore.SlateBrush.ResourceObject")));
	*reinterpret_cast<UObject**>(__int64(Globals::GameState) + UObject::FindOffset(XORSTRING("StructProperty FortniteGame.FortGameStateAthena.MinimapBackgroundBrush")) + UObject::FindOffset(XORSTRING("ObjectProperty SlateCore.SlateBrush.ResourceObject"))) = MinimapAthena;
	
	auto Check = UObject::GetObjectFromName(XORSTRING("StructProperty FortniteGame.FortGameStateAthena.MinimapSafeZoneBrush"));
	auto Check_2 = UObject::GetObjectFromName(XORSTRING("StructProperty FortniteGame.FortGameStateAthena.MinimapNextCircleBrush"));
	auto Check_3 = UObject::GetObjectFromName(XORSTRING("StructProperty FortniteGame.FortGameStateAthena.FullMapNextCircleBrush"));

	if (strstr(Receive.c_str(), "2.") && !IsBadReadPtr(Check))
	{
		*reinterpret_cast<FSlateBrush*>(__int64(Globals::GameState) + UObject::FindOffset(XORSTRING("StructProperty FortniteGame.FortGameStateAthena.MinimapSafeZoneBrush"))) = {};
	}
	if (strstr(Receive.c_str(), "3.") ||
		strstr(Receive.c_str(), "4.") ||
		strstr(Receive.c_str(), "5.") ||
		strstr(Receive.c_str(), "6.") ||
		strstr(Receive.c_str(), "7.") ||
		strstr(Receive.c_str(), "8."))
	{
		*reinterpret_cast<FSlateBrush*>(__int64(Globals::GameState) + UObject::FindOffset(XORSTRING("StructProperty FortniteGame.FortGameStateAthena.MinimapSafeZoneBrush"))) = {};
		*reinterpret_cast<FSlateBrush*>(__int64(Globals::GameState) + UObject::FindOffset(XORSTRING("StructProperty FortniteGame.FortGameStateAthena.MinimapCircleBrush"))) = {};
		if (Check_2) *reinterpret_cast<FSlateBrush*>(__int64(Globals::GameState) + UObject::FindOffset(XORSTRING("StructProperty FortniteGame.FortGameStateAthena.MinimapNextCircleBrush"))) = {};
		*reinterpret_cast<FSlateBrush*>(__int64(Globals::GameState) + UObject::FindOffset(XORSTRING("StructProperty FortniteGame.FortGameStateAthena.FullMapCircleBrush"))) = {};
		if (Check_3) *reinterpret_cast<FSlateBrush*>(__int64(Globals::GameState) + UObject::FindOffset(XORSTRING("StructProperty FortniteGame.FortGameStateAthena.FullMapNextCircleBrush"))) = {};
	}
}

void SetName()
{
	static auto SetName = UObject::GetObjectFromName(XORSTRING("Function Engine.GameModeBase.ChangeName"));
	static auto ServerChangeName = UObject::GetObjectFromName(XORSTRING("Function Engine.PlayerController.ServerChangeName"));

	struct ChangeName_Params
	{
		UObject* PlayerController;
		FString Name;
		bool UpdateName;
	};

	ChangeName_Params params;
	params.PlayerController = Globals::PlayerController;

#if defined(RELEASEVERSION)
	params.Name = TEXT("PROJECT ERA");
	FString Name = TEXT("PROJECT ERA");
#elif defined(TESTINGVERSION)
	params.Name = TEXT("ERA - TESTING VERSION");
	FString Name = TEXT("ERA - TESTING VERSION");
#endif

	params.UpdateName = true;

	Globals::ProcessEvent(Globals::GameMode, SetName, &params);
	Globals::ProcessEvent(Globals::PlayerController, ServerChangeName, &Name);
}

void ServerChoosePart(TEnumAsByte<EFortCustomPartType> Type, UObject* Part, UObject* Target = Globals::PlayerPawn)
{
	static auto ServerChoosePart = UObject::GetObjectFromName(XORSTRING("Function FortniteGame.FortPlayerPawn.ServerChoosePart"));

	struct ServerChoosePart_Params
	{
		TEnumAsByte<EFortCustomPartType> Part;
		UObject* ChosenCharacterPart;
	};

	ServerChoosePart_Params params;
	params.Part = Type;
	params.ChosenCharacterPart = Part;

	Globals::ProcessEvent(Target, ServerChoosePart, &params);
	
}

void SetHealth(UObject* Target, float Value)
{
	static auto SetHealth = UObject::GetObjectFromName(XORSTRING("Function FortniteGame.FortPawn.SetHealth"));

	Globals::ProcessEvent(Target, SetHealth, &Value);
}

void SetMaxHealth(UObject* Target, float Value)
{
	static auto SetMaxHealth = UObject::GetObjectFromName(XORSTRING("Function FortniteGame.FortPawn.SetMaxHealth"));

	Globals::ProcessEvent(Target, SetMaxHealth, &Value);
}

void EquipSkin()
{
	static auto Onrep_CPS = UObject::GetObjectFromName(XORSTRING("Function FortniteGame.FortPlayerState.OnRep_CharacterParts"));

	if (Globals::bInitializedSkins)
	{
		if (Globals::Body) ServerChoosePart(EFortCustomPartType::Body, Globals::Body);
		if (Globals::Hat) ServerChoosePart(EFortCustomPartType::Hat, Globals::Hat);
		if (Globals::Head) ServerChoosePart(EFortCustomPartType::Head, Globals::Head);
		if (Globals::BackPack) ServerChoosePart(EFortCustomPartType::Backpack, Globals::BackPack);
		Globals::ProcessEvent(Globals::PlayerState, Onrep_CPS, nullptr);
		return;
	}

	std::vector<UObject*> CharacterPartsArray;

	auto FortHero = *(UObject**)(__int64(Globals::PlayerController) + OffsetTable::StrongMyHero);
	if (!FortHero) FortHero = UObject::GetObjectFromName(XORSTRING("FortHero Transient.FortHero_"));
	auto HeroCharacterParts = *reinterpret_cast<TArray<UObject*>*>(reinterpret_cast<uintptr_t>(FortHero) + OffsetTable::CharacterParts);

	for (auto i = 0; i < HeroCharacterParts.Num(); i++)
		CharacterPartsArray.push_back(HeroCharacterParts[i]);



	for (auto i = 0; i < CharacterPartsArray.size(); i++)
	{
		auto AdditionalData = *reinterpret_cast<UObject**>(reinterpret_cast<uintptr_t>(CharacterPartsArray[i]) + OffsetTable::AdditionalData);
		if (AdditionalData->IsA(UObject::GetObjectFromName(XORSTRING("Class FortniteGame.CustomCharacterHeadData"))))
		{
			ServerChoosePart(EFortCustomPartType::Head, CharacterPartsArray[i]);
			Globals::Head = CharacterPartsArray[i];
		}
		else if (AdditionalData->IsA(UObject::GetObjectFromName(XORSTRING("Class FortniteGame.CustomCharacterBodyPartData"))))
		{
			ServerChoosePart(EFortCustomPartType::Body, CharacterPartsArray[i]);
			Globals::Body = CharacterPartsArray[i];
		}
		else if (AdditionalData->IsA(UObject::GetObjectFromName(XORSTRING("Class FortniteGame.CustomCharacterHatData"))))
		{
			ServerChoosePart(EFortCustomPartType::Hat, CharacterPartsArray[i]);
			Globals::Hat = CharacterPartsArray[i];
		}
		else if (AdditionalData->IsA(UObject::GetObjectFromName(XORSTRING("Class FortniteGame.CustomCharacterBackpackData"))))
		{
			ServerChoosePart(EFortCustomPartType::Backpack, CharacterPartsArray[i]);
			Globals::BackPack = CharacterPartsArray[i];
		}
	}

	if (UObject::FindOffset(XORSTRING("ObjectProperty FortniteGame.FortAthenaLoadout.Backpack")) > 0) {
		auto BID = OffsetTable::CustomizationLoadout > 0 ? *(UObject**)(__int64(Globals::PlayerController) + OffsetTable::CustomizationLoadout + UObject::FindOffset(XORSTRING("ObjectProperty FortniteGame.FortAthenaLoadout.Backpack"))) :
			*(UObject**)(__int64(Globals::PlayerController) + UObject::FindOffset(XORSTRING("StructProperty FortniteGame.FortPlayerController.CosmeticLoadoutPC")) + UObject::FindOffset(XORSTRING("ObjectProperty FortniteGame.FortAthenaLoadout.Backpack")));

		TArray<UObject*> BackPackArray;

		if (!IsBadReadPtr(BID) && BID) {
			if (UObject::GetObjectFromName(XORSTRING("Function FortniteGame.AthenaCharacterPartItemDefinition.GetCharacterParts"))) Globals::ProcessEvent(BID, UObject::GetObjectFromName(XORSTRING("Function FortniteGame.AthenaCharacterPartItemDefinition.GetCharacterParts")), &BackPackArray);
			if (!IsBadReadPtr(&BackPackArray) && BackPackArray[0] != nullptr) ServerChoosePart(EFortCustomPartType::Backpack, BackPackArray[0]);
		}
	}

	Globals::ProcessEvent(Globals::PlayerState, Onrep_CPS, nullptr);

	Globals::bInitializedSkins = true;
}

void HideNetDebugUI()
{
#ifndef SERVERCODE
	auto NetDebugUI = UObject::GetObjectFromName(XORSTRING("NetDebugUI_C Transient.FortEngine_"));
	if (NetDebugUI) Globals::ProcessEvent(NetDebugUI, UObject::GetObjectFromName(XORSTRING("Function UMG.UserWidget.RemoveFromViewport")), nullptr);
#endif
}

void Destroy(UObject* Target)
{
	static auto DestroyActor = UObject::GetObjectFromName(XORSTRING("Function Engine.Actor.K2_DestroyActor"));

	Globals::ProcessEvent(Target, DestroyActor, nullptr);
}

void Summon(FString Parameter)
{
	static auto Summon = UObject::GetObjectFromName(XORSTRING("Function Engine.CheatManager.Summon"));

	struct Summon_Params
	{
		FString Object;
	};

	Summon_Params params;
	params.Object = Parameter;

	Globals::ProcessEvent(Globals::CheatManager, Summon, &params);
}

UObject* FindActorsFromClass(UObject* Class, int Index = 0)
{
	static auto GetActorsFunction = UObject::GetObjectFromName(XORSTRING("Function Engine.GameplayStatics.GetAllActorsOfClass"));

	TArray<UObject*> ActorArray;

	struct FindActor_Params
	{
		UObject* WorldContextObject;
		UObject* ActorClass;
		TArray<UObject*> ReturnValue;
	};

	FindActor_Params params;
	params.WorldContextObject = Globals::World;
	params.ActorClass = Class;

	Globals::ProcessEvent(Globals::GameplayStatics, GetActorsFunction, &params);

	ActorArray = params.ReturnValue;

	return ActorArray[Index];
}

TArray<UObject*> ArrayFindActorsFromClass(UObject* Class)
{
	static auto GetActorsFunction = UObject::GetObjectFromName(XORSTRING("Function Engine.GameplayStatics.GetAllActorsOfClass"));

	TArray<UObject*> ActorArray;

	struct FindActor_Params
	{
		UObject* WorldContextObject;
		UObject* ActorClass;
		TArray<UObject*> ReturnValue;
	};

	FindActor_Params params;
	params.WorldContextObject = Globals::World;
	params.ActorClass = Class;

	Globals::ProcessEvent(Globals::GameplayStatics, GetActorsFunction, &params);

	ActorArray = params.ReturnValue;

	return ActorArray;
}

FVector GetLocation(UObject* Target)
{
	static auto GetLocation = UObject::GetObjectFromName(XORSTRING("Function Engine.Actor.K2_GetActorLocation"));

	struct GetLocation_Params
	{
		FVector ReturnValue;
	};

	GetLocation_Params params;

	Globals::ProcessEvent(Target, GetLocation, &params);

	if (Target && !IsBadReadPtr(&params.ReturnValue))
	{
		return params.ReturnValue;
	}
}

void SetActorRotation(UObject* Target, FRotator Rot)
{
	static auto SetRotation = UObject::GetObjectFromName(XORSTRING("Function Engine.Actor.K2_SetActorRotation"));
	
	struct SetRotation_Params
	{
		FRotator Rotation;
		bool bTeleportPhysx;
	};

	SetRotation_Params params;
	params.Rotation = Rot;
	params.bTeleportPhysx = false;

	Globals::ProcessEvent(Target, SetRotation, &params);
}

void SetActorScale3D(UObject* Target, FVector Vector)
{
	static auto SetActorScale3D = UObject::GetObjectFromName(XORSTRING("Function Engine.Actor.SetActorScale3D"));

	Globals::ProcessEvent(Target, SetActorScale3D, &Vector);
}

void AdjustRotation()
{
	FRotator CurrentRotation = GetRotation(Globals::PlayerPawn);
	CurrentRotation.Pitch = 0;
	CurrentRotation.Roll = 0;

	SetActorRotation(Globals::PlayerPawn, CurrentRotation);
}

void ApplyBattleBus()
{
#ifdef RELEASEVERSION
	void* Result;
	auto ToConvert = reinterpret_cast<FString * (__fastcall*)(void*)>(UniversalStructInstance->addr_GetEngineVersion)(&Result);
	string Converted;
	Converted = ToConvert->ToString();

	if (Converted.find("3807424") != string::npos)
	{
		UObject* CurrentBus = UObject::GetObjectFromName(XORSTRING("AthenaBattleBusItemDefinition BBID_WinterBus.BBID_WinterBus"));
		if (CurrentBus)
		{
			*reinterpret_cast<UObject**>(reinterpret_cast<uintptr_t>(Globals::PlayerController) + UObject::FindOffset(XORSTRING("StructProperty FortniteGame.FortPlayerControllerAthena.CustomizationLoadout")) + UObject::FindOffset(XORSTRING("ObjectProperty FortniteGame.FortAthenaLoadout.BattleBus"))) = CurrentBus;
		}
		
	}
	else if (Converted.find("5.") != string::npos)
	{
		UObject* CurrentBus = UObject::GetObjectFromName(XORSTRING("AthenaBattleBusItemDefinition BBID_BirthdayBus.BBID_BirthdayBus"));
		if (CurrentBus) {
			*reinterpret_cast<UObject**>(reinterpret_cast<uintptr_t>(Globals::PlayerController) + UObject::FindOffset(XORSTRING("StructProperty FortniteGame.FortPlayerControllerAthena.CustomizationLoadout")) + UObject::FindOffset(XORSTRING("ObjectProperty FortniteGame.FortAthenaLoadout.BattleBus"))) = CurrentBus;
		}
	}
	else if (Converted.find("6.") != string::npos)
	{
		UObject* CurrentBus = UObject::GetObjectFromName(XORSTRING("AthenaBattleBusItemDefinition BBID_HalloweenBus.BBID_HalloweenBus"));
		if (CurrentBus) {
			*reinterpret_cast<UObject**>(reinterpret_cast<uintptr_t>(Globals::PlayerController) + UObject::FindOffset(XORSTRING("StructProperty FortniteGame.FortPlayerControllerAthena.CustomizationLoadout")) + UObject::FindOffset(XORSTRING("ObjectProperty FortniteGame.FortAthenaLoadout.BattleBus"))) = CurrentBus;
		}
	}
	else if (Converted.find("7.") != string::npos)
	{
		UObject* CurrentBus = UObject::GetObjectFromName(XORSTRING("AthenaBattleBusItemDefinition BBID_WinterBus.BBID_WinterBus"));
		if (CurrentBus)
		{
			if (OffsetTable::CustomizationLoadout > 0) {
				*reinterpret_cast<UObject**>(reinterpret_cast<uintptr_t>(Globals::PlayerController) + UObject::FindOffset(XORSTRING("StructProperty FortniteGame.FortPlayerControllerAthena.CustomizationLoadout")) + UObject::FindOffset(XORSTRING("ObjectProperty FortniteGame.FortAthenaLoadout.BattleBus"))) = CurrentBus;
			}
			else 
			{
				*reinterpret_cast<UObject**>(reinterpret_cast<uintptr_t>(Globals::PlayerController) + UObject::FindOffset(XORSTRING("StructProperty FortniteGame.FortPlayerController.CosmeticLoadoutPC")) + UObject::FindOffset(XORSTRING("ObjectProperty FortniteGame.FortAthenaLoadout.BattleBus"))) = CurrentBus;
			}
		}
	}
#endif
}

void CustomizationLoadout()
{
#ifdef RELEASEVERSION
	static auto ReplicateLoadout = UObject::GetObjectFromName(XORSTRING("Function FortniteGame.FortPlayerPawnAthena.OnRep_CustomizationLoadout"));

	if (ReplicateLoadout) Globals::ProcessEvent(Globals::PlayerPawn, ReplicateLoadout, nullptr);
#endif
}

void SetOwner(UObject* Target)
{
	struct SetOwner_Params
	{
		UObject* Owner;
	};

	SetOwner_Params params;
	params.Owner = Globals::PlayerController;


	Globals::ProcessEvent(Target, UObject::GetObjectFromName(XORSTRING("Function Engine.Actor.SetOwner")), &params);
}

UObject* CreateItem(UObject* ItemDefinition, int Count)
{
	if (!ItemDefinition) return nullptr;

	static auto CreateItem = UObject::GetObjectFromName(XORSTRING("Function FortniteGame.FortItemDefinition.CreateTemporaryItemInstanceBP"));
	static auto SetOwner = UObject::GetObjectFromName(XORSTRING("Function FortniteGame.FortItem.SetOwningControllerForTemporaryItem"));

	struct CreateTemporaryItem_Params
	{
		int A;
		int B;
		UObject* ReturnValue;
	};

	struct SetController_Params
	{
		UObject* Controller;
	};

	SetController_Params SecondParams;
	SecondParams.Controller = Globals::PlayerController;

	CreateTemporaryItem_Params params;
	params.A = 1;
	params.B = 1; 

	Globals::ProcessEvent(ItemDefinition, CreateItem, &params);

	auto FortItem = params.ReturnValue;

	Globals::ProcessEvent(FortItem, SetOwner, &SecondParams);

	if (Count > 0)
	{
		reinterpret_cast<ItemCount*>(reinterpret_cast<uintptr_t>(FortItem) + OffsetTable::ItemEntry)->Count = Count;
	}

	return FortItem;
}

void UpdateInventory()
{
	static auto HandleWorldInventoryLocalUpdate = UObject::GetObjectFromName(XORSTRING("Function FortniteGame.FortPlayerController.HandleWorldInventoryLocalUpdate"));
	static auto HandleInventoryLocalUpdate = UObject::GetObjectFromName(XORSTRING("Function FortniteGame.FortInventory.HandleInventoryLocalUpdate"));
	static auto ClientUpdate = UObject::GetObjectFromName(XORSTRING("Function FortniteGame.FortPlayerController.ClientForceWorldInventoryUpdate"));
	static auto OnRep_Quickbar = UObject::GetObjectFromName(XORSTRING("Function FortniteGame.FortPlayerController.OnRep_QuickBar"));
	static auto OnRep_SecondaryQB = UObject::GetObjectFromName(XORSTRING("Function FortniteGame.FortQuickBars.OnRep_SecondaryQuickBar"));
	static auto OnRep_PrimaryQB = UObject::GetObjectFromName(XORSTRING("Function FortniteGame.FortQuickBars.OnRep_PrimaryQuickBar"));
	static auto ForceUpdateClientQuickbars = UObject::GetObjectFromName(XORSTRING("Function FortniteGame.FortPlayerController.ClientForceUpdateQuickbar"));

	EFortQuickBars Primary = EFortQuickBars::Primary;
	EFortQuickBars Secondary = EFortQuickBars::Secondary;

	if (ClientUpdate) Globals::ProcessEvent(Globals::PlayerController, ClientUpdate, nullptr);
	
	Globals::ProcessEvent(Globals::PlayerController, HandleWorldInventoryLocalUpdate, nullptr);
	Globals::ProcessEvent(Globals::Inventory, HandleInventoryLocalUpdate, nullptr);

	if (OnRep_Quickbar) {
		Globals::ProcessEvent(Globals::PlayerController, OnRep_Quickbar, nullptr);
		Globals::ProcessEvent(Globals::Quickbar, OnRep_SecondaryQB, nullptr);
		Globals::ProcessEvent(Globals::Quickbar, OnRep_PrimaryQB, nullptr);
	}
	else {
		Globals::ProcessEvent(Globals::PlayerController, ForceUpdateClientQuickbars, &Secondary);
		Globals::ProcessEvent(Globals::PlayerController, ForceUpdateClientQuickbars, &Primary);
	}
}

FGuid GetGuid(UObject* FortItem)
{
	static auto GetGuid = UObject::GetObjectFromName(XORSTRING("Function FortniteGame.FortItem.GetItemGuid"));

	struct GetGuid_Params
	{
		FGuid ReturnValue;
	};

	GetGuid_Params params;
	
	Globals::ProcessEvent(FortItem, GetGuid, &params);

	return params.ReturnValue;
}

int GetCount(UObject* FortItem)
{
	return *(int*)(__int64(FortItem) + OffsetTable::ItemEntry + OffsetTable::ItemEntryCount);
}

static void DestroyItem(UObject* Item, int Count)
{
	static auto DestroyItem = UObject::GetObjectFromName(XORSTRING("Function FortniteUI.FortInventoryContext.DestroyWorldItem"));
	static auto DropItem = UObject::GetObjectFromName(XORSTRING("Function FortniteUI.FortInventoryContext.DropItem"));

	struct
	{
		UObject* Item;
		int Count;
	} params;

	params.Item = Item;
	params.Count = Count;

	Globals::ProcessEvent(Globals::InventoryContext, DropItem, &params);
	Globals::ProcessEvent(Globals::InventoryContext, DestroyItem, &Item);
	
}

void ActivateSlot(EFortQuickBars Index, int Slot)
{
	static UObject* ActivateSlot = UObject::GetObjectFromName(XORSTRING("Function FortniteGame.FortQuickBars.ServerActivateSlotInternal"));

	struct
	{
		EFortQuickBars QuickbarIndex;
		int Slot;
		float ActivateDelay;
		bool UpdatePreviousFocusedSlot;
		bool ForceExecution;
	} params;

	params.QuickbarIndex = Index;
	params.Slot = Slot;
	params.ActivateDelay = 0;
	params.UpdatePreviousFocusedSlot = false;
	params.ForceExecution = true;

	if (ActivateSlot) Globals::ProcessEvent(Globals::Quickbar, ActivateSlot, &params);
}

void EnableSlot()
{

}

void AddToInventory(UObject* FortItem, EFortQuickBars QuickbarIndex, int Slot)
{
	if (!FortItem) return;

	static auto ServerAddItemInternal = UObject::GetObjectFromName(XORSTRING("Function FortniteGame.FortQuickBars.ServerAddItemInternal"));
	struct ServerAddItem_Params
	{
		FGuid Item;
		EFortQuickBars Quickbar;
		int Slot;
	};

	reinterpret_cast<TArray<UObject*>*>(reinterpret_cast<uintptr_t>(Globals::Inventory) + OffsetTable::FortInventory + OffsetTable::ItemInstances)->Add(FortItem);

	int ItemEntrySize = UObject::FindOffset("ArrayProperty FortniteGame.FortPickup.MultiItemPickupEntries") - UObject::FindOffset("StructProperty FortniteGame.FortPickup.PrimaryPickupItemEntry");

	switch (ItemEntrySize)
	{
	case 0xA8: {
		struct ItemEntrySize { char size[0xA8]; }; reinterpret_cast<TArray<ItemEntrySize>*>(reinterpret_cast<uintptr_t>(Globals::Inventory) + OffsetTable::FortInventory + OffsetTable::ItemEntries)->Add(*reinterpret_cast<ItemEntrySize*>(reinterpret_cast<uintptr_t>(FortItem) + OffsetTable::ItemEntry));
		break;
	}
	case 0xB0: {
		struct ItemEntrySize { char size[0xB0]; }; reinterpret_cast<TArray<ItemEntrySize>*>(reinterpret_cast<uintptr_t>(Globals::Inventory) + OffsetTable::FortInventory + OffsetTable::ItemEntries)->Add(*reinterpret_cast<ItemEntrySize*>(reinterpret_cast<uintptr_t>(FortItem) + OffsetTable::ItemEntry));
		break;
	}
	case 0xC0: {
		struct ItemEntrySize { char size[0xC0]; }; reinterpret_cast<TArray<ItemEntrySize>*>(reinterpret_cast<uintptr_t>(Globals::Inventory) + OffsetTable::FortInventory + OffsetTable::ItemEntries)->Add(*reinterpret_cast<ItemEntrySize*>(reinterpret_cast<uintptr_t>(FortItem) + OffsetTable::ItemEntry));
		break;
	}
	case 0xC8: {
		struct ItemEntrySize { char size[0xC8]; }; reinterpret_cast<TArray<ItemEntrySize>*>(reinterpret_cast<uintptr_t>(Globals::Inventory) + OffsetTable::FortInventory + OffsetTable::ItemEntries)->Add(*reinterpret_cast<ItemEntrySize*>(reinterpret_cast<uintptr_t>(FortItem) + OffsetTable::ItemEntry));
		break;
	}
	case 0xD0: {
		struct ItemEntrySize { char size[0xD0]; }; reinterpret_cast<TArray<ItemEntrySize>*>(reinterpret_cast<uintptr_t>(Globals::Inventory) + OffsetTable::FortInventory + OffsetTable::ItemEntries)->Add(*reinterpret_cast<ItemEntrySize*>(reinterpret_cast<uintptr_t>(FortItem) + OffsetTable::ItemEntry));
		break;
	}
	case 0x120: {
		struct ItemEntrySize { char size[0x120]; }; reinterpret_cast<TArray<ItemEntrySize>*>(reinterpret_cast<uintptr_t>(Globals::Inventory) + OffsetTable::FortInventory + OffsetTable::ItemEntries)->Add(*reinterpret_cast<ItemEntrySize*>(reinterpret_cast<uintptr_t>(FortItem) + OffsetTable::ItemEntry));
		break;
	}
	}

	ServerAddItem_Params params;
	params.Item = GetGuid(FortItem);
	params.Quickbar = QuickbarIndex;
	params.Slot = Slot;

	if (ServerAddItemInternal) Globals::ProcessEvent(Globals::Quickbar, ServerAddItemInternal, &params);

}

UObject* GetDefinition(UObject* FortItem)
{
	static auto GetDefinition = UObject::GetObjectFromName(XORSTRING("Function FortniteGame.FortItem.GetItemDefinitionBP"));

	struct GetItemDef_Params
	{
		UObject* ItemDefinition;
	};
	GetItemDef_Params params;

	Globals::ProcessEvent(FortItem, GetDefinition, &params);

	return params.ItemDefinition;
}

UObject* GetQuickbarItem(EFortQuickBars QuickBarIndex, int Slot)
{
	static auto GetSlottedItem = UObject::GetObjectFromName(XORSTRING("Function FortniteUI.FortInventoryContext.GetQuickBarSlottedItem"));

	struct GetQuickbarItem_Params
	{
		EFortQuickBars QuickbarIndex;
		int Slot;
		UObject* ReturnValue;
	};

	GetQuickbarItem_Params params;
	params.QuickbarIndex = QuickBarIndex;
	params.Slot = Slot;

	Globals::ProcessEvent(Globals::InventoryContext, GetSlottedItem, &params);

	return params.ReturnValue;
	
}

void GetCurrentFocusedSlot(EFortQuickBars* QuickBarIndex, int* Slot, int* SecondSlot)
{
	static auto GetSlottedItem = UObject::GetObjectFromName(XORSTRING("Function FortniteUI.FortInventoryContext.GetQuickbarFocus"));

	struct
	{
		EFortQuickBars QuickbarIndex;
		int Slot;
		int SecondarySlot;
		int PreviouslyFocusedSlot;
	} params;

	Globals::ProcessEvent(Globals::InventoryContext, GetSlottedItem, &params);

	*QuickBarIndex = params.QuickbarIndex;
	*Slot = params.Slot;
	*SecondSlot = params.SecondarySlot;
}

UObject* EquipWeapon(UObject* ItemDef, FGuid ItemGuid)
{
#ifndef SERVERCODE
	static auto EquipWeaponDefinition = UObject::GetObjectFromName(XORSTRING("Function FortniteGame.FortPawn.EquipWeaponDefinition"));

	struct EquipWeapon_Params
	{
		UObject* ItemDefinition;
		FGuid ItemGuid;
		UObject* ReturnValue;
	};

	EquipWeapon_Params params;
	params.ItemDefinition = ItemDef;
	params.ItemGuid = ItemGuid;

	Globals::ProcessEvent(Globals::PlayerPawn, EquipWeaponDefinition, &params);

	return params.ReturnValue;
#endif
}

void Inventory()
{
#ifndef SERVERCODE
	struct InventoryPointer
	{
		UObject* Inventory;
	};

	struct QuickBarPointer
	{
		UObject* QuickBar;
	};

	Globals::Inventory = reinterpret_cast<InventoryPointer*>(reinterpret_cast<uintptr_t>(Globals::PlayerController) + OffsetTable::Inventory)->Inventory;

	if (OffsetTable::QuickBar != 0)
	{
		Globals::Quickbar = SpawnActorFromClass(XORSTRING("Class FortniteGame.FortQuickBars"), GetLocation(Globals::PlayerPawn));
		reinterpret_cast<QuickBarPointer*>(reinterpret_cast<uintptr_t>(Globals::PlayerController) + OffsetTable::QuickBar)->QuickBar = Globals::Quickbar;
	}
	else Globals::Quickbar = *(UObject**)(__int64(Globals::PlayerController) + UObject::FindOffset(XORSTRING("ObjectProperty FortniteGame.FortPlayerController.ClientQuickBars")));

	SetOwner(Globals::Inventory);
	SetOwner(Globals::Quickbar);

	AddToInventory(CreateItem(Globals::PickaxeItemDefinition, 0), EFortQuickBars::Primary, 0);
	// BUILDING PIECES
	AddToInventory(CreateItem(UObject::GetObjectFromName(XORSTRING("FortBuildingItemDefinition BuildingItemData_Wall.BuildingItemData_Wall")), 0), EFortQuickBars::Secondary, 0);
	AddToInventory(CreateItem(UObject::GetObjectFromName(XORSTRING("FortBuildingItemDefinition BuildingItemData_Floor.BuildingItemData_Floor")), 0), EFortQuickBars::Secondary, 1);
	AddToInventory(CreateItem(UObject::GetObjectFromName(XORSTRING("FortBuildingItemDefinition BuildingItemData_Stair_W.BuildingItemData_Stair_W")), 0), EFortQuickBars::Secondary, 2);
	AddToInventory(CreateItem(UObject::GetObjectFromName(XORSTRING("FortBuildingItemDefinition BuildingItemData_RoofS.BuildingItemData_RoofS")), 0), EFortQuickBars::Secondary, 3);
	AddToInventory(CreateItem(UObject::GetObjectFromName(XORSTRING("FortResourceItemDefinition WoodItemData.WoodItemData"), false, false, true, Globals::GItemDefs), 999), EFortQuickBars::Max_None, 0);
	AddToInventory(CreateItem(UObject::GetObjectFromName(XORSTRING("FortResourceItemDefinition StoneItemData.StoneItemData"), false, false, true, Globals::GItemDefs), 999), EFortQuickBars::Max_None, 0);
	AddToInventory(CreateItem(UObject::GetObjectFromName(XORSTRING("FortResourceItemDefinition MetalItemData.MetalItemData"), false, false, true, Globals::GItemDefs), 999), EFortQuickBars::Max_None, 0);
	AddToInventory(CreateItem(UObject::GetObjectFromName(XORSTRING("FortAmmoItemDefinition AthenaAmmoDataBulletsHeavy.AthenaAmmoDataBulletsHeavy"), false, false, true, Globals::GItemDefs), 999), EFortQuickBars::Max_None, 0);
	AddToInventory(CreateItem(UObject::GetObjectFromName(XORSTRING("FortAmmoItemDefinition AthenaAmmoDataBulletsLight.AthenaAmmoDataBulletsLight"), false, false, true, Globals::GItemDefs), 999), EFortQuickBars::Max_None, 0);
	AddToInventory(CreateItem(UObject::GetObjectFromName(XORSTRING("FortAmmoItemDefinition AthenaAmmoDataBulletsMedium.AthenaAmmoDataBulletsMedium"), false, false, true, Globals::GItemDefs), 999), EFortQuickBars::Max_None, 0);
	AddToInventory(CreateItem(UObject::GetObjectFromName(XORSTRING("FortAmmoItemDefinition AthenaAmmoDataShells.AthenaAmmoDataShells"), false, false, true, Globals::GItemDefs), 999), EFortQuickBars::Max_None, 0);
	AddToInventory(CreateItem(UObject::GetObjectFromName(XORSTRING("FortAmmoItemDefinition AmmoDataRockets.AmmoDataRockets"), false, false, true, Globals::GItemDefs), 999), EFortQuickBars::Max_None, 0);

	UpdateInventory();

	//EDIT TOOL
	Globals::EditTool = UObject::GetObjectFromName(XORSTRING("FortEditToolItemDefinition EditTool.EditTool"));
	Globals::EditToolGUID = GetGuid(CreateItem(Globals::EditTool, 0));
#endif
}

static void BP_ApplyGameplayEffectToSelf(UObject* AbilitySystemComponent, UObject* GameplayEffectClass)
{
	static UObject* BP_ApplyGameplayEffectToSelf = UObject::GetObjectFromName(XORSTRING("Function GameplayAbilities.AbilitySystemComponent.BP_ApplyGameplayEffectToSelf"));

	struct FGameplayEffectContextHandle
	{
		char UnknownData_0[0x30]; // 0x00(0x18)
	};

	struct FActiveGameplayEffectHandle
	{
		int Handle; // 0x00(0x04)
		bool bPassedFiltersAndWasExecuted; // 0x04(0x01)
		char UnknownData_5[0x3]; // 0x05(0x03)
	};

	struct
	{
		UObject* GameplayEffectClass;
		float Level;
		FGameplayEffectContextHandle EffectContext;
		FActiveGameplayEffectHandle ret;
	} Params;

	Params.EffectContext = FGameplayEffectContextHandle();
	Params.GameplayEffectClass = GameplayEffectClass;
	Params.Level = 1.0;

	Globals::ProcessEvent(AbilitySystemComponent, BP_ApplyGameplayEffectToSelf, &Params);
}

void GrantAbility(UObject* AbilityClass)
{
	void* Result;
	string Receive = EngineVersion::GetVersion(&Result);

	if (!Globals::DefaultGameplayEffect ||
		!Globals::GameplayEffectClass)
	{
		Globals::DefaultGameplayEffect = UObject::GetObjectFromName(XORSTRING("GE_Athena_PurpleStuff_C GE_Athena_PurpleStuff.Default__GE_Athena_PurpleStuff_C"));
		Globals::GameplayEffectClass = UObject::GetObjectFromName(XORSTRING("BlueprintGeneratedClass GE_Athena_PurpleStuff.GE_Athena_PurpleStuff_C"));

		if (!Globals::DefaultGameplayEffect || !Globals::GameplayEffectClass)
		{
			Globals::DefaultGameplayEffect = UObject::GetObjectFromName(XORSTRING("GE_Athena_PurpleStuff_Health_C GE_Athena_PurpleStuff_Health.Default__GE_Athena_PurpleStuff_Health_C"));
			Globals::GameplayEffectClass = UObject::GetObjectFromName(XORSTRING("BlueprintGeneratedClass GE_Athena_PurpleStuff_Health.GE_Athena_PurpleStuff_Health_C"));
		}

		if (!Globals::DefaultGameplayEffect || !Globals::GameplayEffectClass)
		{	
			string Path = XORSTRING("/Game/Athena/Items/Consumables/PurpleStuff/GE_Athena_PurpleStuff.GE_Athena_PurpleStuff_C");
			UObject* Class = Globals::StaticLoadObject(UObject::GetObjectFromName(XORSTRING("Class CoreUObject.Object")), NULL, wstring(Path.begin(), Path.end()).c_str(), nullptr, 0, nullptr, true);
			Globals::DefaultGameplayEffect = UObject::GetObjectFromName(XORSTRING("GE_Athena_PurpleStuff_C GE_Athena_PurpleStuff.Default__GE_Athena_PurpleStuff_C"));
			Globals::GameplayEffectClass = UObject::GetObjectFromName(XORSTRING("BlueprintGeneratedClass GE_Athena_PurpleStuff.GE_Athena_PurpleStuff_C"));
		}
	}

	struct FGameplayAbilitySpecDef
	{
		UObject* Ability;
		unsigned char Unk00[0x90];
	};

	TArray<struct FGameplayAbilitySpecDef>* GrantedAbilities = reinterpret_cast<TArray<struct FGameplayAbilitySpecDef>*>(__int64(Globals::DefaultGameplayEffect) + OffsetTable::GrantedAbilitiesOffset);

	// overwrite current gameplay ability with the one we want to activate
	if (AbilityClass)
	{
		GrantedAbilities->operator[](0).Ability = AbilityClass;
	}

	// give this gameplay effect an infinite duration
	enum class EGameplayEffectDurationType : uint8_t
	{
		Instant, Infinite, HasDuration, EGameplayEffectDurationType_MAX
	};

	*reinterpret_cast<EGameplayEffectDurationType*>(__int64(Globals::DefaultGameplayEffect) + OffsetTable::DurationPolicy) = EGameplayEffectDurationType::Infinite;

	BP_ApplyGameplayEffectToSelf(Globals::AbilitySystemComponent, Globals::GameplayEffectClass);
}

void GrantDefaultAbilities()
{
#ifndef SERVERCODE
	Globals::AbilitySystemComponent = *reinterpret_cast<UObject**>(reinterpret_cast<uintptr_t>(Globals::PlayerPawn) + OffsetTable::AbilitySystemComponent);
	GrantAbility(UObject::GetObjectFromName(XORSTRING("Class FortniteGame.FortGameplayAbility_Sprint")));
	GrantAbility(UObject::GetObjectFromName(XORSTRING("Class FortniteGame.FortGameplayAbility_Jump")));
	GrantAbility(UObject::GetObjectFromName(XORSTRING("BlueprintGeneratedClass GA_DefaultPlayer_InteractSearch.GA_DefaultPlayer_InteractSearch_C")));
	GrantAbility(UObject::GetObjectFromName(XORSTRING("BlueprintGeneratedClass GA_DefaultPlayer_InteractUse.GA_DefaultPlayer_InteractUse_C")));
	if (Globals::CurrentEngineVersion >= EEngineVersion::UE_4_20)
	{
		GrantAbility(UObject::GetObjectFromName(XORSTRING("BlueprintGeneratedClass GA_AthenaEnterVehicle.GA_AthenaEnterVehicle_C")));
		GrantAbility(UObject::GetObjectFromName(XORSTRING("BlueprintGeneratedClass GA_AthenaExitVehicle.GA_AthenaExitVehicle_C")));
		GrantAbility(UObject::GetObjectFromName(XORSTRING("BlueprintGeneratedClass GA_AthenaInVehicle.GA_AthenaInVehicle_C")));
	}
#endif
}

static bool IsInVehicle()
{
	static auto IsInVehicle = UObject::GetObjectFromName(XORSTRING("Function FortniteGame.FortPlayerPawn.IsInVehicle"));

	bool returnvalue;

	Globals::ProcessEvent(Globals::PlayerPawn, IsInVehicle, &returnvalue);

	return returnvalue;
}

static UObject* GetVehicle()
{
	static auto IsInVehicle = UObject::GetObjectFromName(XORSTRING("Function FortniteGame.FortPlayerPawn.GetVehicle"));

	UObject* returnvalue;

	Globals::ProcessEvent(Globals::PlayerPawn, IsInVehicle, &returnvalue);

	return returnvalue;
}

void OnRep_bAlreadySearched(UObject* Target)
{
	static UObject* OnRep_bAlreadySearched = UObject::GetObjectFromName(XORSTRING("Function FortniteGame.BuildingContainer.OnRep_bAlreadySearched"));

	Globals::ProcessEvent(Target, OnRep_bAlreadySearched, nullptr);
}

void ClientPlaySoundAtLocation(UObject* Target, UObject* SoundToPlay, FVector Location, float VolumeMultiplier, float PitchMultiplier)
{
	static UObject* ClientPlaySoundAtLocation = UObject::GetObjectFromName(XORSTRING("Function Engine.PlayerController.ClientPlaySoundAtLocation"));

	struct
	{
		UObject* soundToPlay;
		FVector Location;
		float VolumeMultiplier;
		float PitchMultiplier;
	} params;

	params.soundToPlay = SoundToPlay;
	params.Location = Location;
	params.VolumeMultiplier = VolumeMultiplier;
	params.PitchMultiplier = PitchMultiplier;

	Globals::ProcessEvent(Target, ClientPlaySoundAtLocation, &params);
}

void Say(FString Msg)
{
	static UObject* World = Globals::World;

	static UObject* Say = UObject::GetObjectFromName(XORSTRING("Function Engine.GameMode.Say"));

	Globals::ProcessEvent(Globals::GameMode, Say, &Msg);
}

static void TossPickup(UObject* FortPickup, FVector FinalLocation, UObject* ItemOwner, int OverrideMaxStackCount, bool bToss, uint8_t InPickupSourceTypeFlags = 0, uint8_t InPickupSpawnSource = 1)
{
	static UObject* TossPickup = UObject::GetObjectFromName(XORSTRING("Function FortniteGame.FortPickup.TossPickup"));

	static auto offset = UObject::FindOffset(XORSTRING("EnumProperty FortniteGame.FortPickup.TossPickup.InPickupSourceTypeFlags"));

	if (offset == 0)
	{
		struct
		{
			FVector FinalLocation;
			UObject* ItemOwner;
			int OverrideMaxStackCount;
			bool bToss;
		} Params;

		Params.FinalLocation = FinalLocation;
		Params.ItemOwner = ItemOwner;
		Params.OverrideMaxStackCount = OverrideMaxStackCount;
		Params.bToss = bToss;

		Globals::ProcessEvent(FortPickup, TossPickup, &Params);
	}
	else
	{
		struct
		{
			FVector FinalLocation;
			UObject* ItemOwner;
			int OverrideMaxStackCount;
			bool bToss;
			uint8_t InPickupSourceTypeFlags;
			uint8_t InPickupSpawnSource;
		} Params;

		Params.FinalLocation = FinalLocation;
		Params.ItemOwner = ItemOwner;
		Params.OverrideMaxStackCount = OverrideMaxStackCount;
		Params.bToss = bToss;
		Params.InPickupSourceTypeFlags = InPickupSourceTypeFlags;
		Params.InPickupSpawnSource = InPickupSpawnSource;

		Globals::ProcessEvent(FortPickup, TossPickup, &Params);
	}
}

static void OnRep_PrimaryPickupItemEntry(UObject* FortPickup)
{
	static UObject* OnRep_PrimaryPickupItemEntry = UObject::GetObjectFromName(XORSTRING("Function FortniteGame.FortPickup.OnRep_PrimaryPickupItemEntry"));

	Globals::ProcessEvent(FortPickup, OnRep_PrimaryPickupItemEntry, nullptr);
}

static void Onrep_TossedFromContainer(UObject* Target)
{
	static auto fn = UObject::GetObjectFromName(XORSTRING("Function FortniteGame.FortPickup.OnRep_TossedFromContainer"));

	Globals::ProcessEvent(Target, fn, nullptr);
}

static UObject* SpawnPickupAtLocation(UObject* ItemDefinition, int Count, FVector Location, bool bTossedFromContainer = false, uint8_t PickupTypeFlag = 0, uint8_t InPickupSpawnSource = 0, bool bToss = true)
{
	static UObject* PickupClass = UObject::GetObjectFromName(XORSTRING("Class FortniteGame.FortPickupAthena"));

	auto FortPickupAthena = SpawnActorFromClassObj(PickupClass, Location);

	if (bTossedFromContainer) {
		*(bool*)(__int64(FortPickupAthena) + OffsetTable::TossedFromContainer) = true;
		Onrep_TossedFromContainer(FortPickupAthena);
	}
	TossPickup(FortPickupAthena, Location, nullptr, 1, bToss, PickupTypeFlag, InPickupSpawnSource);

	*reinterpret_cast<int*>(__int64(FortPickupAthena) + OffsetTable::PrimaryPickupItemEntry + OffsetTable::ItemEntryCount) = Count;
	*reinterpret_cast<UObject**>(__int64(FortPickupAthena) + OffsetTable::PrimaryPickupItemEntry + OffsetTable::ItemEntryDefinition) = ItemDefinition;

	OnRep_PrimaryPickupItemEntry(FortPickupAthena);

	return FortPickupAthena;
}

static bool IsMatchingGuid(FGuid A, FGuid B)
{
	return A.A == B.A && A.B == B.B && A.C == B.C && A.D == B.D;
}

static bool IsInBuildMode()
{
	static auto func = UObject::GetObjectFromName(XORSTRING("Function FortniteGame.FortPlayerController.IsInBuildMode"));

	bool Ret;

	Globals::ProcessEvent(Globals::PlayerController, func, &Ret);
	return Ret;
}

static void EmptySlot(int Slot)
{
	static UObject* KismetLib = UObject::GetObjectFromName(XORSTRING("FortKismetLibrary FortniteGame.Default__FortKismetLibrary"));
	static UObject* EmptySlot = UObject::GetObjectFromName(XORSTRING("Function FortniteGame.FortKismetLibrary.EmptyQuickBarSlot"));

	struct
	{
		UObject* World;
		EFortQuickBars QuickbarIndex;
		int Slot;
		bool ReturnValue;
	} params;

	params.World = Globals::World;
	params.QuickbarIndex = EFortQuickBars::Primary;
	params.Slot = Slot;

	Globals::ProcessEvent(KismetLib, EmptySlot, &params);
}

static void RemoveItem(UObject* ItemDefinition, int amount)
{
	static UObject* KismetLib = UObject::GetObjectFromName(XORSTRING("FortKismetLibrary FortniteGame.Default__FortKismetLibrary"));
	static UObject* RemoveItem = UObject::GetObjectFromName(XORSTRING("Function FortniteGame.FortKismetLibrary.K2_RemoveItemFromPlayer"));

	struct
	{
		UObject* PlayerController;
		UObject* ItemDefinition;
		int AmountToRemove;
		bool ForceRemoval;
		int ReturnValue;
	} params;

	params.PlayerController = Globals::PlayerController;
	params.ItemDefinition = ItemDefinition;
	params.AmountToRemove = amount;
	params.ForceRemoval = true;

	Globals::ProcessEvent(KismetLib, RemoveItem, &params);
}

static void RemoveItemInternal(FGuid Item)
{
	static UObject* RemoveItemInternal = UObject::GetObjectFromName(XORSTRING("Function FortniteGame.FortQuickBars.ServerRemoveItemInternal"));

	struct
	{
		FGuid Item;
		bool bFindReplacement;
		bool bForce;
	} params;

	params.Item = Item;
	params.bFindReplacement = false;
	params.bForce = true;

	Globals::ProcessEvent(Globals::Quickbar, RemoveItemInternal, &params);
}

static void InitializeBuildingActor(UObject* BuildingActor)
{
	static UObject* InitializeKismetSpawnedBuildingActor = UObject::GetObjectFromName("Function FortniteGame.BuildingActor.InitializeKismetSpawnedBuildingActor");

	struct Params
	{
		UObject* BuildingOwner;
		UObject* SpawningController;
	};

	Params params;
	params.BuildingOwner = BuildingActor;
	params.SpawningController = Globals::PlayerController;

	Globals::ProcessEvent(BuildingActor, InitializeKismetSpawnedBuildingActor, &params);
}

FString SoftObjectPtrToString(SoftObjectPtr ptr)
{
	static auto fn = UObject::GetObjectFromName(XORSTRING("Function Engine.KismetSystemLibrary.Conv_SoftObjectReferenceToString"));
	static auto object = UObject::GetObjectFromName(XORSTRING("KismetSystemLibrary Engine.Default__KismetSystemLibrary"));

	struct
	{
		SoftObjectPtr in;
		FString out;
	} params;

	params.in = ptr;

	Globals::ProcessEvent(object, fn, &params);

	if (!params.out.IsValid())
	{
		return L"None";
	}

	return params.out;
}

FString ObjectToString(UObject* ptr)
{
	static auto fn = UObject::GetObjectFromName(XORSTRING("Function Engine.KismetStringLibrary.Conv_ObjectToString"));
	static auto object = UObject::GetObjectFromName(XORSTRING("KismetStringLibrary Engine.Default__KismetStringLibrary"));

	struct
	{
		UObject* in;
		FString out;
	} params;

	params.in = ptr;

	Globals::ProcessEvent(object, fn, &params);

	return params.out;
}

FString AssetPtrToString(AssetPtr ptr)
{
	static auto fn = UObject::GetObjectFromName(XORSTRING("Function Engine.KismetSystemLibrary.Conv_AssetToObject"));
	static auto object = UObject::GetObjectFromName(XORSTRING("KismetSystemLibrary Engine.Default__KismetSystemLibrary"));

	struct
	{
		AssetPtr in;
		UObject* out;
	} params;

	params.in = ptr;

	Globals::ProcessEvent(object, fn, &params);

	if (IsBadReadPtr(params.out) ||
		!params.out)
	{
		return L"None";
	}

	string CurrentName = params.out->GetFullNamePath();

	auto pos = CurrentName.rfind(' ');
	CurrentName = CurrentName.substr(pos + 1);

	return FString(wstring(CurrentName.begin(), CurrentName.end()).c_str());
}

UObject* AssetPtrToObject(AssetPtr ptr)
{
	static auto fn = UObject::GetObjectFromName(XORSTRING("Function Engine.KismetSystemLibrary.Conv_AssetToObject"));
	static auto object = UObject::GetObjectFromName(XORSTRING("KismetSystemLibrary Engine.Default__KismetSystemLibrary"));

	struct
	{
		AssetPtr in;
		UObject* out;
	} params;

	params.in = ptr;

	Globals::ProcessEvent(object, fn, &params);

	if (IsBadReadPtr(params.out) ||
		!params.out)
	{
		return nullptr;
	}

	return params.out;
}

FString IntToString(int var)
{
	static auto fn = UObject::GetObjectFromName(XORSTRING("Function Engine.KismetStringLibrary.Conv_IntToString"));
	static auto object = UObject::GetObjectFromName(XORSTRING("KismetStringLibrary Engine.Default__KismetStringLibrary"));

	struct
	{
		int in;
		FString out;
	} params;

	params.in = var;

	Globals::ProcessEvent(object, fn, &params);

	return params.out;
}

FString FloatToString(float var)
{
	static auto fn = UObject::GetObjectFromName(XORSTRING("Function Engine.KismetStringLibrary.Conv_FloatToString"));
	static auto object = UObject::GetObjectFromName(XORSTRING("KismetStringLibrary Engine.Default__KismetStringLibrary"));

	struct
	{
		float in;
		FString out;
	} params;

	params.in = var;

	Globals::ProcessEvent(object, fn, &params);

	return params.out;
}

static void SetupItemDefinitions()
{
#ifndef SERVERCODE
	static auto ammo = UObject::GetObjectFromName(XORSTRING("Class FortniteGame.FortAmmoItemDefinition"));
	static auto weaponranged = UObject::GetObjectFromName(XORSTRING("Class FortniteGame.FortWeaponRangedItemDefinition"));
	static auto deco = UObject::GetObjectFromName(XORSTRING("Class FortniteGame.FortDecoItemDefinition"));
	static auto resource = UObject::GetObjectFromName(XORSTRING("Class FortniteGame.FortResourceItemDefinition"));

	int ObjectCount = NewGObjectsPointer ? NewGObjectsPointer->ObjectCount : GObjectsPointer->ObjObjects.NumElements;

	for (auto i = 0; i < ObjectCount; i++)
	{
		UObject* CurrentObject = NewGObjectsPointer ? FindObjectById(i) : GObjectsPointer->ObjObjects.GetByIndex(i);

		if (!CurrentObject) continue;

		if (CurrentObject->IsA(ammo) ||
			CurrentObject->IsA(weaponranged) ||
			CurrentObject->IsA(resource))
		{
			Globals::GItemDefs.Add(CurrentObject);
		}
	}
#endif
}

static void PrepareArray()
{
#ifndef SERVERCODE
	if (!Globals::bPreparedLootArrays) {
		Globals::bPreparedLootArrays = true;
		static auto ammo = UObject::GetObjectFromName(XORSTRING("Class FortniteGame.FortAmmoItemDefinition"));
		static auto weaponranged = UObject::GetObjectFromName(XORSTRING("Class FortniteGame.FortWeaponRangedItemDefinition"));
		static auto deco = UObject::GetObjectFromName(XORSTRING("Class FortniteGame.FortDecoItemDefinition"));
		static auto resource = UObject::GetObjectFromName(XORSTRING("Class FortniteGame.FortResourceItemDefinition"));

		auto OnRep_CurrentPlaylistData = UObject::GetObjectFromName(XORSTRING("Function FortniteGame.FortGameStateAthena.OnRep_CurrentPlaylistData"));
		auto OnRep_CurrentPlaylistInfo = UObject::GetObjectFromName(XORSTRING("Function FortniteGame.FortGameStateAthena.OnRep_CurrentPlaylistInfo"));

		if (OnRep_CurrentPlaylistData)
		{
			auto Playlist = *(UObject**)(__int64(Globals::GameState) + UObject::FindOffset(XORSTRING("ObjectProperty FortniteGame.FortGameStateAthena.CurrentPlaylistData")));
			Globals::AthenaLoot = SoftObjectPtrToObject(*(SoftObjectPtr*)(__int64(Playlist) + UObject::FindOffset(XORSTRING("SoftObjectProperty FortniteGame.FortPlaylist.LootPackages"))));
		}
		else if (OnRep_CurrentPlaylistInfo)
		{
			auto Playlist = *reinterpret_cast<UObject**>(__int64(Globals::GameState) + UObject::FindOffset(XORSTRING("StructProperty FortniteGame.FortGameStateAthena.CurrentPlaylistInfo")) + UObject::FindOffset(XORSTRING("ObjectProperty FortniteGame.PlaylistPropertyArray.BasePlaylist")));
			Globals::AthenaLoot = SoftObjectPtrToObject(*(SoftObjectPtr*)(__int64(Playlist) + UObject::FindOffset(XORSTRING("SoftObjectProperty FortniteGame.FortPlaylist.LootPackages"))));
		}
		else if (IsBadReadPtr(Globals::AthenaLoot))
		{
			string Path = XORSTRING("/Game/Items/Datatables/AthenaLootPackages_Client");
			Globals::AthenaLoot = Globals::StaticLoadObject(UObject::GetObjectFromName(XORSTRING("Class Engine.DataTable")), NULL, wstring(Path.begin(), Path.end()).c_str(), nullptr, 0, nullptr, true);
		}

		TArray<FString> ItemDefinitions = GetDataTableColumnAsString(Globals::AthenaLoot, StringToName(L"ItemDefinition"));
		TArray<FString> Count = GetDataTableColumnAsString(Globals::AthenaLoot, StringToName(L"Count"));
		TArray<FString> Weight = GetDataTableColumnAsString(Globals::AthenaLoot, StringToName(L"Weight"));

		if (ItemDefinitions.Num() == 0 ||
			Count.Num() == 0 ||
			Weight.Num() == 0)
		{

			auto names = GetTableRowNames(Globals::AthenaLoot);

			for (auto i = 0; i < names.Num(); i++)
			{
				auto currentPtr = Globals::FindRowUnchecked(Globals::AthenaLoot, names[i], false);

				auto ItemDefinition = *(SoftObjectPtr*)(__int64(currentPtr) + OffsetTable::ItemDefinitionLoot);
				auto ItemDefinition416 = *(AssetPtr*)(__int64(currentPtr) + OffsetTable::ItemDefinitionLoot416);
				auto CurrentCount = *(int*)(__int64(currentPtr) + OffsetTable::CountLoot);
				auto CurrentWeight = *(float*)(__int64(currentPtr) + OffsetTable::Weight);

				if (OffsetTable::ItemDefinitionLoot > 0) {
					ItemDefinitions.Add(SoftObjectPtrToString(ItemDefinition));
				}
				else {
					ItemDefinitions.Add(AssetPtrToString(ItemDefinition416));
				}
				Count.Add(IntToString(CurrentCount));
				Weight.Add(FloatToString(CurrentWeight));

				if (ItemDefinitions[i].ToString().find("None") == string::npos)
				{
					if (stoi(Weight[i].ToString()) > 0)
					{
						auto CurrentItemDef = UObject::GetObjectFromName(ItemDefinitions[i].ToString(), false, true, true, Globals::GItemDefs);
						if (CurrentItemDef) {
							auto CurrentCount = stoi(Count[i].ToString());
							if (CurrentItemDef->IsA(ammo))
							{
								Globals::AmmoItemDefs.Add(CurrentItemDef);
								Globals::AmmoItemCount.Add(CurrentCount);
							}
							else if (CurrentItemDef->IsA(weaponranged))
							{
								if (CurrentItemDef->GetObjectNameString().find("WID") == string::npos)
								{
									Globals::ConsumablesItemDefs.Add(CurrentItemDef);
									Globals::ConsumablesItemCount.Add(CurrentCount);
								}
								else
								{
									Globals::WeaponItemDefs.Add(CurrentItemDef);
									Globals::WeaponItemCount.Add(CurrentCount);
								}
							}
							else if (CurrentItemDef->IsA(deco))
							{
								Globals::TrapsItemDefs.Add(CurrentItemDef);
								Globals::TrapsItemCount.Add(CurrentCount);
							}
							else if (CurrentItemDef->IsA(resource))
							{
								Globals::ResourceItemDefs.Add(CurrentItemDef);
								Globals::ResourceItemCount.Add(CurrentCount);
							}
						}
					}
				}
			}
			printf(XORSTRING("All arrays have been set-up!\n"));
			return;
		}

		for (int i = 0; i < ItemDefinitions.Num(); i++)
		{
			if (ItemDefinitions[i].ToString().find("None") == string::npos)
			{
				if (stoi(Weight[i].ToString()) > 0)
				{
					auto CurrentItemDef = UObject::GetObjectFromName(ItemDefinitions[i].ToString(), false, true, true, Globals::GItemDefs);
					if (CurrentItemDef) {
						auto CurrentCount = stoi(Count[i].ToString());
						if (CurrentItemDef->IsA(ammo))
						{
							Globals::AmmoItemDefs.Add(CurrentItemDef);
							Globals::AmmoItemCount.Add(CurrentCount);
						}
						else if (CurrentItemDef->IsA(weaponranged))
						{
							if (CurrentItemDef->GetObjectNameString().find("WID") == string::npos)
							{
								Globals::ConsumablesItemDefs.Add(CurrentItemDef);
								Globals::ConsumablesItemCount.Add(CurrentCount);
							}
							else
							{
								Globals::WeaponItemDefs.Add(CurrentItemDef);
								Globals::WeaponItemCount.Add(CurrentCount);
							}
						}
						else if (CurrentItemDef->IsA(deco))
						{
							Globals::TrapsItemDefs.Add(CurrentItemDef);
							Globals::TrapsItemCount.Add(CurrentCount);
						}
						else if (CurrentItemDef->IsA(resource))
						{
							Globals::ResourceItemDefs.Add(CurrentItemDef);
							Globals::ResourceItemCount.Add(CurrentCount);
						}
					}
				}
			}
		}

		printf(XORSTRING("All arrays have been set-up!\n"));
	}
#endif
}

static void InteractWith(PVOID Params)
{
	static UObject* ChestsSound = Globals::ChestSound;
	static UObject* AmmoBoxSound = Globals::AmmoBoxSound;

	struct ServerAttemptInteract
	{
		UObject* ReceivingActor;
		UObject* InteractComponent;
		char InteractType;
	};

	auto CurrentParams = (ServerAttemptInteract*)Params;

	if (CurrentParams->ReceivingActor->GetFullName().starts_with(XORSTRING("Tiered_")))
	{
		struct BitField
		{
			char bAlwaysShowContainer : 1; // 0xeb9(0x01)
			char bAlwaysMaintainLoot : 1; // 0xeb9(0x01)
			char bDestroyContainerOnSearch : 1; // 0xeb9(0x01)
			char bAlreadySearched : 1; // 0xeb9(0x01)
		};

		auto ContainerBitField = reinterpret_cast<BitField*>(__int64(CurrentParams->ReceivingActor) + __int64(OffsetTable::bAlreadySearchedOffset));
		
		auto ContainerLocation = GetLocation(CurrentParams->ReceivingActor);
		auto ContainerRotation = GetRotation(CurrentParams->ReceivingActor);

		auto LootTierGroup = *(FName*)(__int64(CurrentParams->ReceivingActor) + OffsetTable::LootTierGroup);
		auto LootTierGroupString = LootTierGroup.ToString();

		auto ForwardVector = GetRightVectorFromRotation(ContainerRotation);
		ForwardVector.X = ForwardVector.X * 64;
		ForwardVector.Y = ForwardVector.Y * 64;
		ContainerLocation = SumVectors(ContainerLocation, ForwardVector);

		//If statement to prevent a bug in 1.8
		if (!ContainerBitField->bAlreadySearched) {
			if (LootTierGroupString.find("Loot_Treasure") != string::npos)
			{
				ClientPlaySoundAtLocation(Globals::PlayerController, ChestsSound, ContainerLocation, 1, 1);

				int WeaponRandomIndex = rand() % Globals::WeaponItemDefs.Num();
				int ResourceRandomIndex = rand() % Globals::ResourceItemDefs.Num();
				int ConsumableRandomIndex = rand() % Globals::ConsumablesItemDefs.Num();
				SpawnPickupAtLocation(Globals::WeaponItemDefs[WeaponRandomIndex], Globals::WeaponItemCount[WeaponRandomIndex], ContainerLocation, true, 6, 2);
				SpawnPickupAtLocation(Globals::ResourceItemDefs[ResourceRandomIndex], Globals::ResourceItemCount[ResourceRandomIndex], ContainerLocation, true, 6, 2);
				SpawnPickupAtLocation(Globals::ConsumablesItemDefs[ConsumableRandomIndex], Globals::ConsumablesItemCount[ConsumableRandomIndex], ContainerLocation, true, 6, 2);

				auto AmmoData = *(SoftObjectPtr*)(__int64(Globals::WeaponItemDefs[WeaponRandomIndex]) + OffsetTable::AmmoData);
				auto CurrentAmmoData = OffsetTable::AmmoData > 0 ? SoftObjectPtrToObject(AmmoData) : AssetPtrToObject(*(AssetPtr*)(__int64(Globals::WeaponItemDefs[WeaponRandomIndex]) + OffsetTable::AmmoData416));
				for (auto j = 0; j < Globals::AmmoItemDefs.Num(); j++)
				{
					if (Globals::AmmoItemDefs[j] == CurrentAmmoData)
					{
						SpawnPickupAtLocation(Globals::AmmoItemDefs[j], Globals::AmmoItemCount[j], ContainerLocation, false, 0, 0, false);
						break;
					}
				}

			}
			else if (LootTierGroupString.find("Loot_Ammo") != string::npos)
			{
				ClientPlaySoundAtLocation(Globals::PlayerController, AmmoBoxSound, ContainerLocation, 1, 1);

				auto MinIndex = *(float*)(__int64(CurrentParams->ReceivingActor) + OffsetTable::NumItemsToDropRange);
				auto MaxIndex = *(float*)(__int64(CurrentParams->ReceivingActor) + OffsetTable::NumItemsToDropRange + 4);

				for (MinIndex; MinIndex < MaxIndex; MinIndex++)
				{
					while (true)
					{
						auto RandomIndex = rand() % Globals::AmmoItemDefs.Num();
						if (Globals::LastItemDefinition != Globals::AmmoItemDefs[RandomIndex]) {
							SpawnPickupAtLocation(Globals::AmmoItemDefs[RandomIndex], Globals::AmmoItemCount[RandomIndex], ContainerLocation, true, 6, 2);
							Globals::LastItemDefinition = Globals::AmmoItemDefs[RandomIndex];
							break;
						}
						else
						{
							continue;
						}
					}
				}
			}
		}
		ContainerBitField->bAlreadySearched = true;
		OnRep_bAlreadySearched(CurrentParams->ReceivingActor);
	}
}

static void InventoryDrop(void* Params)
{
	struct ServerAttemptInventoryDropParams
	{
		FGuid ItemGuid;
		int Count;
	};

	auto PawnLocation = GetLocation(Globals::PlayerPawn);

	auto ItemInstances = reinterpret_cast<TArray<UObject*>*>(__int64(Globals::Inventory) + __int64(OffsetTable::FortInventory) + __int64(OffsetTable::ItemInstances));
	auto RequestedGuid = ((ServerAttemptInventoryDropParams*)Params)->ItemGuid;

	for (int i = 0; i < ItemInstances->Num(); i++)
	{
		auto CurrentItemInstance = ItemInstances->operator[](i);
		auto CurrentGuid = GetGuid(CurrentItemInstance);

		if (IsMatchingGuid(CurrentGuid, RequestedGuid))
		{
			// we know this weapon is the one we want, fetch item definition from ItemEntry
			auto ItemDefinition = reinterpret_cast<UObject**>(__int64(CurrentItemInstance) + __int64(OffsetTable::ItemEntry) + __int64(OffsetTable::ItemEntryDefinition));
			
			TArray<QuickbarSlot> QuickbarSlots = *reinterpret_cast<TArray<QuickbarSlot>*>(reinterpret_cast<uintptr_t>(Globals::Quickbar) + OffsetTable::PrimaryQuickbar + OffsetTable::Slots);

			if (ItemDefinition)
			{
				SpawnPickupAtLocation(*ItemDefinition, ((ServerAttemptInventoryDropParams*)Params)->Count, PawnLocation);

				reinterpret_cast<TArray<UObject*>*>(__int64(Globals::Inventory) + __int64(OffsetTable::FortInventory) + __int64(OffsetTable::ItemInstances))->Remove(i);
				
				int ItemEntrySize = OffsetTable::MultipleItemEntries - OffsetTable::PrimaryPickupItemEntry;

				switch (ItemEntrySize)
				{
				case 0xA8: {
					struct ItemEntrySize { char size[0xA8]; }; reinterpret_cast<TArray<ItemEntrySize>*>(reinterpret_cast<uintptr_t>(Globals::Inventory) + OffsetTable::FortInventory + OffsetTable::ItemEntries)->Remove(i);
					break;
				}
				case 0xB0: {
					struct ItemEntrySize { char size[0xB0]; }; reinterpret_cast<TArray<ItemEntrySize>*>(reinterpret_cast<uintptr_t>(Globals::Inventory) + OffsetTable::FortInventory + OffsetTable::ItemEntries)->Remove(i);
					break;
				}
				case 0xC0: {
					struct ItemEntrySize { char size[0xC0]; }; reinterpret_cast<TArray<ItemEntrySize>*>(reinterpret_cast<uintptr_t>(Globals::Inventory) + OffsetTable::FortInventory + OffsetTable::ItemEntries)->Remove(i);
					break;
				}
				case 0xC8: {
					struct ItemEntrySize { char size[0xC8]; }; reinterpret_cast<TArray<ItemEntrySize>*>(reinterpret_cast<uintptr_t>(Globals::Inventory) + OffsetTable::FortInventory + OffsetTable::ItemEntries)->Remove(i);
					break;
				}
				case 0xD0: {
					struct ItemEntrySize { char size[0xD0]; }; reinterpret_cast<TArray<ItemEntrySize>*>(reinterpret_cast<uintptr_t>(Globals::Inventory) + OffsetTable::FortInventory + OffsetTable::ItemEntries)->Remove(i);
					break;
				}
				case 0x120: {
					struct ItemEntrySize { char size[0x120]; }; reinterpret_cast<TArray<ItemEntrySize>*>(reinterpret_cast<uintptr_t>(Globals::Inventory) + OffsetTable::FortInventory + OffsetTable::ItemEntries)->Remove(i);
					break;
				}
				}

				for (int j = 0; j < 6; j++)
				{
					if (GetQuickbarItem(EFortQuickBars::Primary, j))
					{
						ActivateSlot(EFortQuickBars::Primary, j);
						break;
					}
				}

				UpdateInventory();

				
			}
		}
	}
}

static UObject* CreatePlayer()
{
	static UObject* CreatePlayer = UObject::GetObjectFromName(XORSTRING("Function Engine.GameplayStatics.CreatePlayer"));

	struct {
		UObject* WorldContextObject;
		int ControllerID;
		bool bSpawnPlayerController;
		UObject* ReturnPlayerController;
	} params;

	params.WorldContextObject = Globals::World;
	params.ControllerID = -1;
	params.bSpawnPlayerController = true;

	Globals::ProcessEvent(Globals::GameplayStatics, CreatePlayer, &params);

	return params.ReturnPlayerController;
	
}

static void Pickup(PVOID Params)
{
	struct ServerHandlePickupParams
	{
		UObject* Pickup;
		float InFlyTime;
		FVector InStartDirection;
		bool bPlayPickupSound;
	};

	auto CurrentParams = (ServerHandlePickupParams*)Params;

	if (CurrentParams->Pickup != nullptr)
	{
		// get world item definition from item entry
		UObject** WorldItemDefinition = reinterpret_cast<UObject**>(__int64(CurrentParams->Pickup) + OffsetTable::PrimaryPickupItemEntry + OffsetTable::ItemEntryDefinition);
		int ItemEntryCount = *reinterpret_cast<int*>(__int64(CurrentParams->Pickup) + OffsetTable::PrimaryPickupItemEntry + OffsetTable::ItemEntryCount);
		
		for (int i = 0; i < 6; i++)
		{
			if (!GetQuickbarItem(EFortQuickBars::Primary, i))
			{
				// give player item
				AddToInventory(CreateItem(*WorldItemDefinition, ItemEntryCount), EFortQuickBars::Primary, i);
				UpdateInventory();

				// destroy pickup in world
				Destroy(CurrentParams->Pickup);

				break;
			}

			if (i == 5)
			{
				int CurrentFocusedSlot = *reinterpret_cast<int*>(__int64(Globals::Quickbar) + __int64(OffsetTable::PrimaryQuickbar) + __int64(OffsetTable::CurrentFocusedSlot));

				if (CurrentFocusedSlot != 0)
				{
					auto CurrentDef = GetDefinition(GetQuickbarItem(EFortQuickBars::Primary, CurrentFocusedSlot));
					auto CurrentGuid = GetGuid(GetQuickbarItem(EFortQuickBars::Primary, CurrentFocusedSlot));

					SpawnPickupAtLocation(CurrentDef, 1, GetLocation(Globals::PlayerPawn));

					AddToInventory(CreateItem(*WorldItemDefinition, ItemEntryCount), EFortQuickBars::Primary, CurrentFocusedSlot);

					Destroy(CurrentParams->Pickup);

					auto ItemInstances = *reinterpret_cast<TArray<UObject*>*>(__int64(Globals::Inventory) + __int64(OffsetTable::FortInventory) + __int64(OffsetTable::ItemInstances));
					
					for (int i = 0; i < ItemInstances.Num(); i++)
					{
						if (IsMatchingGuid(GetGuid(ItemInstances[i]), CurrentGuid))
						{
							reinterpret_cast<TArray<UObject*>*>(__int64(Globals::Inventory) + __int64(OffsetTable::FortInventory) + __int64(OffsetTable::ItemInstances))->Remove(i);

							int ItemEntrySize = OffsetTable::MultipleItemEntries - OffsetTable::PrimaryPickupItemEntry;

							switch (ItemEntrySize)
							{
							case 0xA8: {
								struct ItemEntrySize { char size[0xA8]; }; reinterpret_cast<TArray<ItemEntrySize>*>(reinterpret_cast<uintptr_t>(Globals::Inventory) + OffsetTable::FortInventory + OffsetTable::ItemEntries)->Remove(i);
								break;
							}
							case 0xB0: {
								struct ItemEntrySize { char size[0xB0]; }; reinterpret_cast<TArray<ItemEntrySize>*>(reinterpret_cast<uintptr_t>(Globals::Inventory) + OffsetTable::FortInventory + OffsetTable::ItemEntries)->Remove(i);
								break;
							}
							case 0xC0: {
								struct ItemEntrySize { char size[0xC0]; }; reinterpret_cast<TArray<ItemEntrySize>*>(reinterpret_cast<uintptr_t>(Globals::Inventory) + OffsetTable::FortInventory + OffsetTable::ItemEntries)->Remove(i);
								break;
							}
							case 0xC8: {
								struct ItemEntrySize { char size[0xC8]; }; reinterpret_cast<TArray<ItemEntrySize>*>(reinterpret_cast<uintptr_t>(Globals::Inventory) + OffsetTable::FortInventory + OffsetTable::ItemEntries)->Remove(i);
								break;
							}
							case 0xD0: {
								struct ItemEntrySize { char size[0xD0]; }; reinterpret_cast<TArray<ItemEntrySize>*>(reinterpret_cast<uintptr_t>(Globals::Inventory) + OffsetTable::FortInventory + OffsetTable::ItemEntries)->Remove(i);
								break;
							}
							case 0x120: {
								struct ItemEntrySize { char size[0x120]; }; reinterpret_cast<TArray<ItemEntrySize>*>(reinterpret_cast<uintptr_t>(Globals::Inventory) + OffsetTable::FortInventory + OffsetTable::ItemEntries)->Remove(i);
								break;
							}
							}

							break;
						}
					}

					UpdateInventory();
				}
			}

		}
	}
}


static void StartListening()
{
#if defined(SERVERCODE)
	Globals::Beacon = SpawnActorFromClass("Class OnlineSubsystemUtils.OnlineBeaconHost", FVector{0,0,100});

	if (Globals::InitHost(Globals::Beacon)) printf(XORSTRING("EraServerLog: started listening!\n"));
	else printf(XORSTRING("EraServerLog: listening FAILED!\n"));

	Globals::PauseBeaconRequests(Globals::Beacon, false);

	Globals::bIsListening = true;
#endif
}

static void SetMesh(UObject* Pawn, UObject* mesh)
{
	static int MeshOffset = UObject::FindOffset(XORSTRING("ObjectProperty Engine.Character.Mesh"));
	auto object = *(UObject**)(__int64(Pawn) + MeshOffset);

	static auto fn = UObject::GetObjectFromName(XORSTRING("Function Engine.SkinnedMeshComponent.SetSkeletalMesh"));

	struct
	{
		UObject* mesh;
		bool reinitpose;
	} params;

	params.mesh = mesh;
	params.reinitpose = false;

	Globals::ProcessEvent(object, fn, &params);
}

static void SetIsSleeping(UObject* Target, bool bSetIsSleeping)
{
	static auto fn = UObject::GetObjectFromName(XORSTRING("Function FortniteGame.FortAIController.SetIsSleeping"));

	Globals::ProcessEvent(Target, fn, &bSetIsSleeping);
}

static void SetGoalActor(UObject* Target, UObject* Actor, bool bAlwaysKnown)
{
	static auto fn = UObject::GetObjectFromName(XORSTRING("Function FortniteGame.FortAIController.SetGoalActor"));

	struct
	{
		UObject* Actor;
		bool bLocationAlwaysKnown;
	} params;

	params.Actor = Actor;
	params.bLocationAlwaysKnown = bAlwaysKnown;

	Globals::ProcessEvent(Target, fn, &bAlwaysKnown);
}

static void WakeUp(UObject* Target)
{
	static auto fn = UObject::GetObjectFromName(XORSTRING("Function FortniteGame.FortAIController.WakeUp"));

	Globals::ProcessEvent(Target, fn, nullptr);
}

static void ServerLoadingScreenDropped(UObject* target)
{
	static auto fn = UObject::GetObjectFromName(XORSTRING("Function FortniteGame.FortPlayerController.ServerLoadingScreenDropped"));

	Globals::ProcessEvent(target, fn, nullptr);
}

static void ONREP_hasstartedplaying(UObject* Target)
{
	static auto fn = UObject::GetObjectFromName(XORSTRING("Function FortniteGame.FortPlayerState.OnRep_bHasStartedPlaying"));

	Globals::ProcessEvent(Target, fn, nullptr);
}

static void CheatScript(void* Params)
{
	FString* ScriptNameF = (FString*)Params;

	if (!ScriptNameF->IsValid())
	{
		return;
	}

	std::string ScriptName = ScriptNameF->ToString();
	std::string ScriptNameW(ScriptName.begin(), ScriptName.end());
	std::string arg;
	std::wstring argW;

	if (ScriptName.find(" ") != std::string::npos) arg = ScriptName.substr(ScriptNameW.find(" ") + 1);
	std::wstring a(arg.begin(), arg.end());
	argW = a;

#ifdef TESTINGVERSION
	printf("\ncheatscript %s\n", ScriptName.c_str());
#endif // TESTINGVERSION

	if (strstr(ScriptNameW.c_str(), XORSTRING("SpawnPickup")))
	{
		int ObjectCount = NewGObjectsPointer ? NewGObjectsPointer->ObjectCount : GObjectsPointer->ObjObjects.NumElements;

		for (int i = 0; i < ObjectCount; i++)
		{
			UObject* Object = NewGObjectsPointer ? FindObjectById(i) : GObjectsPointer->ObjObjects.GetByIndex(i);

			if (Object == nullptr)
			{
				continue;
			}

			if (Object->GetFullName().find(arg) != std::string::npos)
			{
				if (Object->GetFullName().find(XORSTRING("ItemDefinition")) != std::wstring::npos)
				{
					FVector ActorLocation = GetLocation(Globals::PlayerPawn);
					SpawnPickupAtLocation(Object, 1, ActorLocation);
					return;
				}
			}
		}

		Say(XORSTRING(L"ItemDefinition Was not found!"));

	}
	else if (strstr(ScriptNameW.c_str(), XORSTRING("SpawnCube")))
	{
		static auto CubeObject = UObject::GetObjectFromName(XORSTRING("CUBE_C Level_CUBE.Level_CUBE.PersistentLevel.CUBE_2"));

		if (CubeObject)
		{
			Globals::ProcessEvent(CubeObject, UObject::GetObjectFromName(XORSTRING("Function CUBE.CUBE_C.CubeSpawnEffect")), nullptr);
			Globals::ProcessEvent(CubeObject, UObject::GetObjectFromName(XORSTRING("Function CUBE.CUBE_C.SpawnCube")), nullptr);
		}
	}
	else if (strstr(ScriptNameW.c_str(), XORSTRING("SpawnBot")))
	{
#ifndef BOTS
		Say(XORSTRING(L"Bots are coming in Era 1.5"));
#elif defined(BOTS)
		static auto fn = UObject::GetObjectFromName(XORSTRING("Function Engine.Controller.OnRep_PlayerState"));
		static auto _fn = UObject::GetObjectFromName(XORSTRING("Function Engine.Pawn.OnRep_PlayerState"));

		auto botcontroller = SpawnActorFromClass(XORSTRING("Class FortniteGame.FortAIController"), GetLocation(Globals::PlayerPawn));
		auto botpawn = SpawnActorFromClass(XORSTRING("BlueprintGeneratedClass PlayerPawn_Athena.PlayerPawn_Athena_C"), GetLocation(Globals::PlayerPawn)); 
		auto botstate = SpawnActorFromClass(XORSTRING("Class FortniteGame.FortPlayerStateAthena"), GetLocation(Globals::PlayerPawn));

		Possess(botcontroller, botpawn);

		*(UObject**)(__int64(botcontroller) + OffsetTable::PlayerState) = botstate;
		*(UObject**)(__int64(botpawn) + OffsetTable::PawnPlayerState) = botstate;

		Globals::ProcessEvent(botcontroller, fn, nullptr);
		Globals::ProcessEvent(botpawn, _fn, nullptr);

		SetMaxHealth(botpawn, 100);
		SetHealth(botpawn, 100);

		ServerChoosePart(EFortCustomPartType::Body, UObject::GetObjectFromName(XORSTRING("CustomCharacterPart F_Med_Soldier_01.F_Med_Soldier_01")), botpawn);
		ServerChoosePart(EFortCustomPartType::Head, UObject::GetObjectFromName(XORSTRING("CustomCharacterPart F_Med_Head1.F_Med_Head1")), botpawn);

		Globals::ProcessEvent(botstate, UObject::GetObjectFromName(XORSTRING("Function FortniteGame.FortPlayerState.OnRep_CharacterParts")), nullptr);

		*reinterpret_cast<ENetRole*>(__int64(botpawn) + OffsetTable::RoleOffset) = ENetRole::ROLE_Authority;
#endif
	}
#if defined(CUSTOMCHEATSCRIPTS)
	else if (strstr(ScriptNameW.c_str(), XORSTRING("Splitscreen")))
	{
		CreatePlayer();
	}
	else if (strstr(ScriptNameW.c_str(), XORSTRING("EquipWeapon")))
	{
		auto CurrentItem = CreateItem(UObject::GetObjectFromName(XORSTRING("FortWeaponRangedItemDefinition MountedTurret_Weapon.MountedTurret_Weapon")), 0);
		EquipWeapon(GetDefinition(CurrentItem), GetGuid(CurrentItem));
		GrantAbility(UObject::GetObjectFromName(XORSTRING("BlueprintGeneratedClass GA_FerretVehicleWeapon.GA_FerretVehicleWeapon_C")));
	}
#endif
#if defined(SERVERCODE)
	else if (strstr(ScriptNameW.c_str(), XORSTRING("StartListening")))
	{
		if (!Globals::bIsListening) {
			SwitchLevel();
			Globals::bPressedPlay = true;
}
		else printf("ERROR: A listen server is already running!\n");
	}
#endif
}

static void BuildWith(FString Resource)
{
	static UObject* BuildWith = UObject::GetObjectFromName(XORSTRING("Function FortniteGame.FortCheatManager.BuildWith"));

	Globals::ProcessEvent(Globals::CheatManager, BuildWith, &Resource);
}

static void ServerInternalEquipWeapon(UObject* FortWeapon)
{
	static auto ServerInternalEquipWeapon = UObject::GetObjectFromName(XORSTRING("Function FortniteGame.FortPawn.ServerInternalEquipWeapon"));

	if (ServerInternalEquipWeapon) Globals::ProcessEvent(Globals::PlayerPawn, ServerInternalEquipWeapon, &FortWeapon);
}

static void ClientEquipItem(FGuid ItemGUID)
{
	static auto ClientEquipItem = UObject::GetObjectFromName(XORSTRING("Function FortniteGame.FortPlayerController.ClientEquipItem"));

	struct
	{
		FGuid ItemGuid;
		bool bForceExecute;
	} Params;

	Params.ItemGuid = ItemGUID;
	Params.bForceExecute = false;

	if (ClientEquipItem) Globals::ProcessEvent(Globals::PlayerController, ClientEquipItem, &Params);
}

static void OnRep_WeaponActivted()
{
	static auto ReplicateWeaponActivated = UObject::GetObjectFromName(XORSTRING("Function FortniteGame.FortPawn.OnRep_WeaponActivated"));

	Globals::ProcessEvent(Globals::PlayerPawn, ReplicateWeaponActivated, nullptr);
}

static void Equip(FGuid Guid, UObject* FortWeapon)
{
	auto ItemInstances = reinterpret_cast<TArray<UObject*>*>(__int64(Globals::Inventory) + __int64(OffsetTable::FortInventory) + __int64(OffsetTable::ItemInstances));


	if (FortWeapon)
	{
		auto WeaponItemDefinition = *(UObject**)(__int64(FortWeapon) + OffsetTable::WeaponData);
		auto WeaponItemGUID = *(FGuid*)(__int64(FortWeapon) + OffsetTable::FortWeaponGuid);

		EquipWeapon(WeaponItemDefinition, WeaponItemGUID);

		ServerInternalEquipWeapon(FortWeapon);

		return; 
	}

	for (int i = 0; i < ItemInstances->Num(); i++)
	{
		auto CurrentItemInstance = ItemInstances->operator[](i);

		// Does the GUID in the inventory match with the one we are trying to equip
		if (IsMatchingGuid(GetGuid(CurrentItemInstance), Guid))
		{
			auto CurrentFortWeapon = EquipWeapon(GetDefinition(CurrentItemInstance), Guid);

			ServerInternalEquipWeapon(CurrentFortWeapon);

			if (Globals::EngineVersionString.find("4.16") != string::npos ||
				Globals::EngineVersionString.find("4.19") != string::npos)
			{
				if (GetDefinition(CurrentItemInstance)->GetObjectNameString() == "BuildingItemData_Wall")
				{
					static UObject* WallClass = UObject::GetObjectFromName(XORSTRING("BlueprintGeneratedClass PBWA_W1_Solid.PBWA_W1_Solid_C"));
					*(UObject**)(__int64(Globals::PlayerController) + OffsetTable::CurrentBuildableClassOffset) = WallClass;
					UObject* CurrentBuildPreviewMarker = *(UObject**)(__int64(Globals::PlayerController) + OffsetTable::BuildPreviewMarker);
					SetActorHiddenInGame(CurrentBuildPreviewMarker, true);
					CurrentBuildPreviewMarker = Globals::WallPreview;
					SetActorHiddenInGame(CurrentBuildPreviewMarker, false);
					BuildWith(L"Wood");
					BuildWith(L"Stone");
					BuildWith(L"Metal");
					BuildWith(L"Wood");
				}
				else if (GetDefinition(CurrentItemInstance)->GetObjectNameString() == "BuildingItemData_Floor")
				{
					static UObject* FloorClass = UObject::GetObjectFromName(XORSTRING("BlueprintGeneratedClass PBWA_W1_Floor.PBWA_W1_Floor_C"));
					*(UObject**)(__int64(Globals::PlayerController) + OffsetTable::CurrentBuildableClassOffset) = FloorClass;
					UObject* CurrentBuildPreviewMarker = *(UObject**)(__int64(Globals::PlayerController) + OffsetTable::BuildPreviewMarker);
					SetActorHiddenInGame(CurrentBuildPreviewMarker, true);
					CurrentBuildPreviewMarker = Globals::FloorPreview;
					SetActorHiddenInGame(CurrentBuildPreviewMarker, false);
					BuildWith(L"Wood");
					BuildWith(L"Stone");
					BuildWith(L"Metal");
					BuildWith(L"Wood");
				}
				else if (GetDefinition(CurrentItemInstance)->GetObjectNameString() == "BuildingItemData_Stair_W")
				{
					static UObject* StairClass = UObject::GetObjectFromName(XORSTRING("BlueprintGeneratedClass PBWA_W1_StairW.PBWA_W1_StairW_C"));
					*(UObject**)(__int64(Globals::PlayerController) + OffsetTable::CurrentBuildableClassOffset) = StairClass;
					UObject* CurrentBuildPreviewMarker = *(UObject**)(__int64(Globals::PlayerController) + OffsetTable::BuildPreviewMarker);
					SetActorHiddenInGame(CurrentBuildPreviewMarker, true);
					CurrentBuildPreviewMarker = Globals::StairsPreview;
					SetActorHiddenInGame(CurrentBuildPreviewMarker, false);
					BuildWith(L"Wood");
					BuildWith(L"Stone");
					BuildWith(L"Metal");
					BuildWith(L"Wood");
				}
				else if (GetDefinition(CurrentItemInstance)->GetObjectNameString() == "BuildingItemData_RoofS")
				{
					static UObject* RoofClass = UObject::GetObjectFromName(XORSTRING("BlueprintGeneratedClass PBWA_W1_RoofC.PBWA_W1_RoofC_C"));
					*(UObject**)(__int64(Globals::PlayerController) + OffsetTable::CurrentBuildableClassOffset) = RoofClass;
					UObject* CurrentBuildPreviewMarker = *(UObject**)(__int64(Globals::PlayerController) + OffsetTable::BuildPreviewMarker);
					SetActorHiddenInGame(CurrentBuildPreviewMarker, true);
					CurrentBuildPreviewMarker = Globals::RoofPreview;
					SetActorHiddenInGame(CurrentBuildPreviewMarker, false);
					BuildWith(L"Wood");
					BuildWith(L"Stone");
					BuildWith(L"Metal");
					BuildWith(L"Wood");
				}
			}
		}
	}

}

static void ExecuteConsoleCommand(FString ConsoleCommand)
{
	static UObject* KismetSystemLibrary = UObject::GetObjectFromName("KismetSystemLibrary Engine.Default__KismetSystemLibrary");
	static UObject* ExecuteConsoleCommand = UObject::GetObjectFromName("Function Engine.KismetSystemLibrary.ExecuteConsoleCommand");

	struct
	{
		UObject* World;
		FString Command;
		UObject* PlayerController;
	} params;

	params.World = Globals::World;
	params.Command = ConsoleCommand;
	params.PlayerController = Globals::PlayerController;

	Globals::ProcessEvent(KismetSystemLibrary, ExecuteConsoleCommand, &params);
}

static void DisableFortLogging()
{

}

static void EquipPickaxe()
{
	auto PID = OffsetTable::CustomizationLoadout > 0 ? *(UObject**)(__int64(Globals::PlayerController) + OffsetTable::CustomizationLoadout + UObject::FindOffset(XORSTRING("ObjectProperty FortniteGame.FortAthenaLoadout.Pickaxe"))) :
		*(UObject**)(__int64(Globals::PlayerController) + UObject::FindOffset(XORSTRING("StructProperty FortniteGame.FortPlayerController.CosmeticLoadoutPC")) + UObject::FindOffset(XORSTRING("ObjectProperty FortniteGame.FortAthenaLoadout.Pickaxe")));

	if (!IsBadReadPtr(PID) && PID) {
		Globals::PickaxeItemDefinition = *(UObject**)(__int64(PID) + UObject::FindOffset(XORSTRING("ObjectProperty FortniteGame.AthenaPickaxeItemDefinition.WeaponDefinition")));
	}
	else if (!PID)
	{
		Globals::PickaxeItemDefinition = UObject::GetObjectFromName(XORSTRING("FortWeaponMeleeItemDefinition WID_Harvest_Pickaxe_Athena_C_T01.WID_Harvest_Pickaxe_Athena_C_T01"));
	}
}

static UObject* GetAnimationHardReference(UObject* Target)
{
	static UObject* GetAnimationHardReference = UObject::GetObjectFromName(XORSTRING("Function FortniteGame.FortMontageItemDefinitionBase.GetAnimationHardReference"));

	struct
	{
		EFortCustomBodyType bodytype;
		EFortCustomGender gender;
		UObject* ReturnValue;
	} Params;

	Params.bodytype = EFortCustomBodyType::All;
	Params.gender = EFortCustomGender::Both;

	Globals::ProcessEvent(Target, GetAnimationHardReference, &Params);

	return Params.ReturnValue;
}

static UObject* GetAnimationInstance(UObject* TargetPawn)
{
	static int MeshOffset = UObject::FindOffset(XORSTRING("ObjectProperty Engine.Character.Mesh"));
	auto Mesh = *(UObject**)(__int64(TargetPawn) + MeshOffset);
	static auto GetAnimationInstance = UObject::GetObjectFromName(XORSTRING("Function Engine.SkeletalMeshComponent.GetAnimInstance"));

	UObject* ReturnValue;

	Globals::ProcessEvent(Mesh, GetAnimationInstance, &ReturnValue);

	return ReturnValue;
}

static void Montage_Play(UObject* Montage, float PlayRate, EMontagePlayReturnType ReturnType, float StartMontageAt, bool StopMontages)
{
	static auto Montage_Play = UObject::GetObjectFromName(XORSTRING("Function Engine.AnimInstance.Montage_Play"));

	auto AnimationInstance = GetAnimationInstance(Globals::PlayerPawn);

	struct
	{
		UObject* Montage;
		float PlayRate;
		EMontagePlayReturnType ReturnType;
		float StartMontageAt;
		bool StopMontages;
		float ReturnValue;
	} Params;

	Params.Montage = Montage;
	Params.PlayRate = PlayRate;
	Params.ReturnType = ReturnType;
	Params.StartMontageAt = StartMontageAt;
	Params.StopMontages = StopMontages;

	if (Montage_Play) Globals::ProcessEvent(AnimationInstance, Montage_Play, &Params);
}

static void Emote(PVOID Params)
{
	if (Globals::EngineVersionString.find("4.16") == string::npos) {
		auto EmoteItemDefinition = *(UObject**)(Params);

		auto Animation = GetAnimationHardReference(EmoteItemDefinition);

		if (Animation)
		{
			Globals::CurrentEmote = Animation;
			Globals::EmotePosition = GetLocation(Globals::PlayerPawn);
			Globals::bIsEmoting = true;
			Montage_Play(Animation, 1, EMontagePlayReturnType::Duration, 0, true);
		}
	}
}

static void StopMontage(UObject* MontageToStop)
{
	static auto StopMontage = UObject::GetObjectFromName(XORSTRING("Function FortniteGame.FortPlayerPawn.ServerRootMotionInterruptNotifyStopMontage"));

	Globals::ProcessEvent(Globals::PlayerPawn, StopMontage, &MontageToStop);
}

static UObject* GetBuildingPreviewMesh(UObject* BuildPreview)
{
	static UObject* GetBuildingPreviewMesh = UObject::GetObjectFromName(XORSTRING("Function FortniteGame.BuildingSMActor.GetBuildingMeshComponent"));

	UObject* ReturnValue;

	Globals::ProcessEvent(BuildPreview, GetBuildingPreviewMesh, &ReturnValue);

	return ReturnValue;
}

static void SetMeshAndMaterial(UObject* BuildPreview, UObject* Mesh)
{
	UObject* MeshComponent = GetBuildingPreviewMesh(BuildPreview);

	static UObject* SetMesh = UObject::GetObjectFromName(XORSTRING("Function Engine.StaticMeshComponent.SetStaticMesh"));
	static UObject* SetMaterial = UObject::GetObjectFromName(XORSTRING("Function Engine.PrimitiveComponent.SetMaterial"));

	struct
	{
		UObject* Mesh;
		bool ReturnValue;
	} _Params;

	_Params.Mesh = Mesh;

	struct
	{
		int Index;
		UObject* Material;
	} Params;

	Params.Index = 0;
	Params.Material = *(UObject**)(__int64(Globals::PlayerController) + UObject::FindOffset(XORSTRING("ObjectProperty FortniteGame.FortPlayerController.BuildPreviewMarkerMID")));

	Globals::ProcessEvent(MeshComponent, SetMesh, &_Params);
	Globals::ProcessEvent(MeshComponent, SetMaterial, &Params);
}

static void OnBuildingActorInitialized(UObject* BuildPreview, EFortBuildingType BuildType)
{
	static UObject* OnBuildingActorInitialized = UObject::GetObjectFromName(XORSTRING("Function FortniteGame.BuildingActor.OnBuildingActorInitialized"));

	struct
	{
		EFortBuildingInitializationReason InitReason;
		EFortBuildingPersistentState State;
	} Params;

	Params.InitReason = EFortBuildingInitializationReason::PlacementTool;
	Params.State = EFortBuildingPersistentState::New;

	Globals::ProcessEvent(BuildPreview, OnBuildingActorInitialized, &Params);
}

static void SetupBuildingPreviews()
{
#ifndef SERVERCODE
	if (Globals::EngineVersionString.find("4.16") != string::npos ||
		Globals::EngineVersionString.find("4.19") != string::npos)
	{
		Globals::WallPreview = SpawnActorFromClass(XORSTRING("Class FortniteGame.BuildingPlayerPrimitivePreview"), FVector{ 0, 0, 100 });
		SetMeshAndMaterial(Globals::WallPreview, UObject::GetObjectFromName(XORSTRING("StaticMesh PBW_W1_Solid.PBW_W1_Solid")));
		OnBuildingActorInitialized(Globals::WallPreview, EFortBuildingType::Wall);
		SetActorHiddenInGame(Globals::WallPreview, true);

		Globals::FloorPreview = SpawnActorFromClass(XORSTRING("Class FortniteGame.BuildingPlayerPrimitivePreview"), FVector{ 0, 0, 100 });
		SetMeshAndMaterial(Globals::FloorPreview, UObject::GetObjectFromName(XORSTRING("StaticMesh PBW_W1_Floor.PBW_W1_Floor")));
		OnBuildingActorInitialized(Globals::FloorPreview, EFortBuildingType::Floor);
		SetActorHiddenInGame(Globals::FloorPreview, true);

		Globals::StairsPreview = SpawnActorFromClass(XORSTRING("Class FortniteGame.BuildingPlayerPrimitivePreview"), FVector{ 0, 0, 100 });
		SetMeshAndMaterial(Globals::StairsPreview, UObject::GetObjectFromName(XORSTRING("StaticMesh PBW_W1_StairW.PBW_W1_StairW")));
		OnBuildingActorInitialized(Globals::StairsPreview, EFortBuildingType::Stairs);
		SetActorHiddenInGame(Globals::StairsPreview, true);

		Globals::RoofPreview = SpawnActorFromClass(XORSTRING("Class FortniteGame.BuildingPlayerPrimitivePreview"), FVector{ 0, 0, 100 });
		SetMeshAndMaterial(Globals::RoofPreview, UObject::GetObjectFromName(XORSTRING("StaticMesh PBW_W1_RoofC.PBW_W1_RoofC")));
		OnBuildingActorInitialized(Globals::RoofPreview, EFortBuildingType::Roof);
		SetActorHiddenInGame(Globals::RoofPreview, true);
	}
#endif
}


static void SetupPositioning()
{
	auto PlayerStartArray = ArrayFindActorsFromClass(UObject::GetObjectFromName(XORSTRING("Class FortniteGame.FortPlayerStartWarmup")));
	int Index = rand() % PlayerStartArray.Num();

	TeleportTo(GetRotation(PlayerStartArray[Index]), GetLocation(PlayerStartArray[Index]));
}

static void OnRep_bIsOutsideSafezone()
{
	static auto ReplicateOutsideSafezone = UObject::GetObjectFromName(XORSTRING("Function FortniteGame.FortPlayerPawnAthena.OnRep_IsOutsideSafeZone"));
	static auto ReplicateOutsideSafezonePlayerPawn = UObject::GetObjectFromName(XORSTRING("Function FortniteGame.FortPlayerPawn.OnRep_IsOutsideSafeZone"));

	if (ReplicateOutsideSafezone) Globals::ProcessEvent(Globals::PlayerPawn, ReplicateOutsideSafezone, nullptr);
	else if (ReplicateOutsideSafezonePlayerPawn) Globals::ProcessEvent(Globals::PlayerPawn, ReplicateOutsideSafezonePlayerPawn, nullptr);
}

static void OnRep_EditTool(UObject* EditTool)
{
	static auto ReplicateEditTool = UObject::GetObjectFromName(XORSTRING("Function FortniteGame.FortWeap_EditingTool.OnRep_EditActor"));

	Globals::ProcessEvent(EditTool, ReplicateEditTool, nullptr);
}

static void BeginEditingBuildingActor(PVOID Params)
{
	auto WeaponEditActor = EquipWeapon(Globals::EditTool, Globals::EditToolGUID);

	*(UObject**)(__int64(WeaponEditActor) + OffsetTable::EditActorOffset) = *(UObject**)(Params);

	OnRep_EditTool(WeaponEditActor);
}

static void SetActorLocation(UObject* Target, FVector Location)
{
	static auto SetLocation = UObject::GetObjectFromName(XORSTRING("Function Engine.Actor.K2_SetActorLocation"));

	Globals::ProcessEvent(Target, SetLocation, &Location);
}

static void SetActorScale3d(UObject* Target)
{
	static auto fn = UObject::GetObjectFromName(XORSTRING("Function Engine.Actor.SetActorScale3D"));

	FVector Scale = FVector{ 0,0,0 };

	Globals::ProcessEvent(Target, fn, &Scale);
}

static void ServerFinishEditingBuildingActor(PVOID Params)
{
	struct ParameterStruct
	{
		UObject* Target;
		UObject* Class;
		char RotationIteration;
		bool bMirrored;
	};

	auto CurrentParams = *(ParameterStruct*)(Params);

	static int index = UObject::FindBitFieldOffset(XORSTRING("BoolProperty FortniteGame.BuildingSMActor.bPlayDestructionEffects"));

	((BitFieldStruct*)((bool*)(__int64(CurrentParams.Target) + OffsetTable::bPlayDestructionEffects)))->bitfield[index] = false;

	SetActorScale3d(CurrentParams.Target);
	Destroy(CurrentParams.Target);

	auto EditedActor = SpawnActorFromClassObj(CurrentParams.Class, Globals::ComponentLocation, Globals::ComponentRotation);

	InitializeBuildingActor(EditedActor);
}

bool bIsInEditMode()
{
	static UObject* FortHUDContext = Globals::HudContext;
	static UObject* IsInEditMode = UObject::GetObjectFromName(XORSTRING("Function FortniteUI.FortHUDContext.IsInEditMode"));

	bool ReturnValue;

	Globals::ProcessEvent(FortHUDContext, IsInEditMode, &ReturnValue);

	return ReturnValue;
}

UObject* GetViewTarget(UObject* playerController)
{
	static auto Fn = UObject::GetObjectFromName(XORSTRING("Function Engine.Controller.GetViewTarget"));

	UObject* ReturnValue;

	Globals::ProcessEvent(playerController, Fn, &ReturnValue);

	return ReturnValue;
}

static void SetConsoleKey(FName KeyName)
{
	static auto fn = UObject::GetObjectFromName(XORSTRING("Function Engine.PlayerController.ConsoleKey"));

	FKey params;

	params.keyname = KeyName;

	Globals::ProcessEvent(Globals::PlayerController, fn, &params);
}

static void SpawnPickupsAthena_Terrain()
{
#ifndef SERVERCODE

	auto WarmupArray = ArrayFindActorsFromClass(UObject::GetObjectFromName(XORSTRING("BlueprintGeneratedClass Tiered_Athena_FloorLoot_Warmup.Tiered_Athena_FloorLoot_Warmup_C")));
	auto AthenaArray = ArrayFindActorsFromClass(UObject::GetObjectFromName(XORSTRING("BlueprintGeneratedClass Tiered_Athena_FloorLoot_01.Tiered_Athena_FloorLoot_01_C")));

	if (WarmupArray.Data != nullptr) {
		for (auto i = 0; i < WarmupArray.Num(); i++)
		{
			auto WeaponType = (rand() % 4) + 1;
			switch (WeaponType)
			{
			case 1:
			{
				auto WeaponIndex = rand() % Globals::WeaponItemDefs.Num();
				auto CurrentPickup = SpawnPickupAtLocation(Globals::WeaponItemDefs[WeaponIndex], Globals::WeaponItemCount[WeaponIndex], GetLocation(WarmupArray[i]), false, 0, 0, false);
				auto AmmoData = *(SoftObjectPtr*)(__int64(Globals::WeaponItemDefs[WeaponIndex]) + OffsetTable::AmmoData);
				auto CurrentAmmoData = OffsetTable::AmmoData > 0 ? SoftObjectPtrToObject(AmmoData) : AssetPtrToObject(*(AssetPtr*)(__int64(Globals::WeaponItemDefs[WeaponIndex]) + OffsetTable::AmmoData416));
				for (auto j = 0; j < Globals::AmmoItemDefs.Num(); j++)
				{
					if (Globals::AmmoItemDefs[j] == CurrentAmmoData)
					{
						auto CurrentLoc = GetLocation(CurrentPickup);
						CurrentLoc.X += 40;
						SpawnPickupAtLocation(Globals::AmmoItemDefs[j], Globals::AmmoItemCount[j], CurrentLoc, false, 0, 0, false);
						break;
					}
				}
				break;
			}
			case 2:
			{
				auto ConsumableIndex = rand() % Globals::ConsumablesItemDefs.Num();
				SpawnPickupAtLocation(Globals::ConsumablesItemDefs[ConsumableIndex], Globals::ConsumablesItemCount[ConsumableIndex], GetLocation(WarmupArray[i]), false, 0, 0, false);
				break;
			}
			case 3:
			{
				if (Globals::TrapsItemDefs.Num() > 0) {
					auto TrapsIndex = rand() % Globals::TrapsItemDefs.Num();
					SpawnPickupAtLocation(Globals::TrapsItemDefs[TrapsIndex], Globals::TrapsItemCount[TrapsIndex], GetLocation(WarmupArray[i]), false, 0, 0, false);
				}
				break;
			}
			case 4:
			{
				auto ResourceIndex = rand() % Globals::ResourceItemDefs.Num();
				SpawnPickupAtLocation(Globals::ResourceItemDefs[ResourceIndex], Globals::ResourceItemCount[ResourceIndex], GetLocation(WarmupArray[i]), false, 0, 0, false);
				break;
			}
			}
		}
	}
	for (auto i = 0; i < AthenaArray.Num(); i++)
	{
		auto WeaponType = (rand() % 4) + 1;
		switch (WeaponType)
		{
		case 1:
		{
			auto WeaponIndex = rand() % Globals::WeaponItemDefs.Num();
			auto CurrentPickup = SpawnPickupAtLocation(Globals::WeaponItemDefs[WeaponIndex], Globals::WeaponItemCount[WeaponIndex], GetLocation(AthenaArray[i]), false, 0, 0, false);
			auto AmmoData = *(SoftObjectPtr*)(__int64(Globals::WeaponItemDefs[WeaponIndex]) + OffsetTable::AmmoData);
			auto CurrentAmmoData = OffsetTable::AmmoData > 0 ? SoftObjectPtrToObject(AmmoData) : AssetPtrToObject(*(AssetPtr*)(__int64(Globals::WeaponItemDefs[WeaponIndex]) + OffsetTable::AmmoData416));
			for (auto j = 0; j < Globals::AmmoItemDefs.Num(); j++)
			{
				if (Globals::AmmoItemDefs[j] == CurrentAmmoData)
				{
					auto CurrentLoc = GetLocation(CurrentPickup);
					CurrentLoc.X += 40;
					SpawnPickupAtLocation(Globals::AmmoItemDefs[j], Globals::AmmoItemCount[j], CurrentLoc, false, 0, 0, false);
					break;
				}
			}
			break;
		}
		case 2:
		{
			auto ConsumableIndex = rand() % Globals::ConsumablesItemDefs.Num();
			SpawnPickupAtLocation(Globals::ConsumablesItemDefs[ConsumableIndex], Globals::ConsumablesItemCount[ConsumableIndex], GetLocation(AthenaArray[i]), false, 0, 0, false);
			break;
		}
		case 3:
		{
			if (Globals::TrapsItemDefs.Num() > 0) {
				auto TrapsIndex = rand() % Globals::TrapsItemDefs.Num();
				SpawnPickupAtLocation(Globals::TrapsItemDefs[TrapsIndex], Globals::TrapsItemCount[TrapsIndex], GetLocation(AthenaArray[i]), false, 0, 0, false);
			}
			break;
		}
		case 4:
		{
			auto ResourceIndex = rand() % Globals::ResourceItemDefs.Num();
			SpawnPickupAtLocation(Globals::ResourceItemDefs[ResourceIndex], Globals::ResourceItemCount[ResourceIndex], GetLocation(AthenaArray[i]), false, 0, 0, false);
			break;
		}
		}
	}
#endif
}

static void InitializeClasses()
{
	classes::PlayerPawnClass = UObject::GetObjectFromName(XORSTRING("BlueprintGeneratedClass PlayerPawn_Athena.PlayerPawn_Athena_C"));
}

static void InitializeFunctions()
{

}

static void ToggleGodMode()
{
	static auto fn = UObject::GetObjectFromName(XORSTRING("Function Engine.CheatManager.God"));

	if (fn) Globals::ProcessEvent(Globals::CheatManager, fn, nullptr);
}

static void InitializeConsole()
{
	auto NewConsole = SpawnObject(UObject::GetObjectFromName(XORSTRING("Class Engine.Console")), Globals::Viewport);
	*reinterpret_cast<UObject**>(__int64(Globals::Viewport) + OffsetTable::ViewportConsole) = NewConsole;

	Globals::CheatManager = SpawnObject(UObject::GetObjectFromName(XORSTRING("Class Engine.CheatManager")), Globals::PlayerController);
	*reinterpret_cast<UObject**>(__int64(Globals::PlayerController) + UObject::FindOffset(XORSTRING("ObjectProperty Engine.PlayerController.CheatManager"))) = Globals::CheatManager;
}

#endif // !GAMEFUNCTIONS_H
