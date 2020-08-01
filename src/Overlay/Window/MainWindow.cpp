#include "MainWindow.h"

#include "HitboxOverlay.h"
#include "PaletteEditorWindow.h"

#include "Core/Settings.h"
#include "Core/info.h"
#include "Core/interfaces.h"
#include "Game/gamestates.h"
#include "Overlay/imgui_utils.h"

#include <sstream>

void MainWindow::SetMainWindowTitle(const std::string text)
{
	if (text != "")
	{
		m_windowTitle = text;
	}
	else
	{
		m_windowTitle = MOD_WINDOW_TITLE;
		m_windowTitle += " ";
		m_windowTitle += MOD_VERSION_NUM;
#ifdef _DEBUG
		m_windowTitle += " (DEBUG)";
#endif
	}
	m_windowTitle += "###MainTitle"; //set unique identifier
}

void MainWindow::SetWindowContainer(WindowContainer & windowContainer)
{
	m_pWindowContainer = &windowContainer;
}


void MainWindow::BeforeDraw()
{
	ImGui::SetWindowPos(m_windowTitle.c_str(), ImVec2(12, 20), ImGuiCond_FirstUseEver);

	ImVec2 windowSizeConstraints;
	switch (Settings::settingsIni.menusize)
	{
		case 1:
			windowSizeConstraints = ImVec2(250, 190);
			break;
		case 3:
			windowSizeConstraints = ImVec2(400, 230);
			break;
		default:
			windowSizeConstraints = ImVec2(330, 230);
	}

	ImGui::SetNextWindowSizeConstraints(windowSizeConstraints, ImVec2(1000, 1000));
}

void MainWindow::Draw()
{
	ImGui::Text("Toggle me with %s", Settings::settingsIni.togglebutton.c_str());
	ImGui::Text("Toggle HUD with %s", Settings::settingsIni.toggleHUDbutton.c_str());
	ImGui::Separator();

	ImGui::Text("");

	DrawHudSection();

	DrawCustomPalettesSection();

	DrawHitboxOverlaySection();

	if (ImGui::CollapsingHeader("Loaded settings.ini values"))
	{
		DrawLoadedSettingsValues();
	}

#ifdef _DEBUG
	if (ImGui::Button("DEBUG", BTN_SIZE))
	{
		m_pWindowContainer->GetWindow(WindowType_Debug)->ToggleOpen();
	}
#endif
	if (ImGui::Button("Log", BTN_SIZE))
	{
		m_pWindowContainer->GetWindow(WindowType_Log)->ToggleOpen();
	}

	ImGui::Text("Current online players:"); ImGui::SameLine();
	if (g_interfaces.pSteamApiHelper)
	{
		ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "%s",
			g_interfaces.pSteamApiHelper->current_players <= 0 ? "<No data>" : std::to_string(g_interfaces.pSteamApiHelper->current_players).c_str());
	}
	else
	{
		ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "%s", "<No data>");
	}

	DrawLinkButtons();
}

void MainWindow::DrawHudSection() const
{
	ImGui::Text(" "); ImGui::SameLine();
	ImGui::Checkbox("Show HUD", (bool*)g_gameVals.pIsHUDHidden);
}

void MainWindow::DrawCustomPalettesSection() const
{
	if (ImGui::CollapsingHeader("Custom palettes"))
	{
		if (*g_gameVals.pGameState != GameState_InMatch)
		{
			ImGui::Text(" "); ImGui::SameLine();
			ImGui::TextDisabled("Not in match!");
		}
		else
		{
			m_pWindowContainer->GetWindow<PaletteEditorWindow>(WindowType_PaletteEditor)->ShowAllPaletteSelections();
		}

		ImGui::Text(""); ImGui::Text(" "); ImGui::SameLine();
		m_pWindowContainer->GetWindow<PaletteEditorWindow>(WindowType_PaletteEditor)->ShowReloadAllPalettesButton();

		ImGui::Text(" "); ImGui::SameLine();
		bool pressed = ImGui::Button("Palette editor");

		if (!isPaletteEditingEnabledInCurrentGameMode())
		{
			ImGui::SameLine(); ImGui::TextDisabled("Not in training or versus modes!");
		}
		else if (isPaletteEditingEnabledInCurrentGameMode() && pressed)
		{
			m_pWindowContainer->GetWindow(WindowType_PaletteEditor)->ToggleOpen();
		}
	}
}

