#include "gui_helpers.h"
#include "../../ext/imgui/imgui.h"

namespace GUI {
	void PushColorPickerStyle() {
		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.18f, 0.20f, 0.24f, 1.00f));
		ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.28f, 0.32f, 0.38f, 1.00f));
		ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.34f, 0.39f, 0.46f, 1.00f));
		ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.12f, 0.14f, 0.16f, 0.98f));
		ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.40f, 0.45f, 0.52f, 0.50f));
	}

	void PopColorPickerStyle() {
		ImGui::PopStyleColor(5);
	}

	void PushComboStyle() {
		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.18f, 0.20f, 0.24f, 1.00f));
		ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.35f, 0.40f, 0.48f, 1.00f));
		ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.12f, 0.14f, 0.16f, 0.98f));
		ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.28f, 0.56f, 1.00f, 0.40f));
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.28f, 0.56f, 1.00f, 0.60f));
		ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.28f, 0.56f, 1.00f, 0.80f));
		ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.40f, 0.45f, 0.52f, 0.50f));
	}

	void PopComboStyle() {
		ImGui::PopStyleColor(7);
	}
}
