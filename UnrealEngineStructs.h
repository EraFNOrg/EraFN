/*EraFN Copyright (C) 2021 Daniele "danii" Giompaolo

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.*/

#ifndef UE4_H
#define UE4_H

#include <Windows.h>
#include <string>
#include <locale>
#include <set>
#include <fstream>
#include <bitset>

int GetObjectNameOffset = 0;
int EngineOffset = 0;

static __forceinline bool IsBadReadPtr(void* p)
{
	MEMORY_BASIC_INFORMATION mbi;
	if (VirtualQuery(p, &mbi, sizeof(mbi)))
	{
		DWORD mask = (PAGE_READONLY | PAGE_READWRITE | PAGE_WRITECOPY | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY);
		bool b = !(mbi.Protect & mask);
		if (mbi.Protect & (PAGE_GUARD | PAGE_NOACCESS)) b = true;

		return b;
	}
	return true;
}


using namespace std;

class UObject;

class FNameEntry
{
public:
	static const auto NAME_WIDE_MASK = 0x1;
	static const auto NAME_INDEX_SHIFT = 1;

	int32_t Index;
	char UnknownData00[0x04];
	FNameEntry* HashNext;
	union
	{
		char AnsiName[1024];
		wchar_t WideName[1024];
	};

};

template<typename ElementType, int32_t MaxTotalElements, int32_t ElementsPerChunk>
class TStaticIndirectArrayThreadSafeRead
{
public:
	inline size_t Num() const
	{
		return NumElements;
	}

	inline bool IsValidIndex(int32_t index) const
	{
		return index < Num() && index >= 0;
	}

	inline ElementType const* const& operator[](int32_t index) const
	{
		return *GetItemPtr(index);
	}

private:
	inline ElementType const* const* GetItemPtr(int32_t Index) const
	{
		int32_t ChunkIndex = Index / ElementsPerChunk;
		int32_t WithinChunkIndex = Index % ElementsPerChunk;
		ElementType** Chunk = Chunks[ChunkIndex];
		return Chunk + WithinChunkIndex;
	}

	enum
	{
		ChunkTableSize = (MaxTotalElements + ElementsPerChunk - 1) / ElementsPerChunk
	};

	ElementType** Chunks[ChunkTableSize];
	int32_t NumElements;
	int32_t NumChunks;
};

template<class T>
struct TArray
{
	friend struct FString;

public:
	inline TArray()
	{
		Data = nullptr;
		Count = Max = 0;
	};

	inline int Num() const
	{
		return Count;
	};

	inline T& operator[](int i)
	{
		return Data[i];
	};

	inline const T& operator[](int i) const
	{
		return Data[i];
	};

	inline bool IsValidIndex(int i) const
	{
		return i < Num();
	}

	void Add(T InputData)
	{
		Data = (T*)realloc(Data, sizeof(T) * (Count + 1));
		Data[Count++] = InputData;
		Max = Count;
	};

	void Clear()
	{
		free(Data);
		Count = Max = 0;
	};

	void Remove(int32_t ItemIndex)
	{
		int _Count = 0;
		T* _Data = Data;

		Data = (T*)realloc(Data, sizeof(T) * (Count - 1));

		for (int i = 0; i < Count; i++)
		{
			if (i != ItemIndex)
			{
				Data[_Count++] = _Data[i];
			}
		}

		Count--;
		Max = Count;
	}

	T* Data;
	int32_t Count;
	int32_t Max;

private:

};

struct FString : private TArray<wchar_t>
{
	inline FString()
	{
	};

	FString(const wchar_t* other)
	{
		Max = Count = *other ? std::wcslen(other) + 1 : 0;

		if (Count)
		{
			Data = const_cast<wchar_t*>(other);
		}
	};

	inline bool IsValid() const
	{
		return Data != nullptr;
	}

	const wchar_t* ToWString() const
	{
		return Data;
	}

	std::string ToString() const
	{
		auto length = std::wcslen(Data);

		std::string str(length, '\0');

		std::use_facet<std::ctype<wchar_t>>(std::locale()).narrow(Data, Data + length, '?', &str[0]);

		return str;
	}
};