void MainWindow::DrawHitboxOverlaySection() const
{
	//if (ImGui::CollapsingHeader("Hitbox overlay"))
	//{
	//	if (*g_gameVals.pGameState != GameState_InMatch)
	//	{
	//		ImGui::TextUnformatted(" "); ImGui::SameLine();
	//		ImGui::TextDisabled("Not in match!");
	//		return;
	//	}

	//	if (!isHitboxOverlayEnabledInCurrentGameMode())
	//	{
	//		ImGui::TextUnformatted(" "); ImGui::SameLine();
	//		ImGui::TextDisabled("Not in training mode!");
	//		return;
	//	}

	//	ImGui::TextUnformatted(" "); ImGui::SameLine();
	//	static bool isOpen = false;
	//	if (ImGui::Checkbox("Enable", &isOpen))
	//	{
	//		if (isOpen)
	//		{
	//			m_pWindowContainer->GetWindow(WindowType_HitboxOverlay)->Open();
	//		}
	//		else
	//		{
	//			g_gameVals.isFrameFrozen = false;
	//			m_pWindowContainer->GetWindow(WindowType_HitboxOverlay)->Close();
	//		}
	//	}

	//	if (isOpen)
	//	{
	//		ImGui::TextUnformatted(" ");

	//		if (g_interfaces.player1.GetData()
	//			&& g_interfaces.player2.GetData())
	//		{
	//			ImGui::TextUnformatted(" "); ImGui::SameLine();
	//			ImGui::Checkbox("Player1", &m_pWindowContainer->GetWindow<HitboxOverlay>(WindowType_HitboxOverlay)->drawCharacterHitbox[0]);
	//			HoverTooltip(getCharacterNameByIndexA(g_interfaces.player1.GetData()->charIndex).c_str());

	//			ImGui::SameLine(); ImGui::TextUnformatted(" "); ImGui::SameLine();
	//			ImGui::Checkbox("Player2", &m_pWindowContainer->GetWindow<HitboxOverlay>(WindowType_HitboxOverlay)->drawCharacterHitbox[3]);
	//			HoverTooltip(getCharacterNameByIndexA(g_interfaces.player2.GetData()->charIndex).c_str());
	//		}

	//		ImGui::TextUnformatted(" "); ImGui::SameLine();
	//		m_pWindowContainer->GetWindow<HitboxOverlay>(WindowType_HitboxOverlay)->DrawRectThicknessSlider();

	//		ImGui::TextUnformatted(" "); ImGui::SameLine();
	//		m_pWindowContainer->GetWindow<HitboxOverlay>(WindowType_HitboxOverlay)->DrawRectFillTransparencySlider();

	//		ImGui::TextUnformatted(" "); ImGui::SameLine();
	//		ImGui::Checkbox("Draw origin",
	//			&m_pWindowContainer->GetWindow<HitboxOverlay>(WindowType_HitboxOverlay)->drawOriginLine);

	//		ImGui::TextUnformatted(" ");

	//		ImGui::TextUnformatted(" "); ImGui::SameLine();
	//		ImGui::Checkbox("Freeze frame:", &g_gameVals.isFrameFrozen);
	//		if (g_gameVals.pFrameCount)
	//		{
	//			ImGui::SameLine();
	//			ImGui::Text("%d", *g_gameVals.pFrameCount);
	//			ImGui::SameLine();
	//			if (ImGui::Button("Reset"))
	//			{
	//				*g_gameVals.pFrameCount = 0;
	//				g_gameVals.framesToReach = 0;
	//			}
	//		}

	//		if (g_gameVals.isFrameFrozen)
	//		{
	//			static int framesToStep = 1;
	//			ImGui::TextUnformatted(" "); ImGui::SameLine();
	//			if (ImGui::Button("Step frames"))
	//			{
	//				g_gameVals.framesToReach = *g_gameVals.pFrameCount + framesToStep;
	//			}

	//			ImGui::SameLine();
	//			ImGui::SliderInt("", &framesToStep, 1, 60);
	//		}
	//	}
	//}
}

void MainWindow::DrawLinkButtons() const
{
	ButtonUrl("Discord", MOD_LINK_DISCORD, BTN_SIZE);

	ImGui::SameLine();
	ButtonUrl("Forum", MOD_LINK_FORUM, BTN_SIZE);

	ImGui::SameLine();
	ButtonUrl("GitHub", MOD_LINK_GITHUB, BTN_SIZE);
}

void MainWindow::DrawLoadedSettingsValues() const
{
	//not using ImGui columns here because they are bugged if the window has always_autoresize flag. The window 
	//starts extending to infinity, if the left edge of the window touches any edges of the screen

	std::ostringstream oss;

	ImGui::BeginChild("loaded_settings", ImVec2(0, 300.0f), true, ImGuiWindowFlags_HorizontalScrollbar);

	//X-Macro
#define SETTING(_type, _var, _inistring, _defaultval) \
	oss << " " << _inistring; \
	ImGui::TextUnformatted(oss.str().c_str()); ImGui::SameLine(ImGui::GetWindowWidth() * 0.6f); \
	oss.str(""); \
	oss << "= " << Settings::settingsIni.##_var; \
	ImGui::TextUnformatted(oss.str().c_str()); ImGui::Separator(); \
	oss.str("");
#include "Core/settings.def"
#undef SETTING

	ImGui::EndChild();
}
