#include "MainWindow.h"

#include "HitboxOverlay.h"
#include "PaletteEditorWindow.h"

#include "Core/Settings.h"
#include "Core/info.h"
#include "Core/interfaces.h"
#include "Game/gamestates.h"
#include "Game/stages.h"
#include "Overlay/imgui_utils.h"

#include <sstream>

MainWindow::MainWindow(const std::string& windowTitle, bool windowClosable, WindowContainer& windowContainer, ImGuiWindowFlags windowFlags)
	: IWindow(windowTitle, windowClosable, windowFlags), m_pWindowContainer(&windowContainer)
{
	m_windowTitle = MOD_WINDOW_TITLE;
	m_windowTitle += " ";
	m_windowTitle += MOD_VERSION_NUM;

#ifdef _DEBUG
	m_windowTitle += " (DEBUG)";
#endif

	m_windowTitle += "###MainTitle"; // Set unique identifier
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
	ImGui::Text("Toggle Online with %s", Settings::settingsIni.toggleOnlineButton.c_str());
	ImGui::Text("Toggle HUD with %s", Settings::settingsIni.toggleHUDbutton.c_str());
	ImGui::Separator();

	ImGui::TextUnformatted(" ");

	ImGui::Text("P$"); ImGui::SameLine();
	if (g_gameVals.pGameMoney)
	{
		ImGui::InputInt("##P$", *&g_gameVals.pGameMoney);
	}

	if (ImGui::Button("Online", BTN_SIZE))
	{
		m_pWindowContainer->GetWindow(WindowType_Room)->ToggleOpen();
	}

	DrawGameplaySettingSection();
	DrawCustomPalettesSection();
	DrawHitboxOverlaySection();
	DrawAvatarSection();
	DrawLoadedSettingsValuesSection();
	DrawUtilButtons();
	DrawCurrentPlayersCount();
	DrawLinkButtons();
}

void MainWindow::DrawUtilButtons() const
{
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
}

void MainWindow::DrawCurrentPlayersCount() const
{
	ImGui::Text("Current online players:");
	ImGui::SameLine();

	std::string currentPlayersCount = g_interfaces.pSteamApiHelper ? g_interfaces.pSteamApiHelper->GetCurrentPlayersCountString() : "<No data>";
	ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "%s", currentPlayersCount.c_str());
}

void MainWindow::DrawAvatarSection() const
{
	if (!ImGui::CollapsingHeader("Avatar settings"))
		return;

	ImGui::TextUnformatted(" "); ImGui::SameLine(); ImGui::SliderInt("Avatar", g_gameVals.playerAvatarAddr, 0, 0x2F);
	ImGui::TextUnformatted(" "); ImGui::SameLine(); ImGui::SliderInt("Color", g_gameVals.playerAvatarColAddr, 0, 0x3);
	ImGui::TextUnformatted(" "); ImGui::SameLine(); ImGui::SliderByte("Accessory 1", g_gameVals.playerAvatarAcc1, 0, 0xCF);
	ImGui::TextUnformatted(" "); ImGui::SameLine(); ImGui::SliderByte("Accessory 2", g_gameVals.playerAvatarAcc2, 0, 0xCF);
	ImGui::Separator();
}

