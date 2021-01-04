#include "RoomWindow.h"

#include "Core/interfaces.h"
#include "Core/utils.h"
#include "Game/gamestates.h"
#include "Overlay/WindowManager.h"
#include "Overlay/Window/PaletteEditorWindow.h"

void RoomWindow::SetWindowTitleRoomType(const std::string& roomTypeName)
{
	m_windowTitle = "Online - " + roomTypeName + "###Room";
}

void RoomWindow::BeforeDraw()
{
	ImGui::SetWindowPos(m_windowTitle.c_str(), ImVec2(200, 200), ImGuiCond_FirstUseEver);

	//ImVec2 windowSizeConstraints;
	//switch (Settings::settingsIni.menusize)
	//{
	//case 1:
	//	windowSizeConstraints = ImVec2(250, 190);
	//	break;
	//case 3:
	//	windowSizeConstraints = ImVec2(400, 230);
	//	break;
	//default:
	//	windowSizeConstraints = ImVec2(330, 230);
	//}

	ImVec2 windowSizeConstraints = ImVec2(300, 190);

	ImGui::SetNextWindowSizeConstraints(windowSizeConstraints, ImVec2(1000, 1000));
}

void RoomWindow::Draw()
{
	if (!g_gameVals.pRoom || g_gameVals.pRoom->roomStatus == RoomStatus_Unavailable)
	{
		ImGui::TextUnformatted("NOT IN ROOM OR ONLINE MATCH!");
		m_windowTitle = m_origWindowTitle;
		return;
	}

	std::string roomTypeName = g_interfaces.pRoomManager->GetRoomTypeName();
	SetWindowTitleRoomType(roomTypeName);

	ImGui::Text("Online type: %s", roomTypeName.c_str());
	ImGui::Spacing();

	ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);

	// stage selection

	DrawGameModeSelection();
	ImGui::Spacing();

	DrawPaletteSelection();
	ImGui::Spacing();

	if (*g_gameVals.pGameState == GameState_MainMenu)
	{
		DrawRoomImPlayers();
	}

	if (*g_gameVals.pGameState == GameState_CharacterSelectionScreen ||
		*g_gameVals.pGameState == GameState_InMatch)
	{
		ImGui::SameLine();
		DrawMatchImPlayers();
	}

	ImGui::PopStyleVar();
}

void RoomWindow::DrawRoomImPlayers()
{
	ImGui::BeginGroup();
	ImGui::TextUnformatted("Room IM users");
	ImGui::BeginChild("RoomImUsers", ImVec2(200, 150), true);

	for (const IMPlayer& imPlayer : g_interfaces.pRoomManager->GetIMPlayersInCurrentRoom())
	{
		ImGui::TextUnformatted(imPlayer.steamName.c_str());
	}

	ImGui::EndChild();
	ImGui::EndGroup();
}

void RoomWindow::DrawMatchImPlayers()
{
	ImGui::BeginGroup();
	ImGui::TextUnformatted("Current match IM users");
	ImGui::BeginChild("MatchImUsers", ImVec2(200, 150), true);

	if (g_interfaces.pRoomManager->IsThisPlayerInMatch())
	{
		ImGui::Columns(2);
		for (const IMPlayer& player : g_interfaces.pRoomManager->GetIMPlayersInCurrentMatch())
		{
			uint16_t matchPlayerIndex = g_interfaces.pRoomManager->GetPlayerMatchPlayerIndexByRoomMemberIndex(player.roomMemberIndex);
			std::string playerType;

			if (matchPlayerIndex == 0)
				playerType = "Player 1";
			else if (matchPlayerIndex == 1)
				playerType = "Player 2";
			else
				playerType = "Spectator";

			ImGui::TextUnformatted(player.steamName.c_str());
			ImGui::NextColumn();
			ImGui::TextUnformatted(playerType.c_str());
			ImGui::NextColumn();
		}
	}
	else
	{
		ImGui::TextUnformatted("Not in match!");
	}

	ImGui::EndChild();
	ImGui::EndGroup();
}

void RoomWindow::DrawGameModeSelection()
{
	CustomGameMode currentGameMode = g_interfaces.pOnlineGameModeManager->GetSettledGameModeChoice();
	const std::string activeGameMode = g_interfaces.pGameModeManager->GetGameModeName(currentGameMode);

	ImGui::Text("Active game mode: %s", activeGameMode.c_str());

	if (*g_gameVals.pGameState != GameState_CharacterSelectionScreen)
		return;

	const std::string player1SelectedGameMode = g_interfaces.pGameModeManager->GetGameModeName(
		g_interfaces.pOnlineGameModeManager->GetPlayer1GameModeChoice()
	);

	const std::string player2SelectedGameMode = g_interfaces.pGameModeManager->GetGameModeName(
		g_interfaces.pOnlineGameModeManager->GetPlayer2GameModeChoice()
	);

	// As spectator show both P1 and P2 selection
	if (g_interfaces.pRoomManager->IsThisPlayerSpectator())
	{
		ImGui::Text("Player 1 selection: %s", player1SelectedGameMode.c_str());
		ImGui::Text("Player 2 selection: %s", player2SelectedGameMode.c_str());
	}
	else // As P1 or P2 only show the opponent's selection
	{
		const std::string& opponentSelectedMode = g_interfaces.pRoomManager->GetThisPlayerMatchPlayerIndex()
			? player1SelectedGameMode
			: player2SelectedGameMode;

		ImGui::Text("Opponent selection: %s", opponentSelectedMode.c_str());
	}

	// Hide game mode selection from spectators
	if (g_interfaces.pRoomManager->IsThisPlayerSpectator())
		return;

	ImGui::BeginGroup();
	ImGui::TextUnformatted("Request game mode");
	ImGui::BeginChild("RequestGameMode", ImVec2(200, 200), true);

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

	ImGui::EndChild();
	ImGui::EndGroup();
}

void RoomWindow::DrawPaletteSelection()
{
	if (*g_gameVals.pGameState != GameState_InMatch)
		return;

	WindowManager::GetInstance().GetWindowContainer()->
		GetWindow<PaletteEditorWindow>(WindowType_PaletteEditor)->ShowAllPaletteSelections();
}
