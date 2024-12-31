#include "aimbot.h"
#include "../../core/config.h"
#include "../../core/utils/util.h"
#include "../../game/game.h"
#include "../../game/sdk.h"
#include "../../../ext/imgui/imgui.h"
#include <Windows.h>
#include <cmath>
#include <vector>

namespace Features {
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

    void UpdateAimbot() {
        if (!Config::AimbotEnabled) return;

        if (!GetAsyncKeyState(Config::AimbotKey)) return;

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
        if (!LocalPawn) return;

        // find best target
        SDK::APawn* BestTarget = nullptr;
        float BestDist = Config::AimbotFOV;

        int ScreenWidth = 0, ScreenHeight = 0;
        PlayerController->GetViewportSize(&ScreenWidth, &ScreenHeight);

        if (ScreenWidth == 0 || ScreenHeight == 0) {
            ImGuiIO& io = ImGui::GetIO();
            ScreenWidth = (int)io.DisplaySize.x;
            ScreenHeight = (int)io.DisplaySize.y;
        }

        float ScreenCenterX = (float)ScreenWidth / 2.0f;
        float ScreenCenterY = (float)ScreenHeight / 2.0f;

        for (int i = 0; i < GameState->PlayerArray.Num(); i++) {
            SDK::APlayerState* PlayerState = GameState->PlayerArray[i];
            if (!PlayerState) continue;

            SDK::APawn* Pawn = PlayerState->PawnPrivate;
            if (!Pawn || Pawn == LocalPawn) continue;

            SDK::USkeletalMeshComponent* Mesh = GetPawnMesh(Pawn);
            if (!Mesh) continue;

            static SDK::FName HeadBone = SDK::BasicFilesImpleUtils::StringToName(L"head");
            SDK::FVector HeadPos = Mesh->GetSocketLocation(HeadBone);
            if (HeadPos.X == 0 && HeadPos.Y == 0 && HeadPos.Z == 0) continue; 
            
            SDK::FVector2D HeadScreen;
            if (!PlayerController->ProjectWorldLocationToScreen(HeadPos, &HeadScreen, false)) continue;

            // FOV check
            float DistX = HeadScreen.X - ScreenCenterX;
            float DistY = HeadScreen.Y - ScreenCenterY;
            float SafeDist = std::sqrt(DistX * DistX + DistY * DistY);

            if (SafeDist < BestDist) {
                BestDist = SafeDist;
                BestTarget = Pawn;
            }
        }

        if (BestTarget) {
             SDK::USkeletalMeshComponent* Mesh = GetPawnMesh(BestTarget);
             if (Mesh) {
                 static SDK::FName HeadBone = SDK::BasicFilesImpleUtils::StringToName(L"head");
                 SDK::FVector HeadPos = Mesh->GetSocketLocation(HeadBone);
                 SDK::FVector2D HeadScreen;
                 if (PlayerController->ProjectWorldLocationToScreen(HeadPos, &HeadScreen, false)) {
                     Utils::MouseMove(HeadScreen.X, HeadScreen.Y, (float)ScreenWidth, (float)ScreenHeight, Config::AimbotSmoothness);
                     
                     if (Config::AimbotDrawFOV) {
                        ImGui::GetForegroundDrawList()->AddLine(
                            ImVec2(ScreenCenterX, ScreenCenterY),
                            ImVec2(HeadScreen.X, HeadScreen.Y),
                            IM_COL32(255, 0, 0, 255)
                        );
                     }
                 }
             }
        }
    }

    void RenderAimbotFOV() {
        if (!Config::AimbotEnabled || !Config::AimbotDrawFOV) return;

        ImGuiIO& io = ImGui::GetIO();
        ImGui::GetForegroundDrawList()->AddCircle(
            ImVec2(io.DisplaySize.x / 2.0f, io.DisplaySize.y / 2.0f),
            Config::AimbotFOV,
            IM_COL32(255, 255, 255, 255),
            64,
            2.0f
        );
    }
}
