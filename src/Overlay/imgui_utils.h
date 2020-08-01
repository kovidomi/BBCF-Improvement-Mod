#pragma once

#include <imgui.h>
#include <string>

void HoverTooltip(const char* text);
bool ButtonUrl(const std::string& label, const wchar_t* url, const ImVec2& btnSize = ImVec2(0, 0));
void TextAlignedHorizontalCenter(const char* text, ...);
void TextColoredAlignedHorizontalCenter(const ImVec4 color, const char* text);
void AlignItemHorizontalCenter(float itemWidth);
void VerticalSpacing(float height);