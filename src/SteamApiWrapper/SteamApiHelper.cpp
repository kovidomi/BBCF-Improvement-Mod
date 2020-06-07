#include "SteamApiHelper.h"

#include "Core/logger.h"
#include "ImGui/ImGuiSystem.h"

//note: commented out functions are not working

SteamApiHelper::SteamApiHelper(ISteamUserStats* pSteamUserStats, ISteamFriends* pSteamFriends)
{
	LOG(2, "SteamApiHelper::SteamApiHelper, *pSteamUserStats: 0x%p, *pSteamFriends: 0x%p\n", *pSteamUserStats, *pSteamFriends);
	this->pSteamUserStats = pSteamUserStats;
	this->pSteamFriends = pSteamFriends;
	current_players = -1;
	UpdateNumberOfCurrentPlayers();
}

// Make the asynchronous request to receive the number of current players. 
void SteamApiHelper::UpdateNumberOfCurrentPlayers()
{
	SteamAPICall_t hSteamAPICall = pSteamUserStats->GetNumberOfCurrentPlayers();
	m_NumberOfCurrentPlayersCallResult.Set(hSteamAPICall, this, &SteamApiHelper::OnUpdateNumberOfCurrentPlayers);
}

// Callback for UpdateNumberOfCurrentPlayers()
void SteamApiHelper::OnUpdateNumberOfCurrentPlayers(NumberOfCurrentPlayers_t* pCallback, bool bIOFailure)
{
	if (bIOFailure || !pCallback->m_bSuccess)
	{
		ImGuiSystem::AddLog("[error] Failed to update the current number of ingame players\n");
		current_players = -1;
		return;
	}
	//printf("Number of players currently playing: %d\n", pCallback->m_cPlayers);
#ifdef ENABLE_LOGGING
	ImGuiSystem::AddLog("[debug] Updated the current number of ingame players: %d\n", pCallback->m_cPlayers);
#endif
	current_players = pCallback->m_cPlayers;
}

//void SteamApiHelper::GetRequestUserInformation(CSteamID steamID)
//{
//	SteamAPICall_t hSteamAPICall = pSteamFriends->RequestUserInformation(steamID, true);
//	m_RequestUserInformationCallResult.Set(hSteamAPICall, this, &SteamApiHelper::OnGetRequestUserInformation);
//}
//
//void SteamApiHelper::OnGetRequestUserInformation(PersonaStateChange_t *pCallback, bool bIOFailure)
//{
//	if (bIOFailure)
//	{
//		ImGuiSystem::AddLog("[system] Online match against '?????' has started\n");
//		return;
//	}
//
//	ImGuiSystem::AddLog("[system] Online match against '%s' has started\n", pSteamFriends->GetPlayerNickname(pCallback->m_ulSteamID));
//}