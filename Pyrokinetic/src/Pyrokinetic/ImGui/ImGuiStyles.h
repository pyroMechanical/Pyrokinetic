#pragma once
#include "imgui.h"


namespace ImGui
{
	static void StyleColorsPKDark()
	{
		ImGuiStyle& style = ImGui::GetStyle();

		style.ChildRounding = 0.0f;
		style.TabRounding = 0.0f;
		style.GrabRounding = 0.0f;
		style.WindowRounding = 10.0f;
		style.ScrollbarRounding = 3.0f;
		style.FrameRounding = 0.0f;
		//style.WindowTitleAlign = ImVec2(0.5f, 0.5f);

		style.Colors[ImGuiCol_WindowBg] = ImVec4(0.25f, 0.25f, 0.25f, 1.0f);

		style.Colors[ImGuiCol_DragDropTarget] = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);

		//Tabs
		style.Colors[ImGuiCol_Tab] = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
		style.Colors[ImGuiCol_TabHovered] = ImVec4(0.55f, 0.55f, 0.55f, 1.0f);
		style.Colors[ImGuiCol_TabActive] = ImVec4(0.6f, 0.6f, 0.6f, 1.0f);
		style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
		style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.45f, 0.45f, 0.45f, 1.0f);

		//Title Bars
		style.Colors[ImGuiCol_TitleBg] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
		style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
		style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.35f, 0.35f, 0.35f, 1.0f);
	}

	static void StyleColorsPKLight()
	{
		ImGuiStyle& style = ImGui::GetStyle();

		style.ChildRounding = 0.f;
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
		style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.35f, 0.35f, 0.35f, 1.0f);
	}
}