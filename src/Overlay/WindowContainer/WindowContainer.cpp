#include "WindowContainer.h"

#include "Overlay/Window/DebugWindow.h"
#include "Overlay/Window/HitboxOverlay.h"
#include "Overlay/Window/LogWindow.h"
#include "Overlay/Window/MainWindow.h"
#include "Overlay/Window/PaletteEditorWindow.h"
#include "Overlay/Window/UpdateNotifierWindow.h"

#include "Core/info.h"
#include "Core/logger.h"
#include "Core/Settings.h"

WindowContainer::WindowContainer()
{
	MainWindow* pMainWindow = new MainWindow(MOD_WINDOW_TITLE, false, ImGuiWindowFlags_AlwaysAutoResize);
	pMainWindow->SetWindowContainer(*this);
	AddWindow(WindowType_Main, pMainWindow);

	AddWindow(WindowType_Log,
		new LogWindow("Log", true, *(ImGuiLogger*)g_imGuiLogger,
			ImGuiWindowFlags_NoCollapse));

	AddWindow(WindowType_Debug,
		new DebugWindow("DEBUG", true));

	AddWindow(WindowType_UpdateNotifier,
		new UpdateNotifierWindow("Update available", true,
			ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse));

	AddWindow(WindowType_PaletteEditor,
		new PaletteEditorWindow("Palette Editor", true));

	//AddWindow(WindowType_HitboxOverlay,
	//	new HitboxOverlay("##HitboxOverlay", false, ImGuiWindowFlags_NoCollapse));
}
