#include "esp.h"
#include "../../core/config.h"
#include "../../core/utils/console.h"
#include "../../core/utils/util.h"
#include "../../game/game.h"
#include "../../game/sdk.h"
#include "../../../ext/imgui/imgui.h"

#include <Windows.h>

using namespace SDK;

namespace {
    SDK::USkeletalMeshComponent* GetPawnMesh(SDK::APawn* Pawn) {
        if (!Pawn) return nullptr;

        if (Pawn->IsA(SDK::ACharacter::StaticClass())) {
            SDK::ACharacter* Character = reinterpret_cast<SDK::ACharacter*>(Pawn);
            return Character->Mesh;
        }
        
        if (Pawn->IsA(SDK::ACharacterDummy::StaticClass())) {
            SDK::ACharacterDummy* Character = reinterpret_cast<SDK::ACharacterDummy*>(Pawn);
            return reinterpret_cast<SDK::USkeletalMeshComponent*>(Character->_mesh);
        }

        return nullptr;
    }

    struct BoneNames {
        // body
        SDK::FName Head = SDK::BasicFilesImpleUtils::StringToName(L"head");
        SDK::FName Neck = SDK::BasicFilesImpleUtils::StringToName(L"neck");
        SDK::FName Chest = SDK::BasicFilesImpleUtils::StringToName(L"chest");
        SDK::FName Spine = SDK::BasicFilesImpleUtils::StringToName(L"spine");
        SDK::FName Hip = SDK::BasicFilesImpleUtils::StringToName(L"hip");
        SDK::FName Root = SDK::BasicFilesImpleUtils::StringToName(L"Root");
        
        // left arm
        SDK::FName ArmL = SDK::BasicFilesImpleUtils::StringToName(L"L_arm");
        SDK::FName ElbowL = SDK::BasicFilesImpleUtils::StringToName(L"L_elbow");
        SDK::FName HandL = SDK::BasicFilesImpleUtils::StringToName(L"L_hand");
        
        // right arm
        SDK::FName ArmR = SDK::BasicFilesImpleUtils::StringToName(L"R_arm");
        SDK::FName ElbowR = SDK::BasicFilesImpleUtils::StringToName(L"R_elbow");
        SDK::FName HandR = SDK::BasicFilesImpleUtils::StringToName(L"R_hand");
        
        // left leg
        SDK::FName LegL = SDK::BasicFilesImpleUtils::StringToName(L"L_leg");
        SDK::FName KneeL = SDK::BasicFilesImpleUtils::StringToName(L"L_knee");
        SDK::FName AnkleL = SDK::BasicFilesImpleUtils::StringToName(L"L_ankle");
        
        // right leg
        SDK::FName LegR = SDK::BasicFilesImpleUtils::StringToName(L"R_leg");
        SDK::FName KneeR = SDK::BasicFilesImpleUtils::StringToName(L"R_knee");
        SDK::FName AnkleR = SDK::BasicFilesImpleUtils::StringToName(L"R_ankle");
    };

    static BoneNames Bones;

    void DrawSkeleton(SDK::USkeletalMeshComponent* Mesh, SDK::APlayerController* PlayerController) {
        if (!Mesh || !PlayerController || !Config::EspSkeleton) return;

        ImU32 color = Config::EspSkeletonRainbow ? Utils::GetRainbowColor() : Utils::ColorToImU32(Config::espSkeletonColor);

        auto DrawBoneLine = [&](SDK::FName BoneA, SDK::FName BoneB) {
            SDK::FVector PosA = Mesh->GetSocketLocation(BoneA);
            SDK::FVector PosB = Mesh->GetSocketLocation(BoneB);
            SDK::FVector2D ScreenA, ScreenB;

            if (PlayerController->ProjectWorldLocationToScreen(PosA, &ScreenA, false) &&
                PlayerController->ProjectWorldLocationToScreen(PosB, &ScreenB, false)) {
                ImGui::GetBackgroundDrawList()->AddLine(
                    ImVec2(ScreenA.X, ScreenA.Y),
                    ImVec2(ScreenB.X, ScreenB.Y),
                    color
                );
            }
        };

        // torso
        DrawBoneLine(Bones.Head, Bones.Neck);
        DrawBoneLine(Bones.Neck, Bones.Chest);
        DrawBoneLine(Bones.Chest, Bones.Spine);
        DrawBoneLine(Bones.Spine, Bones.Hip);

        // left arm
        DrawBoneLine(Bones.Chest, Bones.ArmL);
        DrawBoneLine(Bones.ArmL, Bones.ElbowL);
        DrawBoneLine(Bones.ElbowL, Bones.HandL);

        // right arm
        DrawBoneLine(Bones.Chest, Bones.ArmR);
        DrawBoneLine(Bones.ArmR, Bones.ElbowR);
        DrawBoneLine(Bones.ElbowR, Bones.HandR);

        // left leg
        DrawBoneLine(Bones.Hip, Bones.LegL);
        DrawBoneLine(Bones.LegL, Bones.KneeL);
        DrawBoneLine(Bones.KneeL, Bones.AnkleL);

        // right leg
        DrawBoneLine(Bones.Hip, Bones.LegR);
        DrawBoneLine(Bones.LegR, Bones.KneeR);
        DrawBoneLine(Bones.KneeR, Bones.AnkleR);
    }

