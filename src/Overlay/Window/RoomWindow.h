#pragma once
#include "IWindow.h"

class RoomWindow : public IWindow
{
public:
	RoomWindow(const std::string& windowTitle, bool windowClosable,
		ImGuiWindowFlags windowFlags = 0)
		: IWindow(windowTitle, windowClosable, windowFlags), m_origWindowTitle(windowTitle)
	{
	}

	~RoomWindow() override = default;

protected:
	void BeforeDraw() override;
	void Draw() override;

private:
	void SetWindowTitleRoomType(const std::string& roomTypeName);

	void DrawRoomImPlayers();
	void DrawMatchImPlayers();
	void DrawGameModeSelection();
	void DrawPaletteSelection();

	const std::string m_origWindowTitle;
};