struct FName;


inline void (*FreeInternal)(void*);
void (*FNameToString)(FName* pThis, FString& out);

struct FName
{
	uint32_t ComparisonIndex;
	uint32_t DisplayIndex;

	FName() = default;

	explicit FName(int64_t name)
	{
		DisplayIndex = (name & 0xFFFFFFFF00000000LL) >> 32;
		ComparisonIndex = (name & 0xFFFFFFFFLL);
	};

	FName(uint32_t comparisonIndex, uint32_t displayIndex) : ComparisonIndex(comparisonIndex),
		DisplayIndex(displayIndex)
	{
	}

	auto ToString()
	{
		FString temp;
		FNameToString(this, temp);

		std::string ret(temp.ToString());

		FreeInternal((void*)temp.ToWString());

		return ret;
	}
};

class FUObjectItem
{
public:
	UObject* Object;
	int32_t Flags;
	int32_t ClusterIndex;
	int32_t SerialNumber;

	enum class ObjectFlags : int32_t
	{
		None = 0,
		Native = 1 << 25,
		Async = 1 << 26,
		AsyncLoading = 1 << 27,
		Unreachable = 1 << 28,
		PendingKill = 1 << 29,
		RootSet = 1 << 30,
		NoStrongReference = 1 << 31
	};

	inline bool IsUnreachable() const
	{
		return !!(Flags & static_cast<std::underlying_type_t<ObjectFlags>>(ObjectFlags::Unreachable));
	}
	inline bool IsPendingKill() const
	{
		return !!(Flags & static_cast<std::underlying_type_t<ObjectFlags>>(ObjectFlags::PendingKill));
	}
};

class TUObjectArray
{
public:

	inline int32_t Num() const
	{
		return NumElements;
	}

	inline UObject* GetByIndex(int32_t index) const
	{
		return Objects[index].Object;
	}

	inline FUObjectItem* GetItemByIndex(int32_t index) const
	{
		if (index < NumElements)
		{
			return &Objects[index];
		}
		return nullptr;
	}

	FUObjectItem* Objects;
	int32_t MaxElements;
	int32_t NumElements;

};

// This struct is used on 4.21 and above.
struct TUObjectArrayNew
{
	FUObjectItem* Objects[9];
};

struct GObjects
{
	TUObjectArrayNew* ObjectArray;
	BYTE _padding_0[0xC];
	DWORD ObjectCount;
};

GObjects* NewGObjectsPointer;

bool bAdjusted = false;

class FUObjectArray
{
public:
	TUObjectArray ObjObjects;
};

FUObjectArray* GObjectsPointer;

inline void NumChunks(int* start, int* end)
{
	int cStart = 0, cEnd = 0;

	if (!cEnd)
	{
		while (1)
		{
			if (NewGObjectsPointer->ObjectArray->Objects[cStart] == 0)
			{
				cStart++;
			}
			else
			{
				break;
			}
		}

		cEnd = cStart;
		while (1)
		{
			if (NewGObjectsPointer->ObjectArray->Objects[cEnd] == 0)
			{
				break;
			}
			else
			{
				cEnd++;
			}
		}
	}

	*start = cStart;
	*end = cEnd;
}

static UObject* FindObjectById(uint32_t Id)
{
	if (NewGObjectsPointer != 0)
	{
		// we are on ue 4.21+
		int cStart = 0, cEnd = 0;
		int chunkIndex = 0, chunkSize = 0xFFFF, chunkPos;
		FUObjectItem* Object;

		NumChunks(&cStart, &cEnd);

		chunkIndex = Id / chunkSize;
		if (chunkSize * chunkIndex != 0 &&
			chunkSize * chunkIndex == Id)
		{
			chunkIndex--;
		}

		chunkPos = cStart + chunkIndex;
		if (chunkPos < cEnd)
		{
			Object = NewGObjectsPointer->ObjectArray->Objects[chunkPos] + (Id - chunkSize * chunkIndex);

			if (!Object) { return NULL; }

			return Object->Object;
		}
	}
	else
	{
		// we are on ue 4.20 on lower
		auto Offset = 24 * Id;
		return *(UObject**)(GObjectsPointer->ObjObjects.Objects + Offset);
	}

	return nullptr;
}


