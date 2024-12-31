#pragma once
#include <cstdint>
#include <cmath>
#include <string>
#include <Windows.h>

namespace SDK {
    struct FVector {
        float X = 0.0f;
        float Y = 0.0f;
        float Z = 0.0f;

        float Distance(const FVector& V) const {
            float dx = X - V.X;
            float dy = Y - V.Y;
            float dz = Z - V.Z;
            return std::sqrt(dx * dx + dy * dy + dz * dz);
        }

        bool IsZero() const {
            return X == 0.0f && Y == 0.0f && Z == 0.0f;
        }
    };

    struct FVector2D {
        float X = 0.0f;
        float Y = 0.0f;
    };

    template<typename T>
    struct TArray {
        T* Data = nullptr;
        int32_t Count = 0;
        int32_t Max = 0;

        int32_t Num() const { return Count; }
        T& operator[](int32_t i) { return Data[i]; }
        const T& operator[](int32_t i) const { return Data[i]; }
    };

    struct FString {
        const wchar_t* Data = nullptr;
        int32_t Count = 0;
        int32_t Max = 0;

        FString() = default;
        FString(const wchar_t* str) {
            Data = str;
            Count = str ? static_cast<int32_t>(wcslen(str)) + 1 : 0;
            Max = Count;
        }
    };

    struct FName {
        int32_t ComparisonIndex = 0;
        uint32_t Number = 0;

        FName() = default;
        constexpr FName(int32_t index, uint32_t num = 0) : ComparisonIndex(index), Number(num) {}
    };

    class UFunction;
    class UClass;

    class UObject {
    public:
        void** VTable;             // 0x0000
        int32_t ObjectFlags;       // 0x0008
        int32_t InternalIndex;     // 0x000C
        UClass* Class;             // 0x0010
        FName Name;                // 0x0018
        UObject* Outer;            // 0x0020

        void ProcessEvent(UFunction* Function, void* Parms) const;
        bool IsA(UClass* TargetClass) const;

        std::string GetName() const;
        std::string GetFullName() const;

        static UObject* FindObject(const char* Name);
    };

    class UClass : public UObject {
    public:
        static UClass* StaticClass();
    };

    class USkeletalMeshComponent : public UObject {
    public:
        FVector GetSocketLocation(FName InSocketName) const;
    };

    class APawn : public UObject {
    public:
        static UClass* StaticClass();
        FVector K2_GetActorLocation() const;
        float GetDistanceTo(APawn* Other) const;
    };

    class ACharacter : public APawn {
    public:
        uint8_t Pad_0[0x0280 - sizeof(APawn)];
        USkeletalMeshComponent* Mesh; // 0x0280

        static UClass* StaticClass();
    };

    class ACharacterDummy : public APawn {
    public:
        uint8_t Pad_0[0x0280 - sizeof(APawn)];
        USkeletalMeshComponent* _mesh; // 0x0280

        static UClass* StaticClass();
    };

    class APlayerController : public UObject {
    public:
        uint8_t Pad_0[0x02A0 - sizeof(UObject)];
        APawn* AcknowledgedPawn; // 0x02A0

        APawn* K2_GetPawn() const;
        void GetViewportSize(int* SizeX, int* SizeY) const;
        bool ProjectWorldLocationToScreen(const FVector& WorldLocation, FVector2D* ScreenLocation, bool bPlayerViewportRelative = false) const;
    };

    class APlayerState : public UObject {
    public:
        uint8_t Pad_0[0x0280 - sizeof(UObject)];
        APawn* PawnPrivate; // 0x0280
    };

    class AGameStateBase : public UObject {
    public:
        uint8_t Pad_0[0x0238 - sizeof(UObject)];
        TArray<APlayerState*> PlayerArray; // 0x0238
    };

    class ULocalPlayer : public UObject {
    public:
        uint8_t Pad_0[0x0030 - sizeof(UObject)];
        APlayerController* PlayerController; // 0x0030
    };

    class UGameInstance : public UObject {
    public:
        uint8_t Pad_0[0x0038 - sizeof(UObject)];
        TArray<ULocalPlayer*> LocalPlayers; // 0x0038
    };

    class UWorld : public UObject {
    public:
        uint8_t Pad_0[0x0120 - sizeof(UObject)];
        AGameStateBase* GameState; // 0x0120
        uint8_t Pad_1[0x0180 - (0x0120 + sizeof(AGameStateBase*))];
        UGameInstance* OwningGameInstance; // 0x0180
    };

    namespace BasicFilesImpleUtils {
        FName StringToName(const wchar_t* Name);
    }
}
