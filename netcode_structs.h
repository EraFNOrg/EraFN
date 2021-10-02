#ifndef NETCODESTRUCTS_H
#define NETCODESTRUCTS_H

#include <Windows.h>
#include <string>
#include <locale>
#include <set>
#include <fstream>
#include "UnrealEngineStructs.h"

typedef __int8 int8;
typedef __int16 int16;
typedef __int32 int32;
typedef __int64 int64;
typedef unsigned  __int8 uint8;
typedef unsigned __int16 uint16;
typedef unsigned __int32 uint32;
typedef unsigned __int64 uint64;

#if defined(SERVERCODE)

template<class ElementType>
class TSet {};

struct UNetConnection {};

template<class ObjectType>
class TSharedPtr
{
public:
	ObjectType* Object;

	int32_t SharedReferenceCount;
	int32_t WeakReferenceCount;
};

enum class EChannelCloseReason : uint8
{
	Destroyed,
	Dormancy,
	LevelUnloaded,
	Relevancy,
	TearOff,
	MAX = 15		// this value is used for serialization, modifying it may require a network version change
};

struct FActorDestructionInfo
{
public:
	FActorDestructionInfo()
		: Reason(EChannelCloseReason::Destroyed)
		, bIgnoreDistanceCulling(false)
	{}

	TWeakObjectPtr<UObject> Level;
	TWeakObjectPtr<UObject> ObjOuter;
	FVector DestroyedPosition;
	FString NetGUID; // must be network guid but SMH
	FString PathName;
	FName StreamingLevelName;
	EChannelCloseReason Reason;

	/** When true the destruction info data will be sent even if the viewers are not close to the actor */
	bool bIgnoreDistanceCulling;

};

struct FNetViewer
{
	UNetConnection* Connection;
	UObject* InViewer;
	UObject* ViewTarget;
	FVector ViewLocation;
	FVector ViewDir;

	FNetViewer()
		: Connection(NULL)
		, InViewer(NULL)
		, ViewTarget(NULL)
		, ViewLocation(FVector())
		, ViewDir(FVector())
	{
	}

	FNetViewer(UNetConnection* InConnection, float DeltaSeconds) {
		UObject* ViewingController = reinterpret_cast<UObject*>(int64(InConnection) + OffsetTable::PlayerController);

		// Get viewer coordinates.
		ViewLocation = GetLocation(ViewTarget);
		if (ViewingController)
		{
			FRotator ViewRotation = GetRotation(ViewingController);
			//ViewingController->GetPlayerViewPoint(ViewLocation, ViewRotation);
			//ViewDir = ViewRotation.Vector();
			// that's all for now
		}
	}
};

struct FActorPriority
{
	int32 Priority;

	FNetworkObjectInfo* ActorInfo;
	class UActorChannel* Channel;

	FActorDestructionInfo* DestructionInfo;

	FActorPriority() :
		Priority(0), ActorInfo(NULL), Channel(NULL), DestructionInfo(NULL)
	{}

	FActorPriority(UNetConnection* InConnection, UObject* InChannel, FNetworkObjectInfo* InActorInfo, TArray<FNetViewer>& Viewers, bool bLowBandwidth) {
		auto Driver = reinterpret_cast<UObject*>(int64(InConnection) + OffsetTable::ConnectionDriver);
		auto SpawnSeconds = reinterpret_cast<float*>(int64(Driver) + OffsetTable::SpawnPrioritySeconds);
		auto DriverTime = reinterpret_cast<float*>(int64(Driver) + OffsetTable::NetDriverTime);

		//float Time = DriverTime - Channel->LastUpdateTime;
		float Time = *SpawnSeconds;
		Priority = 0;
		for (int32 i = 0; i < Viewers.Num(); i++)
		{
			//Priority = FMath::Max<int32>(Priority, FMath::RoundToInt(65536.0f * ActorInfo->Actor->GetNetPriority(Viewers[i].ViewLocation, Viewers[i].ViewDir, Viewers[i].InViewer, Viewers[i].ViewTarget, InChannel, Time, bLowBandwidth)));
		}
	}

	FActorPriority(UNetConnection* InConnection, FActorDestructionInfo* DestructInfo, TArray<FNetViewer>& Viewers) {
		Priority = 0;

		for (int32 i = 0; i < Viewers.Num(); i++)
		{
			auto Driver = reinterpret_cast<UObject*>(int64(InConnection) + OffsetTable::ConnectionDriver);
			auto SpawnSeconds = reinterpret_cast<float*>(int64(Driver) + OffsetTable::SpawnPrioritySeconds);
			float Time = *SpawnSeconds;

			//FVector Dir = DestructionInfo->DestroyedPosition - Viewers[i].ViewLocation;  
			// Implement - OPERATOR THANKS
			
			//float DistSq = Dir.SizeSquared();

			/*if ((Viewers[i].ViewDir | Dir) < 0.f)
			{
				if (DistSq > NEARSIGHTTHRESHOLDSQUARED)
					Time *= 0.2f;
				else if (DistSq > CLOSEPROXIMITYSQUARED)
					Time *= 0.4f;
			}
			else if (DistSq > MEDSIGHTTHRESHOLDSQUARED)
				Time *= 0.4f;

			Priority = FMath::Max<int32>(Priority, 65536.0f * Time);*/ // i will adjust this when i want to xd
		}
	}
};
#endif

#endif // !NETCODESTRUCTS_H