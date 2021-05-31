#pragma once
#include "imgui.h"


namespace ImGui
{
	static void StyleColorsPKDark()
	{
		ImGuiStyle& style = ImGui::GetStyle();
		ImGuiIO& io = ImGui::GetIO();
		io.Fonts->AddFontFromFileTTF("assets/fonts/Trueno.otf", 13);
		style.ChildRounding = 2.0f;
		style.TabRounding = 2.0f;
		style.GrabRounding = 2.0f;
		style.WindowRounding = 2.0f;
		style.ScrollbarRounding = 3.0f;
		style.FrameRounding = 2.0f;
		style.PopupRounding = 2.0f;
		//style.WindowTitleAlign = ImVec2(0.5f, 0.5f);

		style.Colors[ImGuiCol_WindowBg] = ImVec4(0.18f, 0.18f, 0.18f, 1.0f);

		style.Colors[ImGuiCol_DragDropTarget] = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);

		style.Colors[ImGuiCol_CheckMark] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

		style.Colors[ImGuiCol_Button] = ImVec4(0.875f, 0.475f, 0.176f, 1.0f);
		style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.914f, 0.639f, 0.435f, 1.0f);
		style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.886f, 0.529f, 0.263f, 1.0f);

		style.Colors[ImGuiCol_Header] = ImVec4(0.875f, 0.475f, 0.176f, 1.0f);
		style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.9f, 0.55f, 0.35f, 1.0f);
		style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.886f, 0.529f, 0.263f, 1.0f);

		//Tabs
		style.Colors[ImGuiCol_Tab] = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
		style.Colors[ImGuiCol_TabHovered] = ImVec4(0.45f, 0.45f, 0.45f, 1.0f);
		style.Colors[ImGuiCol_TabActive] = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
		style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
		style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);

		//Title Bars
		style.Colors[ImGuiCol_TitleBg] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
		style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
		style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);

		style.Colors[ImGuiCol_FrameBg] = ImVec4(0.875f, 0.475f, 0.176f, 1.0f);
		style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.9f, 0.55f, 0.35f, 1.0f);
		style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.886f, 0.529f, 0.263f, 1.0f);
	}

	static void StyleColorsPKLight()
	{
		ImGuiStyle& style = ImGui::GetStyle();

		/*style.ChildRounding = 0.f;
		style.GrabRounding = 0.f;
		style.WindowRounding = 10.f;
		style.ScrollbarRounding = 3.f;
		style.FrameRounding = 0.f;
		//style.WindowTitleAlign = ImVec2(0.5f, 0.5f);

		style.Colors[ImGuiCol_Text] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);

		style.Colors[ImGuiCol_WindowBg] = ImVec4(0.55f, 0.55f, 0.58f, 1.0f);

		//Tabs
		style.Colors[ImGuiCol_Tab] = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
		style.Colors[ImGuiCol_TabHovered] = ImVec4(0.55f, 0.55f, 0.55f, 1.0f);
		style.Colors[ImGuiCol_TabActive] = ImVec4(0.6f, 0.6f, 0.6f, 1.0f);
		style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
		style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.45f, 0.45f, 0.45f, 1.0f);

		//Title Bars
		style.Colors[ImGuiCol_TitleBg] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
		style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
		style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.35f, 0.35f, 0.35f, 1.0f);*/
	}
}