    bool GetPlayerPositions(SDK::APawn* Pawn, SDK::USkeletalMeshComponent* Mesh, 
                           SDK::FVector& OutHeadPos, SDK::FVector& OutRootPos) {
        if (!Pawn) return false;

        if (Mesh) {
            OutHeadPos = Mesh->GetSocketLocation(Bones.Head);
            OutRootPos = Mesh->GetSocketLocation(Bones.Root);
            
            if (OutRootPos.X == 0 && OutRootPos.Y == 0 && OutRootPos.Z == 0) {
                OutRootPos = Mesh->GetSocketLocation(Bones.Hip);
            }

            if (OutHeadPos.X != 0 || OutHeadPos.Y != 0 || OutHeadPos.Z != 0) {
                return true;
            }
        }

        SDK::FVector Loc = Pawn->K2_GetActorLocation();
        OutHeadPos = Loc;
        OutHeadPos.Z += 88.0f;
        OutRootPos = Loc;
        OutRootPos.Z -= 88.0f;
        return true;
    }

    void DrawBox(const SDK::FVector2D& HeadScreen, const SDK::FVector2D& RootScreen) {
        if (!Config::EspBox) return;

        float BoxHeight = abs(RootScreen.Y - HeadScreen.Y);
        float BoxWidth = BoxHeight * 0.6f;
        ImU32 color = Config::EspBoxRainbow ? Utils::GetRainbowColor() : Utils::ColorToImU32(Config::espBoxColor);

        ImGui::GetBackgroundDrawList()->AddRect(
            ImVec2(HeadScreen.X - BoxWidth / 2, HeadScreen.Y),
            ImVec2(HeadScreen.X + BoxWidth / 2, RootScreen.Y),
            color
        );
    }

    void DrawSnapline(const SDK::FVector2D& RootScreen) {
        if (!Config::EspSnapline) return;

        ImU32 color = Config::EspSnaplineRainbow ? Utils::GetRainbowColor() : Utils::ColorToImU32(Config::espSnaplineColor);
        ImVec2 displaySize = ImGui::GetIO().DisplaySize;
        
        float startY = (Config::espSnaplinePosition == 1) ? 0.0f : displaySize.y;
        
        ImGui::GetBackgroundDrawList()->AddLine(
            ImVec2(displaySize.x / 2, startY),
            ImVec2(RootScreen.X, RootScreen.Y),
            color
        );
    }
}

namespace Features {
    void RenderESP() {
        if (!Config::EspEnabled) return;

        SDK::UWorld* World = Game::GetWorld();
        if (!World) return;

        SDK::AGameStateBase* GameState = World->GameState;
        if (!GameState) return;

        SDK::UGameInstance* GameInstance = World->OwningGameInstance;
        if (!GameInstance) return;

        if (GameInstance->LocalPlayers.Num() == 0) return;
        SDK::ULocalPlayer* LocalPlayer = GameInstance->LocalPlayers[0];
        if (!LocalPlayer) return;

        SDK::APlayerController* PlayerController = LocalPlayer->PlayerController;
        if (!PlayerController) return;

        SDK::APawn* LocalPawn = PlayerController->AcknowledgedPawn;
        if (!LocalPawn) LocalPawn = PlayerController->K2_GetPawn();

        for (int i = 0; i < GameState->PlayerArray.Num(); i++) {
            SDK::APlayerState* PlayerState = GameState->PlayerArray[i];
            if (!PlayerState) continue;

            SDK::APawn* Pawn = PlayerState->PawnPrivate;
            if (!Pawn || Pawn == LocalPawn) continue;

            SDK::USkeletalMeshComponent* Mesh = GetPawnMesh(Pawn);

            DrawSkeleton(Mesh, PlayerController);

            SDK::FVector HeadPos, RootPos;
            if (!GetPlayerPositions(Pawn, Mesh, HeadPos, RootPos)) continue;

            float Dist = LocalPawn->GetDistanceTo(Pawn);
            if (Dist > Config::EspMaxDistance) continue;

            SDK::FVector2D HeadScreen, RootScreen;
            if (!PlayerController->ProjectWorldLocationToScreen(HeadPos, &HeadScreen, false) ||
                !PlayerController->ProjectWorldLocationToScreen(RootPos, &RootScreen, false)) continue;

            DrawBox(HeadScreen, RootScreen);
            DrawSnapline(RootScreen);
        }
    }
}