class UObject
{
public:
	void** Vtable;
	int32_t ObjectFlags;
	int32_t InternalIndex;
	class UObject* Class;
	FName Name;
	UObject* Outer;

	bool IsA(UObject* cmp)
	{
		if (Class == cmp)
		{
			return true;
		}
		return false;
	}

	string GetObjectNameString()
	{
		string name = Name.ToString();
		auto pos = name.rfind('/');
		if (pos == std::string::npos)
		{
			return name;
		}

		return name.substr(pos + 1);
	}

	string GetFullName()
	{
		std::string temp;

		for (auto outer = this->Outer; outer; outer = outer->Outer)
		{
			temp = outer->GetObjectNameString() + "." + temp;
		}

		temp = reinterpret_cast<UObject*>(Class)->GetObjectNameString() + " " + temp + this->GetObjectNameString();

		return temp;
	}
	string GetObjectNameStringPath()
	{
		string name = Name.ToString();
		return name;
	}

	string GetFullNamePath()
	{
		std::string temp;

		for (auto outer = this->Outer; outer; outer = outer->Outer)
		{
			temp = outer->GetObjectNameStringPath() + "." + temp;
		}

		temp = reinterpret_cast<UObject*>(Class)->GetObjectNameStringPath() + " " + temp + this->GetObjectNameStringPath();

		return temp;
	}

	static UObject* GetObjectFromName(string ObjectName, bool Equal = false, bool busePath = false, bool bUseCustomArray = false, TArray<UObject*> CustomArray = TArray<UObject*>::TArray())
	{
		if (bUseCustomArray)
		{
			for (auto i = 0; i < CustomArray.Num(); i++)
			{
				if (busePath)
				{
					if (CustomArray[i]->GetFullNamePath().find(ObjectName) != string::npos) return CustomArray[i];
				}
				else
				{
					if (CustomArray[i]->GetFullName().find(ObjectName) != string::npos) return CustomArray[i];
				}
			}

			return nullptr;
		}

		int ObjectCount = NewGObjectsPointer ? NewGObjectsPointer->ObjectCount : GObjectsPointer->ObjObjects.NumElements;

		for (auto i = 0; i < ObjectCount; i++)
		{
			if (!busePath)
			{
				UObject* CurrentObject = NewGObjectsPointer ? FindObjectById(i) : GObjectsPointer->ObjObjects.GetByIndex(i);

				if (CurrentObject == nullptr)
				{
					continue;
				}

				auto CurObjectNameString = CurrentObject->GetFullName();

				if (!Equal)
				{
					if (strstr(CurObjectNameString.c_str(), ObjectName.c_str()))
					{
						return CurrentObject;
					}
				}
				else
				{
					if (CurObjectNameString == ObjectName)
					{
						return CurrentObject;
					}
				}
			}
			else if (busePath)
			{
				UObject* CurrentObject = NewGObjectsPointer ? FindObjectById(i) : GObjectsPointer->ObjObjects.GetByIndex(i);

				if (CurrentObject == nullptr)
				{
					continue;
				}

				auto CurObjectNameString = CurrentObject->GetFullNamePath();

				if (!Equal)
				{
					if (CurObjectNameString.find(ObjectName) != string::npos)
					{
						return CurrentObject;
					}
				}
				else
				{
					if (CurObjectNameString == ObjectName)
					{
						return CurrentObject;
					}
				}
			}
		}

		return nullptr;
	}

	static int FindOffset(std::string OffsetToFind)
	{
		auto Object = GetObjectFromName(OffsetToFind, true);

		if (Object)
		{
			return *(uint32_t*)(__int64(Object) + 0x44);
		}

		return 0;
	}
	