void MainWindow::DrawCustomPalettesSection() const
{
	if (!ImGui::CollapsingHeader("Custom palettes"))
		return;

	if (*g_gameVals.pGameState != GameState_InMatch)
	{
		ImGui::TextUnformatted(" "); ImGui::SameLine();
		ImGui::TextDisabled("Not in match!");
	}
	else
	{
		m_pWindowContainer->GetWindow<PaletteEditorWindow>(WindowType_PaletteEditor)->ShowAllPaletteSelections();
	}

	ImGui::TextUnformatted(""); ImGui::TextUnformatted(" "); ImGui::SameLine();
	m_pWindowContainer->GetWindow<PaletteEditorWindow>(WindowType_PaletteEditor)->ShowReloadAllPalettesButton();

	ImGui::TextUnformatted(" "); ImGui::SameLine();
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

void MainWindow::DrawHitboxOverlaySection() const
{
	if (!ImGui::CollapsingHeader("Hitbox overlay"))
		return;

	if (*g_gameVals.pGameState != GameState_InMatch)
	{
		ImGui::TextUnformatted(" "); ImGui::SameLine();
		ImGui::TextDisabled("Not in match!");
		return;
	}

	if (!isHitboxOverlayEnabledInCurrentGameMode())
	{
		ImGui::TextUnformatted(" "); ImGui::SameLine();
		ImGui::TextDisabled("Not in training or versus modes!");
		return;
	}

	ImGui::TextUnformatted(" "); ImGui::SameLine();
	static bool isOpen = false;
	if (ImGui::Checkbox("Enable", &isOpen))
	{
		if (isOpen)
		{
			m_pWindowContainer->GetWindow(WindowType_HitboxOverlay)->Open();
		}
		else
		{
			g_gameVals.isFrameFrozen = false;
			m_pWindowContainer->GetWindow(WindowType_HitboxOverlay)->Close();
		}
	}

	if (isOpen)
	{
		ImGui::TextUnformatted(" ");

		if (!g_interfaces.player1.IsCharDataNullPtr() && !g_interfaces.player2.IsCharDataNullPtr())
		{
			ImGui::TextUnformatted(" "); ImGui::SameLine();
			ImGui::Checkbox("Player1", &m_pWindowContainer->GetWindow<HitboxOverlay>(WindowType_HitboxOverlay)->drawCharacterHitbox[0]);
			HoverTooltip(getCharacterNameByIndexA(g_interfaces.player1.GetData()->charIndex).c_str());
			ImGui::SameLine(); ImGui::TextUnformatted(" "); ImGui::SameLine();
			ImGui::TextUnformatted(g_interfaces.player1.GetData()->currentAction);

			ImGui::TextUnformatted(" "); ImGui::SameLine();
			ImGui::Checkbox("Player2", &m_pWindowContainer->GetWindow<HitboxOverlay>(WindowType_HitboxOverlay)->drawCharacterHitbox[1]);
			HoverTooltip(getCharacterNameByIndexA(g_interfaces.player2.GetData()->charIndex).c_str());
			ImGui::SameLine(); ImGui::TextUnformatted(" "); ImGui::SameLine();
			ImGui::TextUnformatted(g_interfaces.player2.GetData()->currentAction);
		}

		ImGui::TextUnformatted(" "); ImGui::SameLine();
		m_pWindowContainer->GetWindow<HitboxOverlay>(WindowType_HitboxOverlay)->DrawRectThicknessSlider();

		ImGui::TextUnformatted(" "); ImGui::SameLine();
		m_pWindowContainer->GetWindow<HitboxOverlay>(WindowType_HitboxOverlay)->DrawRectFillTransparencySlider();

		ImGui::TextUnformatted(" "); ImGui::SameLine();
		ImGui::Checkbox("Draw origin",
			&m_pWindowContainer->GetWindow<HitboxOverlay>(WindowType_HitboxOverlay)->drawOriginLine);

		ImGui::TextUnformatted(" ");

		ImGui::TextUnformatted(" "); ImGui::SameLine();
		ImGui::Checkbox("Freeze frame:", &g_gameVals.isFrameFrozen);
		if (g_gameVals.pFrameCount)
		{
			ImGui::SameLine();
			ImGui::Text("%d", *g_gameVals.pFrameCount);
			ImGui::SameLine();
			if (ImGui::Button("Reset"))
			{
				*g_gameVals.pFrameCount = 0;
				g_gameVals.framesToReach = 0;
			}
		}

		if (g_gameVals.isFrameFrozen)
		{
			static int framesToStep = 1;
			ImGui::TextUnformatted(" "); ImGui::SameLine();
			if (ImGui::Button("Step frames"))
			{
				g_gameVals.framesToReach = *g_gameVals.pFrameCount + framesToStep;
			}

			ImGui::SameLine();
			ImGui::SliderInt("", &framesToStep, 1, 60);
		}
	}
}

void MainWindow::DrawGameplaySettingSection() const
{
	if (!ImGui::CollapsingHeader("Gameplay settings"))
		return;

	ImGui::TextUnformatted(" "); ImGui::SameLine();

	if (isStageSelectorEnabledInCurrentGameMode() &&
		g_gameVals.stageSelect_X != nullptr &&
		g_gameVals.stageSelect_Y != nullptr)
	{

		static unsigned char selectedStage = 0;

		// Sync slider with the selected stage
		for (int i = 0; i < STAGES_COUNT; i++)
		{
			if (stages[i][0] == *g_gameVals.stageSelect_X &&
				stages[i][1] == *g_gameVals.stageSelect_Y)
			{
				selectedStage = i;
				break;
			}
		}

		if (ImGui::SliderByte("Stage", &selectedStage, 0, STAGES_COUNT - 1))
		{
			*g_gameVals.stageSelect_X = stages[selectedStage][0];
			*g_gameVals.stageSelect_Y = stages[selectedStage][1];
		}
	}
	else
	{
		ImGui::TextDisabled("Stage selection not available");
	}

	if (*g_gameVals.pGameState == GameState_InMatch)
	{
		ImGui::TextUnformatted(" "); ImGui::SameLine();
		ImGui::Checkbox("Hide HUD", (bool*)g_gameVals.pIsHUDHidden);
	}

	std::string selectedGameMode = g_interfaces.pGameModeManager->GetCurrentGameModeName();

	if (!isGameModeSelectorEnabledInCurrentGameState())
	{
		ImGui::TextUnformatted(" "); ImGui::SameLine(); ImGui::RadioButton(selectedGameMode.c_str(), true);
		ImGui::TextUnformatted(" "); ImGui::SameLine(); ImGui::TextDisabled("NOT ON CHARACTER SELECTION SCREEN");
		ImGui::TextUnformatted(" "); ImGui::SameLine(); ImGui::TextDisabled("NOT ON REPLAY MENU SCREEN");

		return;
	}

	if (!isGameModeSelectorEnabledInCurrentGameMode())
	{
		ImGui::TextUnformatted(" "); ImGui::SameLine(); ImGui::RadioButton(selectedGameMode.c_str(), true);
		ImGui::TextUnformatted(" "); ImGui::SameLine(); ImGui::TextDisabled("NOT IN ONLINE, TRAINING, OR VERSUS MODES");

		return;
	}

	for (int i = 0; i < g_interfaces.pGameModeManager->GetGameModesCount(); i++)
	{
		ImGui::TextUnformatted(" "); ImGui::SameLine();
		std::string gameModeName = g_interfaces.pGameModeManager->GetGameModeName((CustomGameMode)i);
		std::string gameModeDesc = g_interfaces.pGameModeManager->GetGameModeDesc((CustomGameMode)i);

		if (ImGui::RadioButton(gameModeName.c_str(), (int*)&g_interfaces.pGameModeManager->GetActiveGameModeRef(), i))
		{
			if (g_interfaces.pRoomManager->IsRoomFunctional())
			{
				g_interfaces.pOnlineGameModeManager->SetThisPlayerGameMode(g_interfaces.pGameModeManager->GetActiveGameMode());
				g_interfaces.pOnlineGameModeManager->SendGameModePacket();
			}
		}

		if (ImGui::IsItemHovered() && !gameModeDesc.empty())
		{
			ImGui::BeginTooltip();
			ImGui::TextUnformatted(gameModeDesc.c_str());
			ImGui::EndTooltip();
		}
	}
}

void MainWindow::DrawLinkButtons() const
{
	ButtonUrl("Discord", MOD_LINK_DISCORD, BTN_SIZE);

	ImGui::SameLine();
	ButtonUrl("Forum", MOD_LINK_FORUM, BTN_SIZE);

	ImGui::SameLine();
	ButtonUrl("GitHub", MOD_LINK_GITHUB, BTN_SIZE);
}

void MainWindow::DrawLoadedSettingsValuesSection() const
{
	if (!ImGui::CollapsingHeader("Loaded settings.ini values"))
		return;

	// Not using ImGui columns here because they are bugged if the window has always_autoresize flag. The window 
	// starts extending to infinity, if the left edge of the window touches any edges of the screen

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
