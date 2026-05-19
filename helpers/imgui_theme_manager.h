#pragma once 

#include "imgui.h"
#include "imgui_internal.h"

struct ImGuiThemeManager {

	struct ImGuiTheme {
		ImVec4 Accent;       // Primary highlight
		ImVec4 Hover;        // Brightened accent
		ImVec4 Active;       // Deep/Pressed accent
		ImVec4 Muted;        // Low-intensity background/inactive
		ImVec4 LowAlpha;     // Translucent selection/glow
		ImVec4 WindowBg;     // Main dark background
		ImVec4 Border;       // Subtle edge contrast
	};

	inline static const ImGuiTheme nordicSkyTheme{ 
		{0.60f, 0.75f, 0.85f, 1.00f}, // Accent (Base)
		{0.70f, 0.85f, 0.95f, 1.00f}, // Hover (Brighter)
		{0.40f, 0.55f, 0.65f, 1.00f}, // Active (Darkened/Deepened) - FIXED
		{0.22f, 0.30f, 0.38f, 0.70f}, // Muted
		{0.60f, 0.75f, 0.85f, 0.15f}, // LowAlpha
		{0.10f, 0.11f, 0.13f, 1.00f}, // WindowBg
		{0.20f, 0.25f, 0.30f, 0.50f}  // Border
	};

	static void ApplyTheme(const ImGuiTheme& theme) {
		ImGuiStyle& style = ImGui::GetStyle();
		
		// --- Layout Settings ---
		style.WindowRounding    = 8.0f;     
		style.FrameRounding     = 6.0f;      
		style.GrabRounding      = 6.0f;       
		style.PopupRounding     = 4.0f;      
		style.TabRounding       = 4.0f;
		style.WindowBorderSize  = 1.0f;
		style.FrameBorderSize   = 1.0f;
		style.WindowPadding     = ImVec2(15, 15);
		

		ImVec4* colors = style.Colors;
		
		// --- Backgrounds & Text ---
		colors[ImGuiCol_WindowBg]             = theme.WindowBg;
		colors[ImGuiCol_PopupBg]              = ImVec4(theme.WindowBg.x + 0.02f, theme.WindowBg.y + 0.02f, theme.WindowBg.z + 0.03f, 0.98f);
		colors[ImGuiCol_Border]               = theme.Border;
		colors[ImGuiCol_Text]                 = ImVec4(0.95f, 0.95f, 0.96f, 1.00f); // Soft white
		colors[ImGuiCol_TextDisabled]         = ImVec4(0.50f, 0.50f, 0.55f, 1.00f);

		// --- Interaction & Headers ---
		colors[ImGuiCol_Header]               = theme.LowAlpha;
		colors[ImGuiCol_HeaderHovered]        = theme.Muted;
		colors[ImGuiCol_HeaderActive]         = theme.Accent;

		colors[ImGuiCol_Button]               = theme.Muted;
		colors[ImGuiCol_ButtonHovered]        = theme.Accent;
		colors[ImGuiCol_ButtonActive]         = theme.Active;

		colors[ImGuiCol_FrameBg]              = ImVec4(0.16f, 0.16f, 0.18f, 1.00f);
		colors[ImGuiCol_FrameBgHovered]       = theme.LowAlpha;
		colors[ImGuiCol_FrameBgActive]        = theme.Muted;

		// --- Tabs & Widgets ---
		colors[ImGuiCol_Tab]                  = ImVec4(theme.WindowBg.x + 0.03f, theme.WindowBg.y + 0.03f, theme.WindowBg.z + 0.04f, 1.00f);
		colors[ImGuiCol_TabHovered]           = theme.Muted;
		colors[ImGuiCol_TabActive]            = theme.Active;
		colors[ImGuiCol_TabUnfocusedActive]   = theme.Muted;

		colors[ImGuiCol_CheckMark]            = theme.Accent;
		colors[ImGuiCol_SliderGrab]           = theme.Accent;
		colors[ImGuiCol_SliderGrabActive]     = theme.Hover;

		// --- Title Bar ---
		colors[ImGuiCol_TitleBg]              = colors[ImGuiCol_Tab];
		colors[ImGuiCol_TitleBgActive]        = theme.Muted;
		colors[ImGuiCol_TitleBgCollapsed]     = ImVec4(0.00f, 0.00f, 0.00f, 0.50f);

		// --- Misc ---
		colors[ImGuiCol_Separator]            = theme.Border;
		colors[ImGuiCol_SeparatorHovered]     = theme.Accent;
		colors[ImGuiCol_TextSelectedBg]       = theme.LowAlpha;
		colors[ImGuiCol_NavHighlight]         = theme.Accent;

		// --- Docking & Resizing (Fixing the Blue) ---
		colors[ImGuiCol_DockingPreview]          = theme.LowAlpha; // The "ghost" window when dragging
		colors[ImGuiCol_DockingEmptyBg]          = theme.WindowBg; // Background when no windows are docked

		// The resize grip is the little triangle in the bottom-right corner
		colors[ImGuiCol_ResizeGrip]              = theme.Muted; 
		colors[ImGuiCol_ResizeGripHovered]       = theme.Accent;
		colors[ImGuiCol_ResizeGripActive]        = theme.Active;

		// Docking tabs and separators
		colors[ImGuiCol_Separator]               = theme.Border;
		colors[ImGuiCol_SeparatorHovered]        = theme.Accent;
		colors[ImGuiCol_SeparatorActive]         = theme.Active;
	}

};

