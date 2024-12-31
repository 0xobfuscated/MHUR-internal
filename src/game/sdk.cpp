#include "sdk.h"
#include "../core/utils/memory.h"
#include "offsets.h"
#include <unordered_map>

namespace SDK {
    struct FUObjectItem {
        UObject* Object;
        int32_t Flags;
        int32_t ClusterRootIndex;
        int32_t SerialNumber;
    };

    struct TUObjectArray {
        FUObjectItem** Objects;
        uint8_t Pad_8[8];
        int32_t MaxElements;
        int32_t NumElements;
        int32_t MaxChunks;
        int32_t NumChunks;

        UObject* GetByIndex(int32_t Index) const {
            if (Index < 0 || Index >= NumElements) return nullptr;
            int32_t ChunkIndex = Index / 65536;
            int32_t InChunkIdx = Index % 65536;
            if (!Objects || ChunkIndex >= NumChunks) return nullptr;
            FUObjectItem* Chunk = Objects[ChunkIndex];
            if (!Chunk) return nullptr;
            return Chunk[InChunkIdx].Object;
        }
    };

    static TUObjectArray* GetGObjects() {
        uintptr_t base = Memory::GetImageBase();
        if (!base) return nullptr;
        return *reinterpret_cast<TUObjectArray**>(base + Game::Offsets::GObjects);
    }

    void UObject::ProcessEvent(UFunction* Function, void* Parms) const {
        if (!this || !Function) return;
        typedef void(*tProcessEvent)(const UObject*, UFunction*, void*);
        void** vtable = *reinterpret_cast<void***>(const_cast<UObject*>(this));
        if (!vtable) return;
        tProcessEvent processEvent = reinterpret_cast<tProcessEvent>(vtable[68]);
        if (processEvent) {
            processEvent(this, Function, Parms);
        }
    }

    std::string UObject::GetName() const {
        if (!this) return "";
        return "";
    }

    std::string UObject::GetFullName() const {
        if (!this) return "";
        return "";
    }

    UObject* UObject::FindObject(const char* TargetName) {
        TUObjectArray* gObjects = GetGObjects();
        if (!gObjects || !TargetName) return nullptr;

        static std::unordered_map<std::string, UObject*> Cache;
        auto it = Cache.find(TargetName);
        if (it != Cache.end()) return it->second;

        for (int32_t i = 0; i < gObjects->NumElements; i++) {
            UObject* obj = gObjects->GetByIndex(i);
            if (!obj) continue;
        }

        return nullptr;
    }

    bool UObject::IsA(UClass* TargetClass) const {
        if (!this || !TargetClass) return false;
        for (UClass* super = Class; super; super = reinterpret_cast<UClass*>(super->Outer)) {
            if (super == TargetClass) return true;
        }
        return true;
    }

    UClass* UClass::StaticClass() {
        static UClass Clss{};
        return &Clss;
    }

    UClass* APawn::StaticClass() {
        static UClass Clss{};
        return &Clss;
    }

    UClass* ACharacter::StaticClass() {
        static UClass Clss{};
        return &Clss;
    }

    UClass* ACharacterDummy::StaticClass() {
        static UClass Clss{};
        return &Clss;
    }

    FVector USkeletalMeshComponent::GetSocketLocation(FName InSocketName) const {
        struct {
            FName InSocketName;
            FVector ReturnValue;
        } params{ InSocketName };

        static UFunction* func = nullptr;
        if (!func) {
            func = reinterpret_cast<UFunction*>(UObject::FindObject("GetSocketLocation"));
        }

        if (func) {
            ProcessEvent(func, &params);
        }

        return params.ReturnValue;
    }

    APawn* APlayerController::K2_GetPawn() const {
        if (AcknowledgedPawn) return AcknowledgedPawn;
        return nullptr;
    }

    void APlayerController::GetViewportSize(int* SizeX, int* SizeY) const {
        struct {
            int32_t SizeX;
            int32_t SizeY;
        } params{};

        static UFunction* func = nullptr;
        if (!func) {
            func = reinterpret_cast<UFunction*>(UObject::FindObject("GetViewportSize"));
        }

        if (func) {
            ProcessEvent(func, &params);
        }

        if (SizeX) *SizeX = params.SizeX;
        if (SizeY) *SizeY = params.SizeY;
    }

    bool APlayerController::ProjectWorldLocationToScreen(const FVector& WorldLocation, FVector2D* ScreenLocation, bool bPlayerViewportRelative) const {
        struct {
            FVector WorldLocation;
            FVector2D ScreenLocation;
            bool bPlayerViewportRelative;
            bool ReturnValue;
        } params{};

        params.WorldLocation = WorldLocation;
        params.bPlayerViewportRelative = bPlayerViewportRelative;

        static UFunction* func = nullptr;
        if (!func) {
            func = reinterpret_cast<UFunction*>(UObject::FindObject("ProjectWorldLocationToScreen"));
        }

        if (func) {
            ProcessEvent(func, &params);
        }

        if (ScreenLocation) *ScreenLocation = params.ScreenLocation;
        return params.ReturnValue;
    }

    FVector APawn::K2_GetActorLocation() const {
        struct {
            FVector ReturnValue;
        } params{};

        static UFunction* func = nullptr;
        if (!func) {
            func = reinterpret_cast<UFunction*>(UObject::FindObject("K2_GetActorLocation"));
        }

        if (func) {
            ProcessEvent(func, &params);
        }

        return params.ReturnValue;
    }

    float APawn::GetDistanceTo(APawn* Other) const {
        if (!this || !Other) return 0.0f;
        FVector loc1 = K2_GetActorLocation();
        FVector loc2 = Other->K2_GetActorLocation();
        return loc1.Distance(loc2);
    }

    namespace BasicFilesImpleUtils {
        FName StringToName(const wchar_t* Name) {
            static int32_t Counter = 1;
            return FName(Counter++);
        }
    }
}