	static int FindBitFieldOffset(std::string OffsetToFind)
	{
		auto Object = GetObjectFromName(OffsetToFind, true);

		if (Object)
		{
			__int8 ByteMask = *(__int8*)(__int64(Object) + 0x72);
			if (ByteMask == 1) return 0;
			else if (ByteMask == 2) return 1;
			else if (ByteMask == 4) return 2;
			else if (ByteMask == 8) return 3;
			else if (ByteMask == 16) return 4;
			else if (ByteMask == 32) return 5;
			else if (ByteMask == 64) return 6;
			else if (ByteMask == 128) return 7;
		}

		return 0;
	}

	static void DumpObjects()
	{
		ofstream DumpFile;

		DumpFile.open("GObjects.txt");

		int ObjectCount = NewGObjectsPointer ? NewGObjectsPointer->ObjectCount : GObjectsPointer->ObjObjects.NumElements;

		for (auto i = 0; i < ObjectCount; i++)
		{
			UObject* CurrentObject = NewGObjectsPointer ? FindObjectById(i) : GObjectsPointer->ObjObjects.GetByIndex(i);

			if (CurrentObject == nullptr)
			{
				continue;
			}

			auto CurObjectNameString = CurrentObject->GetFullNamePath();

			
			DumpFile << CurObjectNameString << "\n";
			
		}

		DumpFile.close();
	}
};

template<class TEnum>
class TEnumAsByte
{
public:
	inline TEnumAsByte()
	{
	}

	inline TEnumAsByte(TEnum _value)
		: value(static_cast<uint8_t>(_value))
	{
	}

	explicit inline TEnumAsByte(int32_t _value)
		: value(static_cast<uint8_t>(_value))
	{
	}

	explicit inline TEnumAsByte(uint8_t _value)
		: value(_value)
	{
	}

	inline operator TEnum() const
	{
		return (TEnum)value;
	}

	inline TEnum GetValue() const
	{
		return (TEnum)value;
	}

private:
	uint8_t value;
};

class FWeakObjectPtr
{
	int32_t		objectIndex;
	int32_t		objectSerialNumber;

public:
	inline bool operator==(FWeakObjectPtr other)
	{
		return objectIndex == other.objectIndex && objectSerialNumber == other.objectSerialNumber;
	}
	inline bool operator!=(FWeakObjectPtr other)
	{
		return objectIndex != other.objectIndex || objectSerialNumber != other.objectSerialNumber;
	}
	inline FWeakObjectPtr& operator=(FWeakObjectPtr other)
	{
		objectIndex = other.objectIndex;
		objectSerialNumber = other.objectSerialNumber;
	}

	inline UObject* GetUObject() const
	{
		return  FindObjectById(objectIndex);
	}
};

template<typename ObjectType, class TWeakObjectPtrBase = FWeakObjectPtr>
class TWeakObjectPtr : public TWeakObjectPtrBase
{
	inline UObject* Get() const
	{
		return TWeakObjectPtrBase::GetUObject();
	}
	inline ObjectType* GetCastet() const
	{
		return static_cast<ObjectType*>(Get());
	}
	inline UObject* operator*()
	{
		return Get();
	}
	inline UObject* operator->()
	{
		return Get();
	}
};

struct FVector
{
	float X;
	float Y;
	float Z;
};

struct FRotator
{
	float Pitch;
	float Yaw;
	float Roll;
};

struct FVector2D
{
	float X;            
	float Y; 
};

struct FMargin
{
	float Left;     
	float Top;      
	float Right;    
	float Bottom;   
};

enum class ESlateColorStylingMode : uint8_t
{
	UseColor_Specified = 0,
	UseColor_Specified_Link = 1,
	UseColor_Foreground = 2,
	UseColor_Foreground_Subdued = 3,
	UseColor_MAX = 4
};

struct FLinearColor
{
	float R;         
	float G;         
	float B;         
	float A;         
};

struct FSlateColor
{
	FLinearColor SpecifiedColor;        
	TEnumAsByte<ESlateColorStylingMode> ColorUseRule;          
	unsigned char UnknownData00[0x17];   
};

struct FSlateBrush
{
	unsigned char UnknownData00[0x8];                   
	FVector2D ImageSize;                            
	FMargin Margin;                               
	FSlateColor TintColor;                            
	UObject* ObjectResource;
};

