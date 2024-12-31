#include "gui.h"
#include "../../ext/imgui/imgui.h"

namespace GUI {
	void ApplyStyle() {
		ImGui::StyleColorsDark();
		ImGuiStyle& style = ImGui::GetStyle();

		//style.WindowMinSize = ImVec2(32.0f, 32.0f); // crashes
		style.WindowMenuButtonPosition = ImGuiDir_Left;
		style.WindowTitleAlign = ImVec2(0.0f, 0.5f);

		style.WindowRounding = 5.0f;
		style.FrameRounding = 4.0f;
		style.PopupRounding = 4.0f;
		style.ScrollbarRounding = 4.0f;
		style.GrabRounding = 4.0f;
		style.GrabMinSize = 12.0f;
	}
}