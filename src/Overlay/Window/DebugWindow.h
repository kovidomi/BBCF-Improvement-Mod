#pragma once
#include "IWindow.h"

class DebugWindow : public IWindow
{
public:
	DebugWindow(const std::string& windowTitle, bool windowClosable,
		ImGuiWindowFlags windowFlags = 0)
		: IWindow(windowTitle, windowClosable, windowFlags) {}
	~DebugWindow() override = default;
protected:
	void Draw() override;
};
