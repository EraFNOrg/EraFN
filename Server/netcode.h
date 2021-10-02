#pragma once

#include "../OffsetInitialize.h"
#include "../netcode_structs.h"
#include "../GameFunctions.h"
#if defined(SERVERCODE)

namespace Server
{
	int32 ServerReplicateActors_PrepConnections(UObject* NetDriver, float DeltaSeconds)
	{
		TArray<UObject*> ClientConnections = *(TArray<UObject*>*)(__int64(NetDriver) + OffsetTable::ClientConnectionsOffset);

		int32_t NumClientsToTick = ClientConnections.Num();

		bool bFoundReadyConnection = false;

		for (int ConnIdx = 0; ConnIdx < ClientConnections.Num(); ConnIdx++)
		{
			auto Connection = ClientConnections[ConnIdx];

			//CHECK
			if (!Connection)
			{
				printf(XORSTRING("the connection with index %d is nullptr.\n"), ConnIdx);
				continue;
			}

			//OwningActor
			auto OwningActor = *(UObject**)(__int64(Connection) + OffsetTable::OwningActor);

			auto Driver = *(UObject**)(__int64(Connection) + OffsetTable::ConnectionDriver);
			auto Time = *(float*)(__int64(Driver) + OffsetTable::NetDriverTime);
			auto LastReceiveTime = *(double*)(__int64(Connection) + OffsetTable::LastReceiveTime);

			if (OwningActor != nullptr && (Time - LastReceiveTime < 1.5f))
			{
				printf(XORSTRING("valid owningactor and time.\n"));

				bFoundReadyConnection = true;

				auto DesiredViewTarget = OwningActor;

				auto ConnectionPlayerController = *(UObject**)(__int64(Connection) + OffsetTable::PlayerController);

				if (ConnectionPlayerController)
				{
					if (UObject* ViewTarget = GetViewTarget(ConnectionPlayerController))
					{
						DesiredViewTarget = ViewTarget;
					}
				}

				*(UObject**)(__int64(Connection) + OffsetTable::ConnectionViewTarget) = DesiredViewTarget;

				auto ConnectionChildren = *(TArray<UObject*>*)(__int64(Connection) + OffsetTable::ChildrenConnection);

				for (int ChildIdx = 0; ChildIdx < ConnectionChildren.Num(); ChildIdx++)
				{
					auto Child = ConnectionChildren[ChildIdx];

					auto ChildConnectionPlayerController = *(UObject**)(__int64(Child) + OffsetTable::PlayerController);

					if (ChildConnectionPlayerController != nullptr)
					{
						*(UObject**)(__int64(Child) + OffsetTable::ConnectionViewTarget) = GetViewTarget(ChildConnectionPlayerController);
					}
					else
					{
						*(UObject**)(__int64(Child) + OffsetTable::ConnectionViewTarget) = NULL;
					}
				}
			}
			else
			{
				*(UObject**)(__int64(Connection) + OffsetTable::ConnectionViewTarget) = NULL;

				auto ConnectionChildren = *(TArray<UObject*>*)(__int64(Connection) + OffsetTable::ChildrenConnection);

				for (int ChildIdx = 0; ChildIdx < ConnectionChildren.Num(); ChildIdx++)
				{
					auto Child = ConnectionChildren[ChildIdx];

					*(UObject**)(__int64(Child) + OffsetTable::ConnectionViewTarget) = NULL;
				}

			}
		}

		return bFoundReadyConnection ? NumClientsToTick : 0;
	}

