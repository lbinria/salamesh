#pragma once 
// This class aims to restore the stack state of ImGui in case of error between a Begin / End
// We need to restore the stack if we don't want the app crashes on simple error that occurs when GUI is drawed
#include "imgui.h"
#include "imgui_internal.h"

struct ImGuiStackState {

	ImGuiStackState(ImGuiContext *ctx) : ctx(ctx) {
		windowStackSize = ctx->CurrentWindowStack.size();
		groupStackSize = ctx->GroupStack.size();
		treeNodeStackSize = ctx->TreeNodeStack.size();
		itemFlagsStackSize = ctx->ItemFlagsStack.size();
		openPopupStackSize = ctx->OpenPopupStack.size();
		beginPopupStackSize = ctx->BeginPopupStack.size();
		currentTabBarStackSize = ctx->CurrentTabBarStack.size();
		colorStackSize = ctx->ColorStack.size();
		styleVarStackSize = ctx->StyleVarStack.size();
		fontStackSize = ctx->FontStack.size();
		disabledStackSize = ctx->DisabledStackSize;
	}

	void restore() {
		// Restore all stacks to their previous state
		// Trying to pop in reverse order of typical nesting/dependency
		while (ctx->FontStack.size() > fontStackSize) {
			ImGui::PopFont();
		}

		while (ctx->StyleVarStack.size() > styleVarStackSize) {
			ImGui::PopStyleVar();
		}

		while (ctx->ColorStack.size() > colorStackSize) {
			ImGui::PopStyleColor();
		}

		while (ctx->DisabledStackSize > disabledStackSize) {
			ImGui::EndDisabled();
		}

		while (ctx->CurrentTabBarStack.size() > currentTabBarStackSize) {
			ImGui::EndTabBar();
		}

		while (ctx->BeginPopupStack.size() > beginPopupStackSize) {
			ctx->BeginPopupStack.pop_back();
		}

		while (ctx->OpenPopupStack.size() > openPopupStackSize) {
			ctx->OpenPopupStack.pop_back();
		}

		while (ctx->ItemFlagsStack.size() > itemFlagsStackSize) {
			ctx->ItemFlagsStack.pop_back();
		}

		while (ctx->TreeNodeStack.size() > treeNodeStackSize) {
			ImGui::TreePop();
		}

		while (ctx->GroupStack.size() > groupStackSize) {
			ImGui::EndGroup();
		}

		while (ctx->CurrentWindowStack.size() > windowStackSize) {
			ImGui::End();
		}
	}

	int 
		windowStackSize,
		groupStackSize,
		treeNodeStackSize,
		itemFlagsStackSize,
		openPopupStackSize,
		beginPopupStackSize,
		currentTabBarStackSize,
		colorStackSize,
		styleVarStackSize,
		fontStackSize,
		disabledStackSize;

	private:
	ImGuiContext *ctx;
};