enum class EFortCustomPartType : uint8_t
{
	Head = 0,
	Body = 1,
	Hat = 2,
	Backpack = 3,
	Charm = 4,
	Face = 5,
	NumTypes = 6,
	EFortCustomPartType_MAX = 7
};

enum class EFortMovementStyle : uint8_t
{
	Running = 0,
	Walking = 1,
	Charging = 2,
	Sprinting = 3,
	Hoverboard = 4,
	EFortMovementStyle_MAX = 5
};

enum class EFortQuickBars : uint8_t
{
	Primary = 0,
	Secondary = 1,
	Max_None = 2,
	EFortQuickBars_MAX = 3
};

struct FGuid
{
	int A;       
	int B;       
	int C;       
	int D;       
};

struct ItemCount
{
	char Bytes[0xC];
	int Count;
};

struct SlotFocusParams
{
	EFortQuickBars QuickbarIndex;
	int Slot;
};

enum class EAthenaGamePhase : uint8_t
{
	None = 0,
	Setup = 1,
	Warmup = 2,
	Aircraft = 3,
	SafeZones = 4,
	EndGame = 5,
	Count = 6,
	EAthenaGamePhase_MAX = 7
};

enum class EEngineVersion
{
	UE_4_16,
	UE_4_19,
	UE_4_20,
	UE_4_21,
	UE_NONE
};

enum class ENetRole : uint8_t
{
	ROLE_None = 0,
	ROLE_SimulatedProxy = 1,
	ROLE_AutonomousProxy = 2,
	ROLE_Authority = 3,
	ROLE_MAX = 4
};

enum class EDynamicFoundationType : uint8_t
{
	Static = 0, StartEnabled_Stationary = 1, StartEnabled_Dynamic = 2, StartDisabled = 3, EDynamicFoundationType_MAX = 4
};

struct QuickbarSlot
{
	TArray<struct FGuid> Items;
	bool bEnabled;
	char Unk00[0x7];
};

struct FText
{
	char UnknownData[0x18];
};

struct SpawnActorParams
{
	unsigned char Bytes[0x40];
};
UObject* (*SpawnActor)(UObject* World, UObject* Class, FVector* Position, FRotator* Rotation, const SpawnActorParams& SpawnParameters);

enum class EFortCustomBodyType : uint8_t {
	Small,
	Medium,
	MediumAndSmall,
	Large,
	LargeAndSmall,
	LargeAndMedium,
	All,
	Deprecated,
	EFortCustomBodyType_MAX,
};

enum class EFortCustomGender : uint8_t {
	Invalid,
	Male,
	Female,
	Both,
	EFortCustomGender_MAX,
};

enum class EMontagePlayReturnType : uint8_t {
	MontageLength,
	Duration,
	EMontagePlayReturnType_MAX,
};

enum class EFortBuildingInitializationReason : uint8_t {
	StaticallyPlaced,
	Spawned,
	Replaced,
	LoadedFromSave,
	DynamicBuilderPlaced,
	PlacementTool,
	TrapTool,
	Replay,
	None,
	EFortBuildingInitializationReason_MAX,
};

enum class EFortBuildingPersistentState : uint8_t {
	Default,
	New,
	Constructed,
	Destroyed,
	Searched,
	None,
	EFortBuildingPersistentState_MAX,
};

enum class EFortBuildingType : uint8_t {
	Wall,
	Floor,
	Corner,
	Deco,
	Prop,
	Stairs,
	Roof,
	Pillar,
	SpawnedItem,
	Container,
	Trap,
	GenericCenterCellActor,
	None,
	EFortBuildingType_MAX,
};

struct FURL
{
	char size[0x70];
};

struct FUniqueNetIdWrapper
{
	char UnknownData_0[0x1]; // 0x00(0x01)
};

struct FUniqueNetIdRepl : FUniqueNetIdWrapper
{
	char UnknownData_1[0x17]; // 0x01(0x17)
	struct TArray<char> ReplicationBytes; // 0x18(0x10)
};

enum EChannelType
{
	CHTYPE_None = 0,
	CHTYPE_Control = 1,
	CHTYPE_Actor = 2,
	CHTYPE_File = 3,
	CHTYPE_Voice = 4,
	CHTYPE_MAX = 8,
};

