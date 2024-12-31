#include "gui.h"
#include "gui_helpers.h"
#include "../core/renderer/renderer.h"
#include "../core/config.h"
#include "../../ext/imgui/imgui.h"
#include "../game/game.h"

namespace GUI {
	void Render() {
		if (!Renderer::ShowMenu) return;

		ImGui::SetNextWindowPos(ImVec2(100, 100), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(700, 400), ImGuiCond_Always);

		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar;

		if (ImGui::Begin("MHUR Internal", &Renderer::ShowMenu, windowFlags)) {

			// title bar
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5, 5));
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));

			ImGui::TextColored(ImVec4(0.28f, 0.56f, 1.00f, 1.00f), "MHUR");
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(0.9f, 0.9f, 0.9f, 0.8f), "INTERNAL");
			ImGui::SameLine(ImGui::GetWindowWidth() - 30);
			if (ImGui::Button("X")) Renderer::ShowMenu = false;

			ImGui::PopStyleColor(3);
			ImGui::PopStyleVar();
			ImGui::Separator();

			ImGui::BeginChild("MainContent", ImVec2(0, 0), true);

			ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "Player Movement");
			ImGui::Spacing();

			GUI::ToggleButton("##MasterSwitch", &Config::SpeedMultiplierEnabled);

			ImGui::SameLine();
			ImGui::Text("Master Switch");
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "(movement & combat)");

			if (Config::SpeedMultiplierEnabled) {
				ImGui::Indent();
				ImGui::SliderFloat("##multiplier", &Config::SpeedMultiplierValue, 0.1f, 10.0f, "Scale: %.1fx");
				ImGui::Unindent();
			}

			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			ImGui::Columns(2, nullptr, false);

			ImGui::Text("Walking");
			ImGui::Spacing();

			GUI::ToggleButton("##EnableWalk", &Config::WalkSpeedEnabled);

			ImGui::SameLine();
			ImGui::Text("Enable Speed Exploit");

			if (Config::WalkSpeedEnabled) {
				ImGui::SliderFloat("##walk_speed", &Config::WalkSpeedValue, 100.0f, 5000.0f, "%.0f");
			}

			ImGui::NextColumn();

			ImGui::Text("Sprinting");
			ImGui::Spacing();

			GUI::ToggleButton("##EnableRun", &Config::RunSpeedEnabled);

			ImGui::SameLine();
			ImGui::Text("Enable Speed Exploit");

			if (Config::RunSpeedEnabled) {
				ImGui::SliderFloat("##run_speed", &Config::RunSpeedValue, 100.0f, 5000.0f, "%.0f");
			}

			ImGui::Columns(1);

			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "Abilities & Misc");
			ImGui::Spacing();

			GUI::ToggleButton("##JumpHack", &Config::JumpEnabled);
			ImGui::SameLine();
			ImGui::Text("Jump in air");
			ImGui::Spacing();

			GUI::ToggleButton("##PlusUltra", &Config::PlusUltraEnabled);
			ImGui::SameLine();
			ImGui::Text("Max Plus Ultra");
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "(Always 100%)");

			ImGui::Spacing();

			GUI::ToggleButton("##InfSkill", &Config::SkillEnabled);
			ImGui::SameLine();
			ImGui::Text("No Skill Cooldown");

			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "Combative (Aimbot)");
			ImGui::Spacing();

			GUI::ToggleButton("##AimbotEnabled", &Config::AimbotEnabled);
			ImGui::SameLine();
			ImGui::Text("Enable Aimbot");

			if (Config::AimbotEnabled) {
				ImGui::Indent();
				ImGui::SliderFloat("##AimbotFOV", &Config::AimbotFOV, 10.0f, 800.0f, "FOV: %.0f");
				ImGui::SliderFloat("##AimbotSmooth", &Config::AimbotSmoothness, 3.0f, 50.0f, "Smoothness: %.1f");
				GUI::ToggleButton("##DrawFOV", &Config::AimbotDrawFOV);
				ImGui::SameLine();
				ImGui::Text("Draw FOV Circle");
				
				ImGui::Spacing();
				ImGui::TextDisabled("Hold Right Click to Aim");
				ImGui::Unindent();
			}

            ImGui::Spacing();
			ImGui::Separator();

			ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "Visuals (ESP)");
			ImGui::Spacing();

			GUI::ToggleButton("##ESP_Master", &Config::EspEnabled);
			ImGui::SameLine();
			ImGui::Text("Enable ESP");

			if (Config::EspEnabled) {
				ImGui::Indent();

				ImGui::Columns(3, nullptr, false);

				GUI::ToggleButton("##EspBox", &Config::EspBox);
				ImGui::SameLine();
				ImGui::Text("2D Box");
				
				ImGui::PushItemWidth(100);
				ImGui::SliderFloat("##EspDist", &Config::EspMaxDistance, 1000.0f, 50000.0f, "Dist: %.0f", ImGuiSliderFlags_Logarithmic);
				ImGui::PopItemWidth();

				if (Config::EspBox) {
					GUI::ToggleButton("##BoxRainbow", &Config::EspBoxRainbow);
					ImGui::SameLine();
					ImGui::Text("Rainbow");

					if (!Config::EspBoxRainbow) {
						GUI::PushColorPickerStyle();
						ImGui::ColorEdit3("##BoxColor", Config::espBoxColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
						GUI::PopColorPickerStyle();
					}
				}

				ImGui::NextColumn();

				GUI::ToggleButton("##EspSkeleton", &Config::EspSkeleton);
				ImGui::SameLine();
				ImGui::Text("Skeleton");
				if (Config::EspSkeleton) {
					GUI::ToggleButton("##SkeletonRainbow", &Config::EspSkeletonRainbow);
					ImGui::SameLine();
					ImGui::Text("Rainbow");

					if (!Config::EspSkeletonRainbow) {
						GUI::PushColorPickerStyle();
						ImGui::ColorEdit3("##SkeletonColor", Config::espSkeletonColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
						GUI::PopColorPickerStyle();
					}
				}

				ImGui::NextColumn();

				GUI::ToggleButton("##EspSnaplines", &Config::EspSnapline);
				ImGui::SameLine();
				ImGui::Text("Snaplines");
				if (Config::EspSnapline) {
					GUI::ToggleButton("##SnaplineRainbow", &Config::EspSnaplineRainbow);
					ImGui::SameLine();
					ImGui::Text("Rainbow");

					if (!Config::EspSnaplineRainbow) {
						GUI::PushColorPickerStyle();
						ImGui::ColorEdit3("##SnaplineColor", Config::espSnaplineColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
						GUI::PopColorPickerStyle();
					}

					ImGui::SetNextItemWidth(80);
					const char* positions[] = { "Bottom", "Top" };
					GUI::PushComboStyle();
					ImGui::Combo("##SnaplinePos", &Config::espSnaplinePosition, positions, 2);
					GUI::PopComboStyle();
					ImGui::SameLine();
					ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "From");
				}

				ImGui::Columns(1);
				ImGui::Unindent();
			}

			ImGui::EndChild();

			ImGui::Spacing();
			ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "Player State:");
			ImGui::SameLine();
			if ((Game::Player::IsValid())) {
				ImGui::TextColored(ImVec4(0.4f, 1.0f, 0.4f, 1.0f), "HOOKED");
			}
			else {
				ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f), "NOT IN MATCH");
			}
			ImGui::SameLine(ImGui::GetWindowWidth() - 120);
			ImGui::TextDisabled("v1.0.0");

			ImGui::End();
		}
	}
}
