/*EraFN Copyright (C) 2021 Daniele "danii" Giompaolo

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.*/

#ifndef OFFSETINIT_H
#define OFFSETINIT_H

#include "Internals.h"
#include "OffsetTable.h"

namespace EraInit
{

	void Console()
	{
		AllocConsole();

		ShowWindow(GetConsoleWindow(), SW_SHOW);
		FILE* fp;
		freopen_s(&fp, "CONOIN$", "r", stdin);
		freopen_s(&fp, "CONOUT$", "w", stdout);
		freopen_s(&fp, "CONOUT$", "w", stderr);

		printf(XORSTRING("EraFN Copyright (C) 2021 Daniele 'danii' Giompaolo\n\nThis program is free software: you can redistribute it and/or modify\n"));
		printf(XORSTRING("it under the terms of the GNU General Public License as published by\nthe Free Software Foundation, either version 3 of the License, or\n"));
		printf(XORSTRING("(at your option) any later version.\n\nThis program is distributed in the hope that it will be useful,\nbut WITHOUT ANY WARRANTY; without even the implied warranty of\n"));
		printf(XORSTRING("MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\nGNU General Public License for more details.\n\n\n"));
	
		printf(XORSTRING("Era 1.5.1 || Made by danii#4000\nBackend by Kyiro#7884\nLauncher by ozne#3303 and Not a Robot#6932\nSpecial Thanks to Sizzy, Kemo, Mix, Fischsalat!\n\nEnjoy!\n\n\n"));

#if defined (SERVERCODE)
		printf(XORSTRING("IMPORTANT: THIS ERA VERSION WAS BUILT WITH SERVER CODE!\n\n"));
#endif
	}

