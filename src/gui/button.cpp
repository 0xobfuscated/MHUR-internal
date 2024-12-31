#include "gui.h"
#include "../core/renderer/renderer.h"
#include "../../ext/imgui/imgui.h"

namespace GUI {
	void ToggleButton(const char* strId, bool* isToggled)
	{
		ImVec2 cursorPos = ImGui::GetCursorScreenPos();
		ImDrawList* drawList = ImGui::GetWindowDrawList();

		float height = ImGui::GetFrameHeight();
		float width = height; // square button

		if (ImGui::InvisibleButton(strId, ImVec2(width, height)))
			*isToggled = !*isToggled;

		ImU32 bgColor;
		if (ImGui::IsItemHovered())
			bgColor = *isToggled ? ImGui::GetColorU32(ImGuiCol_ButtonActive) : ImGui::GetColorU32(ImGuiCol_FrameBgHovered);
		else
			bgColor = *isToggled ? ImGui::GetColorU32(ImGuiCol_Button) : ImGui::GetColorU32(ImGuiCol_FrameBg);

		drawList->AddRectFilled(cursorPos, ImVec2(cursorPos.x + width, cursorPos.y + height), bgColor, 4.0f); // rounded corners

		if (*isToggled) {
			float padding = height * 0.25f; // padding inside button
			ImVec2 minPos = ImVec2(cursorPos.x + padding, cursorPos.y + padding);
			ImVec2 maxPos = ImVec2(cursorPos.x + width - padding, cursorPos.y + height - padding);
			ImU32 xColor = IM_COL32(255, 255, 255, 255); // white for X

			drawList->AddLine(minPos, maxPos, xColor, 2.5f); // diagonal 1
			drawList->AddLine(ImVec2(maxPos.x, minPos.y), ImVec2(minPos.x, maxPos.y), xColor, 2.5f); // diagonal 2
		}
	}
}