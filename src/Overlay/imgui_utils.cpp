#include "imgui_utils.h"

#include "imgui_internal.h"

#include <windows.h>

void HoverTooltip(const char* text)
{
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::TextUnformatted(text);
		ImGui::EndTooltip();
	}
}

bool ButtonUrl(const std::string& label, const wchar_t* url, const ImVec2& btnSize)
{
	if (ImGui::Button(label.c_str(), btnSize))
	{
		ShellExecute(nullptr, L"open", url, nullptr, nullptr, SW_SHOWNORMAL);
		return true;
	}

	return false;
}

void TextAlignedHorizontalCenter(const char* text, ...)
{
	const ImVec4 COLOR_TRANSPARENT(0.000f, 0.000f, 0.000f, 0.000f);

	va_list args;
	va_start(args, text);

	ImGui::TextColoredV(COLOR_TRANSPARENT, text, args);
	float width = ImGui::GetItemRectSize().x;
	float height = ImGui::GetItemRectSize().y;
	AlignItemHorizontalCenter(width);
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() - height);

	ImGui::TextV(text, args);
	va_end(args);
}

void TextColoredAlignedHorizontalCenter(const ImVec4 color, const char* text)
{
	const ImVec4 COLOR_TRANSPARENT(0.000f, 0.000f, 0.000f, 0.000f);
	ImGui::TextColored(COLOR_TRANSPARENT, text);

	float width = ImGui::GetItemRectSize().x;
	float height = ImGui::GetItemRectSize().y;
	AlignItemHorizontalCenter(width);
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() - height);

	ImGui::TextColored(color, text);
}

void AlignItemHorizontalCenter(float itemWidth)
{
	ImGui::SetCursorPosX(ImGui::GetWindowSize().x / 2 - (itemWidth / 2));
}

void VerticalSpacing(float height)
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems)
		return;

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
	ImGui::ItemSize(ImVec2(0, height));
	ImGui::PopStyleVar();
}