	void ServerReplicateActors_BuildConsiderList(UObject* NetDriver, TArray<FNetworkObjectInfo*> OutConsiderList, float ServerTickTime)
	{
		int32_t NumInitiallyDormant = 0;

		const bool bUseAdaptiveNetFrequency = false;////////// should be set by the return value of a function

		TArray<AActor*> ActorsToRemove;

		for (const TSharedPtr<FNetworkObjectInfo>& ObjectInfo : TSet<int>()) // ?????????
		{
			FNetworkObjectInfo* ActorInfo = ObjectInfo.Object;

			double* WorldTimeSeconds = (double*)((__int64)Globals::World + OffsetTable::WorldTimeSeconds);

			if (!ActorInfo->bPendingNetUpdate && *WorldTimeSeconds <= ActorInfo->NextUpdateTime)
			{
				continue;
			}

			AActor* Actor = ActorInfo->Actor;

			float* ActorNetUpdateFrequency = (float*)((__int64)Actor + OffsetTable::NetUpdateFrequency);
			float* ActorMinNetUpdateFrequency = (float*)((__int64)Actor + OffsetTable::MinNetUpdateFrequency);

			if (GObjectsPointer->ObjObjects.GetItemByIndex(((UObject*)Actor)->Name.ComparisonIndex)->IsPendingKill())	//Actor->IsBeingDestroyed maybe, but its a very annoying bitfiled so fuck off
			{
				printf(XORSTRING("Actor added to 'ActorsToRemove' list cause object IsPendingKillPending\n"));
				ActorsToRemove.Add(Actor);
			}

			if ((*(ENetRole*)((__int64)Actor + OffsetTable::ActorRemoteRole)) == ENetRole::ROLE_None)
			{
				printf(XORSTRING("Actor added to 'ActorsToRemove' list cause it didn't have a NetRole\n"));
				ActorsToRemove.Add(Actor);
			}

			if (((FName*)((__int64)Actor + OffsetTable::ActorNetDriverName))->ComparisonIndex !=
				((FName*)(NetDriver + OffsetTable::NetDriverName))->ComparisonIndex)
			{
				printf(XORSTRING("Actor added to 'ActorsToRemove' list cause it belongs to a different NetDriver\n"));
				ActorsToRemove.Add(Actor);
			}
			
			UObject* Level = ((UObject*)Actor)->Outer; // Actor->Outer
			if (!Level || !((UObject*)((__int64)Level + OffsetTable::LevelOwningWorld)))
			{
				continue;
			}

			if (Actor && Globals::ActorIsNetStartupActor(Actor) && *(ENetDormancy*)((__int64)Actor + OffsetTable::ActorNetDormancy) == ENetDormancy::DORM_Initial) //if ( IsDormInitialStartupActor(Actor) ) E8 ?? ?? ?? ?? 48 8B 55 07 84 C0 AActor->IsNetStartupActor()
			{
				NumInitiallyDormant++;
				ActorsToRemove.Add(Actor);
				printf(XORSTRING("skipping Actor %s - its initially dormant!"), ((UObject*)(__int64)Actor)->GetFullName());
				continue;
			}


			if (ActorInfo->LastNetReplicateTime == 0)
			{
				ActorInfo->LastNetReplicateTime = *WorldTimeSeconds;
				ActorInfo->OptimalNetUpdateDelta = 1.0f / *ActorNetUpdateFrequency;
			}

			const float ScaleDownStartTime = 2.0f;
			const float ScaleDownTimeRange = 5.0f;

			const float LastReplicateDelta = *WorldTimeSeconds - ActorInfo->LastNetReplicateTime;

			if (LastReplicateDelta > ScaleDownStartTime)
			{
				if (*ActorMinNetUpdateFrequency == 0.0f)
				{
					*ActorMinNetUpdateFrequency = 2.0f;
				}

				const float MinOptimalDelta = 1.0f / *ActorNetUpdateFrequency;
				const float MaxOptimalDelta = *ActorMinNetUpdateFrequency >= MinOptimalDelta ? *ActorMinNetUpdateFrequency : MinOptimalDelta;

				auto clamp = [](float X, float Min, float Max)
				{
					return X >= Min ? Min : X >= Max ? X : Max;
				};

				const float Alpha = clamp((LastReplicateDelta - ScaleDownStartTime) / ScaleDownTimeRange, 0.0f, 1.0f);

				ActorInfo->OptimalNetUpdateDelta = std::lerp(MinOptimalDelta, MaxOptimalDelta, Alpha);
			}

			if (!ActorInfo->bPendingNetUpdate)
			{
				const float NextUpdateDelta = bUseAdaptiveNetFrequency ? ActorInfo->OptimalNetUpdateDelta : 1.0f / *ActorNetUpdateFrequency;

				ActorInfo->NextUpdateTime = *WorldTimeSeconds + rand() * ServerTickTime + NextUpdateDelta;

				//UNetDriver::ElapsedTime is right after UNetDriver::Time, I still need to confirm it tho
				float* Time = (float*)((__int64)NetDriver + OffsetTable::NetDriverTime);

				ActorInfo->LastNetUpdateTime = *Time;
			}

			ActorInfo->bPendingNetUpdate = false;

			OutConsiderList.Add(ActorInfo);

			Globals::ActorCallPreReplication(Actor, NetDriver);
		}

		for (int i = 0; i < ActorsToRemove.Num(); i++)
		{
			Globals::RemoveNetworkActor(NetDriver, ActorsToRemove[i]);
		}
	}