enum EConnectionState
{
	USOCK_Invalid = 0,
	// Connection is invalid, possibly uninitialized.
	USOCK_Closed = 1,
	// Connection permanently closed.
	USOCK_Pending = 2,
	// Connection is awaiting connection.
	USOCK_Open = 3,
	// Connection is open.
};

enum class ENetDormancy : uint8_t
{
	DORM_Never,
	DORM_Awake,
	DORM_DormantAll,
	DORM_DormantPartial,
	DORM_Initial,
	DORM_MAX,
};

namespace EAcceptConnection
{
	enum Type
	{
		/** Reject the connection */
		Reject,
		/** Accept the connection */
		Accept,
		/** Ignore the connection, sending no reply, while server traveling */
		Ignore
	};
};

float lerp(float a, float b, float f)
{
	return a + f * (b - a); // just use std::lerp bruh
	//std::lerp(a, b, f);
}

struct AActor
{
	unsigned char Unk00[0x80];
	char bHidden : 1; // 0x80(0x01)
	char bNetTemporary : 1; // 0x80(0x01)
	char bNetStartup : 1; // 0x80(0x01)
	char bOnlyRelevantToOwner : 1; // 0x80(0x01)
	char bAlwaysRelevant : 1; // 0x80(0x01)
	char bReplicateMovement : 1; // 0x80(0x01)
	char bTearOff : 1; // 0x80(0x01)
	char bExchangedRoles : 1; // 0x80(0x01)
};

struct FActorSpawnParameters
{
	FActorSpawnParameters() : Name(),
		Template(nullptr),
		Owner(nullptr),
		Instigator(nullptr),
		OverrideLevel(nullptr),
		SpawnCollisionHandlingOverride(),
		bRemoteOwned(0),
		bNoFail(0),
		bDeferConstruction(0),
		bAllowDuringConstructionScript(0),
		NameMode(),
		ObjectFlags()
	{
	}
	;

	FName Name;
	UObject* Template;
	UObject* Owner;
	UObject* Instigator;
	UObject* OverrideLevel;
	uint8_t SpawnCollisionHandlingOverride;

private:
	uint8_t bRemoteOwned : 1;

public:
	bool IsRemoteOwned() const { return bRemoteOwned; }

	uint8_t bNoFail : 1;
	uint8_t bDeferConstruction : 1;
	uint8_t bAllowDuringConstructionScript : 1;
	uint8_t NameMode;
	uint8_t ObjectFlags;
};

// a replica of the engine struct but with our own custom implementation due to the original implementation using shared pointers
struct FNetworkObjectInfo
{
	UObject* Actor;
	uint8_t bDirtyForReplay : 1;
	uint8_t bPendingNetUpdate : 1;
	uint8_t bSwapRolesOnReplicate;
	uint32_t ForceRelevantFrame;
	double LastNetReplicateTime;
	float LastNetUpdateTime;
	double NextUpdateTime;
	float OptimalNetUpdateDelta;

	FNetworkObjectInfo(UObject*);
};

FNetworkObjectInfo::FNetworkObjectInfo(UObject* InActor)
{
	this->Actor = InActor;
	this->NextUpdateTime = 0.0;
	this->LastNetReplicateTime = 0.0;
	this->OptimalNetUpdateDelta = 0.0f;
	this->LastNetUpdateTime = 0.0f;
	this->bPendingNetUpdate = false;
	this->bDirtyForReplay = false;
	this->bSwapRolesOnReplicate = false;
}

struct FQuat
{
	float X;
	float Y;
	float Z;
	float W;
};

struct FTransform
{
	FQuat Rotation;
	FVector Location;
	char Unk00[0x4];
	FVector Scale3D;
	char Unk01[0x4];
};

struct SoftObjectPtr
{
	unsigned char pad[0x28];
};

struct AssetPtr
{
	unsigned char pad[0x20];
};

struct FKey
{
	FName keyname;
	char pad[0x10];
};

struct BitFieldStruct
{
	bitset<8> bitfield;
};

#endif // !UE4_H