	void SetOffsets()
	{
		Globals::CurrentEngineVersion = EEngineVersion::UE_NONE;

		auto GObjectsAddress = FindPattern(XORSTRING("48 8D 0D ? ? ? ? E8 ? ? ? ? E8 ? ? ? ? E8 ? ? ? ? 48 8B D6"));
		auto ToStringAddress = FindPattern(XORSTRING("48 89 5C 24 ? 57 48 83 EC 40 83 79 04 00 48 8B DA 48 8B F9"));
		auto SpawnActorFromClassAddress = FindPattern(XORSTRING("40 53 56 57 48 83 EC 70 48 8B 05 ? ? ? ? 48 33 C4 48 89 44 24 ? 0F 28 1D ? ? ? ? 0F 57 D2 48 8B B4 24 ? ? ? ? 0F 28 CB"));
		auto FreeAddress = FindPattern(XORSTRING("48 85 C9 74 2E 53 48 83 EC 20 48 8B D9 48 8B 0D ?"));

		if (GObjectsAddress && ToStringAddress &&
			SpawnActorFromClassAddress &&
			FreeAddress)
		{
			Globals::CurrentEngineVersion = EEngineVersion::UE_4_16;
#if defined(TESTINGVERSION)
			printf(XORSTRING("All patterns were valid, for Engine Version 4.16 or higher.\n"));

			std::cout << XORSTRING("GObjects: ") << GObjectsAddress << std::endl;
			std::cout << XORSTRING("ToString: ") << ToStringAddress << std::endl;
			std::cout << XORSTRING("SpawnActorFromClass: ") << SpawnActorFromClassAddress << std::endl;
			std::cout << XORSTRING("Free: ") << FreeAddress << std::endl;
#endif
		}
		else {
#if defined(TESTINGVERSION)
			printf(XORSTRING("Some patterns weren't found. Further information: \n"));
			std::cout << XORSTRING("GObjects: ") << GObjectsAddress << std::endl;
			std::cout << XORSTRING("ToString: ") << ToStringAddress << std::endl;
			std::cout << XORSTRING("SpawnActorFromClass: ") << SpawnActorFromClassAddress << std::endl;
			std::cout << XORSTRING("Free: ") << FreeAddress << std::endl;
#endif
		}

		if (Globals::CurrentEngineVersion == EEngineVersion::UE_NONE)
		{
			GObjectsAddress = FindPattern(XORSTRING("48 8B 05 ? ? ? ? 48 8D 1C C8 81 4B ? ? ? ? ? 49 63 76 30"));
			ToStringAddress = FindPattern(XORSTRING("40 53 48 83 EC 40 83 79 04 00 48 8B DA 75 19 E8 ? ? ? ?"));
			SpawnActorFromClassAddress = FindPattern(XORSTRING("40 53 56 57 48 83 EC 70 48 8B 05 ? ? ? ? 48 33 C4 48 89 44 24 ? 0F 28 1D ? ? ? ? 0F 57 D2 48 8B B4 24 ? ? ? ? 0F 28 CB"));
			FreeAddress = FindPattern(XORSTRING("48 85 C9 74 1D 4C 8B 05 ? ? ? ? 4D 85 C0"));

			if (GObjectsAddress && ToStringAddress &&
				SpawnActorFromClassAddress &&
				FreeAddress)
			{
				Globals::CurrentEngineVersion = EEngineVersion::UE_4_19;
#if defined(TESTINGVERSION)
				printf(XORSTRING("All patterns were valid, for Engine Version 4.19 or lower.\n"));

				std::cout << XORSTRING("GObjects: ") << GObjectsAddress << std::endl;
				std::cout << XORSTRING("ToString: ") << ToStringAddress << std::endl;
				std::cout << XORSTRING("SpawnActorFromClass: ") << SpawnActorFromClassAddress << std::endl;
				std::cout << XORSTRING("Free: ") << FreeAddress << std::endl;
#endif
			}
			else {
#if defined(TESTINGVERSION)
				printf(XORSTRING("Some patterns weren't found. Further information: \n"));
				std::cout << XORSTRING("GObjects: ") << GObjectsAddress << std::endl;
				std::cout << XORSTRING("ToString: ") << ToStringAddress << std::endl;
				std::cout << XORSTRING("SpawnActorFromClass: ") << SpawnActorFromClassAddress << std::endl;
				std::cout << XORSTRING("Free: ") << FreeAddress << std::endl;
#endif
		}
		}

		if (Globals::CurrentEngineVersion == EEngineVersion::UE_NONE)
		{
			GObjectsAddress = FindPattern(XORSTRING("48 8B 05 ? ? ? ? 48 8D 1C C8 81 4B ? ? ? ? ? 49 63 76 30"));
			if (!GObjectsAddress) GObjectsAddress = FindPattern(XORSTRING("48 8B 05 ? ? ? ? 48 8D 14 C8 EB 03"));
			ToStringAddress = FindPattern(XORSTRING("48 89 5C 24 ? 57 48 83 EC 40 83 79 04 00 48 8B DA 48 8B F9"));
			SpawnActorFromClassAddress = FindPattern(XORSTRING("40 53 56 57 48 83 EC 70 48 8B 05 ? ? ? ? 48 33 C4 48 89 44 24 ? 0F 28 1D ? ? ? ? 0F 57 D2 48 8B B4 24 ? ? ? ? 0F 28 CB"));
			FreeAddress = FindPattern(XORSTRING("48 85 C9 74 1D 4C 8B 05 ? ? ? ? 4D 85 C0"));

			if (GObjectsAddress && ToStringAddress &&
				SpawnActorFromClassAddress &&
				FreeAddress)
			{
				Globals::CurrentEngineVersion = EEngineVersion::UE_4_20;
#if defined(TESTINGVERSION)
				printf(XORSTRING("All patterns were valid, for Engine Version 4.20 or lower.\n"));

				std::cout << XORSTRING("GObjects: ") << GObjectsAddress << std::endl;
				std::cout << XORSTRING("ToString: ") << ToStringAddress << std::endl;
				std::cout << XORSTRING("SpawnActorFromClass: ") << SpawnActorFromClassAddress << std::endl;
				std::cout << XORSTRING("Free: ") << FreeAddress << std::endl;
#endif
			}
			else {
#if defined(TESTINGVERSION)
				printf(XORSTRING("Some patterns weren't found. Further information: \n"));
				std::cout << "GObjects: " << GObjectsAddress << std::endl;
				std::cout << "ToString: " << ToStringAddress << std::endl;
				std::cout << "SpawnActorFromClass: " << SpawnActorFromClassAddress << std::endl;
				std::cout << "Free: " << FreeAddress << std::endl;
#endif
			}
		}
		
		if (Globals::CurrentEngineVersion == EEngineVersion::UE_NONE)
		{
			GObjectsAddress = FindPattern(XORSTRING("48 8B 05 ? ? ? ? 48 8B 0C C8 48 8D 04 D1 EB 03 48 8B ? 81 48 08 ? ? ? 40 49"));
			if (!GObjectsAddress) GObjectsAddress = FindPattern(XORSTRING("48 8B 05 ? ? ? ? 48 8B 0C C8 48 8D 04 D1 EB 03 49 8B C5 81 48 ? ? ? ? ? ? 39 46 48"));
			ToStringAddress = FindPattern(XORSTRING("48 89 5C 24 ? 57 48 83 EC 30 83 79 04 00 48 8B DA 48 8B F9"));
			SpawnActorFromClassAddress = FindPattern(XORSTRING("40 53 56 57 48 83 EC 70 48 8B 05 ? ? ? ? 48 33 C4 48 89 44 24 ? 0F 28 1D ? ? ? ? 0F 57 D2 48 8B B4 24 ? ? ? ? 0F 28 CB"));
			FreeAddress = FindPattern(XORSTRING("48 85 C9 74 2E 53 48 83 EC 20 48 8B D9"));

			if (GObjectsAddress && ToStringAddress &&
				SpawnActorFromClassAddress &&
				FreeAddress)
			{
				Globals::CurrentEngineVersion = EEngineVersion::UE_4_21;
#if defined(TESTINGVERSION)
				printf(XORSTRING("All patterns were valid, for Engine Version 4.21 or higher.\n"));

				std::cout << XORSTRING("GObjects: ") << GObjectsAddress << std::endl;
				std::cout << XORSTRING("ToString: ") << ToStringAddress << std::endl;
				std::cout << XORSTRING("SpawnActorFromClass: ") << SpawnActorFromClassAddress << std::endl;
				std::cout << XORSTRING("Free: ") << FreeAddress << std::endl;
#endif
			}
			else {
#if defined(TESTINGVERSION)
				printf(XORSTRING("Some patterns weren't found. Further information: \n"));
				std::cout << "GObjects: " << GObjectsAddress << std::endl;
				std::cout << "ToString: " << ToStringAddress << std::endl;
				std::cout << "SpawnActorFromClass: " << SpawnActorFromClassAddress << std::endl;
				std::cout << "Free: " << FreeAddress << std::endl;
#endif
			}
		}

		if (Globals::CurrentEngineVersion != EEngineVersion::UE_NONE)
		{
			auto ObjectsOffset = *(int32_t*)(GObjectsAddress + 3);
			auto FinalObjectsAddress = GObjectsAddress + 7 + ObjectsOffset;
			if (Globals::CurrentEngineVersion >= EEngineVersion::UE_4_21)
			{
				NewGObjectsPointer = decltype(NewGObjectsPointer)(FinalObjectsAddress);
			}
			else
			{
				GObjectsPointer = decltype(GObjectsPointer)(FinalObjectsAddress);

				if (GObjectsPointer->ObjObjects.NumElements <= 0) GObjectsPointer = decltype(GObjectsPointer)(FinalObjectsAddress + 0x10);

			}
			SpawnActor = decltype(SpawnActor)(SpawnActorFromClassAddress);
			FNameToString = decltype(FNameToString)(ToStringAddress);
			FreeInternal = decltype(FreeInternal)(FreeAddress);
			auto ProcessEventAddress = FindPattern(XORSTRING("40 55 56 57 41 54 41 55 41 56 41 57 48 81 EC ? ? ? ? 48 8D 6C 24 ? 48 89 9D ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C5 48 89 85 ? ? ? ? 48 63 41 0C"));
			if (ProcessEventAddress > 0)
			{
				Globals::ProcessEvent = decltype(Globals::ProcessEvent)(ProcessEventAddress);
			}
			else
			{
				void* Ptr;
				if (EngineVersion::GetVersion(&Ptr).find("7.4") != string::npos) Globals::ProcessEvent = decltype(Globals::ProcessEvent)(UObject::GetObjectFromName(XORSTRING("FortEngine_"))->Vtable[0x41]);
				else Globals::ProcessEvent = decltype(Globals::ProcessEvent)(UObject::GetObjectFromName(XORSTRING("FortEngine_"))->Vtable[0x40]);
			}

			void* Ptr;
			Globals::EngineVersionString = EngineVersion::EngineVer(&Ptr);

			auto FindRowUncheckedAddress = FindPattern(XORSTRING("48 83 EC 08 48 83 79 ? ? 4C 8B D9 75 07 33 C0 48 83 C4 08 C3"));
			if (!FindRowUncheckedAddress) {
				FindRowUncheckedAddress = FindPattern(XORSTRING("40 53 48 83 EC 20 48 83 79 ? ? 48 8B DA 75 08"));
			}
			Globals::FindRowUnchecked = decltype(Globals::FindRowUnchecked)(FindRowUncheckedAddress);
			Globals::StaticLoadObject = decltype(Globals::StaticLoadObject)(FindPattern(XORSTRING("40 55 53 56 57 41 54 41 55 41 56 41 57 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 8B 85 ? ? ? ? 33 F6 4C 8B BD ? ? ? ? 49 8B F9")));
		}
		else
		{
			printf(XORSTRING("Unsupported Engine Version!\n"));
		}

#if defined(SERVERCODE)

		//find addresses
		auto InitHostAddress = FindPattern(XORSTRING("48 8B C4 48 81 EC ? ? ? ? 48 89 58 18 4C 8D 05 ? ? ? ? 48 8B D9 48 89 78 F8 48 8D 48 88 45 33 C9 33 D2 E8 ? ? ? ? E8 ? ? ? ?"));
		auto PauseBeaconRequestAddress = FindPattern(XORSTRING("40 53 48 83 EC 30 48 8B D9 84 D2 74 68 80 3D ? ? ? ? ? 72 2C 48 8B 05 ? ? ? ? 4C 8D 44 24 ? 48 89 44 24 ?"));
		auto WelcomePlayerAddress = FindPattern(XORSTRING("48 8B C4 55 48 8D A8 ? ? ? ? 48 81 EC ? ? ? ? 48 89 70 20 48 8B F1 48 89 78 F0 48 8B CA 4C 89 70 E8 48 8B FA E8 ? ? ? ? 48 8B 8E ? ? ? ? E8 ? ? ? ? 48 8D 54 24 ? 48 8D 8D ? ? ? ? 4C 8B 40 18 4C 89 85 ? ? ? ? E8 ? ? ? ? 48 8B 8E ? ? ? ? E8 ? ? ? ? 48 8B CF 48 8B 50 18 E8 ? ? ? ? 48 8B 8E ? ? ? ? 45 33 F6 4C 89 74 24 ? 4C 89 74 24 ? 4C 89 74 24 ? 4C 89 74 24 ? 48 85 C9 0F 84 ? ? ? ?"));
		auto BeaconNotifyControlMessageAddress = FindPattern(XORSTRING("40 55 53 56 57 41 54 41 56 41 57 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 33 FF 48 89 4C 24 ? 89 7C 24 60 49 8B F1 48 8B 41 10 45 0F B6 E0 4C 8B F2 48 8B D9 44 8B FF 48 39 78 78 0F 85 ? ? ? ? 80 3D ? ? ? ? ?"));
		auto WorldNotifyControlMessageAddress = FindPattern(XORSTRING("40 55 53 56 57 41 54 41 55 41 56 41 57 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 45 33 F6 49 8B D9 44 89 74 24 ? 45 8B E6 48 8B 41 10 45 0F B6 F8 48 8B FA 4C 8B E9 4C 39 60 78"));
		auto SpawnPlayActorAddress = FindPattern(XORSTRING("44 89 44 24 ? 48 89 54 24 ? 48 89 4C 24 ? 55 53 56 57 41 54 41 55 41 56 41 57 48 8D 6C 24 ? 48 81 EC ? ? ? ? 33 F6"));
		auto SeamlessTravelHandlerForWorldAddress = FindPattern(XORSTRING("E8 ? ? ? ? 48 83 C0 ? 48 83 C4 ? C3 CC EA ? ? ? ? ? ? 6E"), true, 1);
		auto BeaconNotifyAcceptingConnectionAddress = FindPattern(XORSTRING("48 83 EC 48 48 8B 41 10 48 83 78 ? ? 74 3C 80 3D ? ? ? ? ? 72 2C 48 8B 05 ? ? ? ? 4C 8D 44 24 ? 48 89 44 24 ?"));
		auto WorldNotifyAcceptingConnectionAddress = FindPattern(XORSTRING("40 55 48 83 EC 50 48 8B 41 10 48 8B E9 48 83 78 ? ? 74 3D 80 3D ? ? ? ? ? 72 2C 48 8B 05 ? ? ? ? 4C 8D 44 24 ? 48 89 44 24 ?"));
		auto BeaconAcceptedConnectionAddress = FindPattern(XORSTRING("40 56 48 83 EC 60 80 3D ? ? ? ? ? 4C 8B C2 48 8B F1 0F 82 ? ? ? ? 48 8B 02 49 8B C8 48 89 5C 24 ? 48 8D 54 24 ? 48 89 7C 24 ? FF 90 ? ? ? ? 48 8D 1D ? ? ? ? 83 78 08 00 74 05 48 8B 38 EB 03 48 8B FB 48 8B 86 ? ? ? ? 48 8D 54 24 ? 48 8D 8C 24 ? ? ? ? 48 89 84 24 ? ? ? ? E8 ? ? ? ? 83 7C 24 ? ? 48 8B 05 ? ? ? ? 48 0F 45 5C 24 ? 48 89 84 24 ? ? ? ? E8 ? ? ? ? 48 89 7C 24 ? 4C 8D 84 24 ? ? ? ? 48 89 44 24 ? 41 B9 ? ? ? ? 48 8D 05 ? ? ? ? 48 89 5C 24 ? 33 D2 48 89 44 24 ? 33 C9 E8 ? ? ? ? 48 8B 4C 24 ? 48 8B 7C 24 ? 48 8B 5C 24 ? 48 85 C9 74 05 E8 ? ? ? ? 48 8B 4C 24 ? 48 85 C9 74 05 E8 ? ? ? ? 48 83 C4 60 5E C3"));
		auto WorldAcceptedConnectionAddress = FindPattern(XORSTRING("40 56 48 83 EC 60 80 3D ? ? ? ? ? 4C 8B C2 48 8B F1 0F 82 ? ? ? ? 48 8B 02 49 8B C8 48 89 5C 24 ? 48 8D 54 24 ? 48 89 7C 24 ? FF 90 ? ? ? ? 48 8D 1D ? ? ? ? 83 78 08 00 74 05 48 8B 38 EB 03 48 8B FB 48 8B 46 F0 48 8D 54 24 ? 48 8D 8C 24 ? ? ? ? 48 89 84 24 ? ? ? ? E8 ? ? ? ? 83 7C 24 ? ? 48 8B 05 ? ? ? ? 48 0F 45 5C 24 ? 48 89 84 24 ? ? ? ? E8 ? ? ? ? 48 89 7C 24 ? 4C 8D 84 24 ? ? ? ? 48 89 44 24 ? 41 B9 ? ? ? ? 48 8D 05 ? ? ? ? 48 89 5C 24 ? 33 D2 48 89 44 24 ? 33 C9 E8 ? ? ? ? 48 8B 4C 24 ? 48 8B 7C 24 ? 48 8B 5C 24 ? 48 85 C9 74 05 E8 ? ? ? ? 48 8B 4C 24 ? 48 85 C9 74 05 E8 ? ? ? ? 48 83 C4 60 5E C3"));
		auto BeaconNotifyAcceptingChannelAddress = FindPattern(XORSTRING("40 53 48 83 EC 50 48 8B 42 28 48 8B DA 4C 8B 40 58 49 83 78 ? ? 0F 84 ? ? ? ? 0F B6 05 ? ? ? ? 3C 05 0F 82 ? ? ? ? "));
		auto WorldNotifyAcceptingChannelAddress = FindPattern(XORSTRING("40 53 48 83 EC 50 48 8B 42 28 48 8B DA 4C 8B 40 58 49 83 78 ? ? 74 5E 8B 42 44 83 F8 02 0F 84 ? ? ? ? 83 F8 04 0F 84 ? ? ? ? 80 3D ? ? ? ? ? 0F 82 ? ? ? ? "));
		auto TickFlushAddress = FindPattern(XORSTRING("4C 8B DC 55 49 8D AB ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 49 89 5B 18 48 8D 05 ? ? ? ? 49 89 7B E8 48 8B F9 4D 89 63 E0 45 33 E4"));
		auto ActorCallPreReplicationAddress = FindPattern(XORSTRING("E8 ? ? ? ? 48 83 3F 00 0F B6 6B 14"));
		auto ActorIsNetStartupActorAddress = FindPattern(XORSTRING("E8 ?? ?? ?? ?? 48 8B 55 07 84 C0"));
		auto NetDriverRemoveNetworkActorAddress = FindPattern(XORSTRING("E8 ? ? ? ? 48 83 C3 10 48 3B DF 75 E7 48 8B 5C 24 ?"));

		//initialize functions
		if (InitHostAddress &&
			PauseBeaconRequestAddress &&
			WelcomePlayerAddress &&
			BeaconNotifyControlMessageAddress &&
			WorldNotifyControlMessageAddress &&
			SpawnPlayActorAddress &&
			SeamlessTravelHandlerForWorldAddress &&
			BeaconNotifyAcceptingConnectionAddress &&
			WorldNotifyAcceptingConnectionAddress &&
			BeaconAcceptedConnectionAddress &&
			WorldAcceptedConnectionAddress &&
			BeaconNotifyAcceptingChannelAddress &&
			WorldNotifyAcceptingChannelAddress &&
			TickFlushAddress &&
			ActorCallPreReplicationAddress &&
			ActorIsNetStartupActorAddress &&
			NetDriverRemoveNetworkActorAddress)
		{
			Globals::InitHost = decltype(Globals::InitHost)(InitHostAddress);
			Globals::PauseBeaconRequests = decltype(Globals::PauseBeaconRequests)(PauseBeaconRequestAddress);
			Globals::WelcomePlayer = decltype(Globals::WelcomePlayer)(WelcomePlayerAddress);
			Globals::BeaconNotifyControlMessage = decltype(Globals::BeaconNotifyControlMessage)(BeaconNotifyControlMessageAddress);
			Globals::WorldNotifyControlMessage = decltype(Globals::WorldNotifyControlMessage)(WorldNotifyControlMessageAddress);
			Globals::SpawnPlayActor = decltype(Globals::SpawnPlayActor)(SpawnPlayActorAddress);
			Globals::SeamlessTravelHandlerForWorld = decltype(Globals::SeamlessTravelHandlerForWorld)(SeamlessTravelHandlerForWorldAddress);
			Globals::BeaconNotifyAcceptingConnection = decltype(Globals::BeaconNotifyAcceptingConnection)(BeaconNotifyAcceptingConnectionAddress);
			Globals::WorldNotifyAcceptingConnection = decltype(Globals::WorldNotifyAcceptingConnection)(WorldNotifyAcceptingConnectionAddress);
			Globals::BeaconNotifyAcceptedConnection = decltype(Globals::BeaconNotifyAcceptedConnection)(BeaconAcceptedConnectionAddress);
			Globals::WorldNotifyAcceptedConnection = decltype(Globals::WorldNotifyAcceptedConnection)(WorldAcceptedConnectionAddress);
			Globals::BeaconNotifyAcceptingChannel = decltype(Globals::BeaconNotifyAcceptingChannel)(BeaconNotifyAcceptingChannelAddress);
			Globals::WorldNotifyAcceptingChannel = decltype(Globals::WorldNotifyAcceptingChannel)(WorldNotifyAcceptingChannelAddress);
			Globals::TickFlushNetDriver = decltype(Globals::TickFlushNetDriver)(TickFlushAddress);
			Globals::ActorCallPreReplication = decltype(Globals::ActorCallPreReplication)(ActorCallPreReplicationAddress);
			Globals::ActorIsNetStartupActor = decltype(Globals::ActorIsNetStartupActor)(ActorIsNetStartupActorAddress);
			Globals::RemoveNetworkActor = decltype(Globals::RemoveNetworkActor)(NetDriverRemoveNetworkActorAddress);
		}
		else printf(XORSTRING("ERROR: one or more patterns weren't found!\n"));

#endif
	}
}


#endif // !OFFSETINIT_H