	int32 ServerReplicateActors_PrioritizeActors(UNetConnection* Connection, const TArray<FNetViewer>& ConnectionViewers, const TArray<FNetworkObjectInfo*> ConsiderList, const bool bCPUSaturated, FActorPriority*& OutPriorityList, FActorPriority**& OutPriorityActors)
	{
		int32 FinalSortedCount = 0;
		int32 DeletedCount = 0;

		/*NetTag++;
		Connection->TickCount++;

		// Set up to skip all sent temporary actors
		for (int32 j = 0; j < Connection->SentTemporaries.Num(); j++)
		{
			Connection->SentTemporaries[j]->NetTag = NetTag;
		}
		
		// Make weak ptr once for IsActorDormant call
		//TWeakObjectPtr<UNetConnection> WeakConnection(Connection);

		const int32 MaxSortedActors = ConsiderList.Num() + DestroyedStartupOrDormantActors.Num();
		if (MaxSortedActors > 0)
		{
			OutPriorityList = new FActorPriority[MaxSortedActors];
			OutPriorityActors = new FActorPriority*[MaxSortedActors];

			for (FNetworkObjectInfo* ActorInfo : ConsiderList)
			{
				AActor* Actor = ActorInfo->Actor;

				UObject* Channel = Connection->FindActorChannelRef(ActorInfo->WeakActor);

				if (!Channel)
				{
					if (!IsLevelInitializedForActor(Actor, Connection))
					{
						continue;
					}

					if (!IsActorRelevantToConnection(Actor, ConnectionViewers))
					{
						continue;
					}
				}

				UNetConnection* PriorityConnection = Connection;

				if (Actor->bOnlyRelevantToOwner)
				{
					bool bHasNullViewTarget = false;

					PriorityConnection = IsActorOwnedByAndRelevantToConnection(Actor, ConnectionViewers, bHasNullViewTarget);

					if (PriorityConnection == nullptr)
					{
						if (!bHasNullViewTarget && Channel != NULL && Time - Channel->RelevantTime >= RelevantTimeout)
						{
							Channel->Close();
						}

						continue;
					}
				}

				if (Actor->NetTag != NetTag)
				{
					Actor->NetTag = NetTag;

					OutPriorityList[FinalSortedCount] = FActorPriority(PriorityConnection, Channel, ActorInfo, ConnectionViewers, bLowNetBandwidth);
					OutPriorityActors[FinalSortedCount] = OutPriorityList + FinalSortedCount;

					FinalSortedCount++;
				}
			}

			// Add in deleted actors
			for (auto It = Connection->GetDestroyedStartupOrDormantActorGUIDs().CreateConstIterator(); It; ++It)
			{
				FActorDestructionInfo& DInfo = *DestroyedStartupOrDormantActors.FindChecked(*It);
				OutPriorityList[FinalSortedCount] = FActorPriority(Connection, &DInfo, ConnectionViewers);
				OutPriorityActors[FinalSortedCount] = OutPriorityList + FinalSortedCount;
				FinalSortedCount++;
				DeletedCount++;
			}*
			// TSets here too im gonna kms

			//Sort(OutPriorityActors, FinalSortedCount, FCompareFActorPriority());
		}*/

		return FinalSortedCount;
	}

	int32 ServerReplicateActors(UObject* NetDriver, float DeltaSeconds)
	{
		TArray<UObject*> ClientConnections = *(TArray<UObject*>*)(__int64(NetDriver) + OffsetTable::ClientConnectionsOffset);
		auto World = *(UObject**)(__int64(NetDriver) + OffsetTable::NetDriverWorld);
		auto ReplicationDriver = *(UObject**)(__int64(NetDriver) + OffsetTable::ReplicationDriverOffset);

		if (ClientConnections.Num() == 0)
		{
			return 0;
		}

		if (ReplicationDriver)
		{
			printf(XORSTRING("ReplicationDriver ServerReplicateActors should be called instead!\n"));
			return 0;
		}

		if (!World)
		{
			printf(XORSTRING("Error: WORLD is nullptr!\n"));
			return 0;
		}

		int NumClientsToTick = ServerReplicateActors_PrepConnections(NetDriver, DeltaSeconds);

		//DO THAT FOR DEBUG
		//printf(XORSTRING("NumClientsToTick: %d\n"), NumClientsToTick);

		if (NumClientsToTick == 0)
		{
			return 0;
		}
	}
}

#endif