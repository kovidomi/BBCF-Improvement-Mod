#include "GameModeSelectWidget.h"

#include "Core/interfaces.h"

#include <imgui.h>

void GameModeSelectWidget()
{
	ImGui::BeginGroup();

	for (int i = 0; i < g_interfaces.pGameModeManager->GetGameModesCount(); i++)
	{
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

	ImGui::EndGroup();
}
