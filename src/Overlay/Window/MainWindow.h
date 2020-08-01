#pragma once
#include "IWindow.h"

#include "Overlay/WindowContainer/WindowContainer.h"

class MainWindow : public IWindow
{
public:
	MainWindow(const std::string& windowTitle, bool windowClosable,
		ImGuiWindowFlags windowFlags = 0)
		: IWindow(windowTitle, windowClosable, windowFlags)
	{
		SetMainWindowTitle();
	}
	~MainWindow() override = default;
	void SetMainWindowTitle(const std::string text = "");
	void SetWindowContainer(WindowContainer& windowContainer);
protected:
	void BeforeDraw() override;
	void Draw() override;
private:
	void DrawLinkButtons() const;
	void DrawLoadedSettingsValues() const;
	void DrawHudSection() const;
	void DrawCustomPalettesSection() const;
	void DrawHitboxOverlaySection() const;

	const ImVec2 BTN_SIZE = ImVec2(60, 20);
	WindowContainer* m_pWindowContainer = nullptr;
};
