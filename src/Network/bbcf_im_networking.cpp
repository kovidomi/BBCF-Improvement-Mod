#include "bbcf_im_networking.h"

#include "Core/info.h"
#include "Core/interfaces.h"
#include "Core/logger.h"
#include "Core/Settings.h"
#include "Game/custom_gameModes.h"
#include "Game/gamestates.h"
#include "Overlay/Logger/ImGuiLogger.h"

int internal_version_num = MOD_VERSION_NUM_INTERNAL;

void Send_BBCFIM_ID(CSteamID opponentSteamID)
{
	LOG(2, "Send_BBCFIM_ID\n");

	EP2PSend sendtype = k_EP2PSendUnreliable;
	im_packet_t packet;

	packet.type = packetType_IMID;
	packet.playernum = g_gameVals.thisPlayerNum;
	packet.datalen = sizeof(int) + (strlen(MOD_VERSION_NUM) + 1);

	packet.part = 1;
	memcpy(&packet.data[0], &internal_version_num, sizeof(int));
	memcpy(&packet.data[4], MOD_VERSION_NUM, strlen(MOD_VERSION_NUM) + 1);

	g_interfaces.pNetworkManager->SendPacket(&opponentSteamID, &packet);

#ifdef _DEBUG
	g_imGuiLogger->Log("[debug] BBCFIM_ID packet sent to: '%s'\n", g_interfaces.pSteamFriendsWrapper->GetFriendPersonaName(opponentSteamID));
#endif
}

void Receive_BBCFIM_ID(im_packet_internal_t *packet)
{
	LOG(2, "Receive_BBCFIM_ID\n");

	if (*g_gameVals.pGameState != GameState_ChracterSelectionScreen)
	{
		LOG(2, "ERROR, RECEIVED BBCFIM ID PACKET WHILE NOT BEING ON THE CHARSELECT SCREEN\n");
		g_imGuiLogger->Log("[error] BBCFIM ID packet received outside of character selection screen\n");
		return;
	}

	char opponentVersionNum[10];
	std::string text;
	memcpy(opponentVersionNum, &packet->data[4], strlen(MOD_VERSION_NUM) + 1);

	//we are spectators
	if (g_gameVals.thisPlayerNum == 0)
	{
		if (packet->playernum == 1)
		{
			text = "Player1 ";
			memcpy(&g_gameVals.iP1BBCFIMvernum, &packet->data[0], sizeof(int));
			g_gameVals.sP1BBCFIMvernum = opponentVersionNum;
			g_gameVals.bP1UsingBBCFIM = true;
		}
		else
		{
			text = "Player2 ";
			memcpy(&g_gameVals.iP2BBCFIMvernum, &packet->data[0], sizeof(int));
			g_gameVals.sP2BBCFIMvernum = opponentVersionNum;
			g_gameVals.bP2UsingBBCFIM = true;
		}

	}
	else //As P1 / P2
	{
		text = "Opponent ";
		memcpy(&g_gameVals.iOpponentBBCFIMvernum, &packet->data[0], sizeof(int));
		g_gameVals.sOpponentBBCFIMvernum = opponentVersionNum;
		g_gameVals.bOpponentUsingBBCFIM = true;
	}

	text += "BBCFIM ";
	text += opponentVersionNum;
	text += " detected";
	// ImGuiSystem::SetNotification(text.c_str(), 10, true);
	text += '\n';
	g_imGuiLogger->Log("[system] %s", text.c_str());
}

void Send_customGameMode_request(CSteamID opponentSteamID)
{
	LOG(2, "Send_customGameMode_request\n");

	EP2PSend sendtype = k_EP2PSendUnreliable;
	im_packet_t packet;

	packet.type = packetType_gamemode;
	packet.playernum = g_gameVals.thisPlayerNum;
	packet.datalen = sizeof(int);

	packet.part = 1;
	memcpy(&packet.data[0], &activatedGameMode, sizeof(int));

	g_interfaces.pNetworkManager->SendPacket(&opponentSteamID, &packet);

#ifdef _DEBUG
	g_imGuiLogger->Log("[debug] customGameMode packet sent to: '%s'\n", g_interfaces.pSteamFriendsWrapper->GetFriendPersonaName(opponentSteamID));
#endif
}

void Receive_customGameMode_request(im_packet_internal_t *packet)
{
	LOG(2, "Receive_customGameMode_request\n");

	std::string text;

	//if spectators
	if (g_gameVals.thisPlayerNum == 0)
	{
		if (packet->playernum == 1)
		{
			memcpy(&P1_activatedGameMode, &packet->data[0], sizeof(int));
			text = "Player 1 ";
		}
		else
		{
			memcpy(&P2_activatedGameMode, &packet->data[0], sizeof(int));
			text = "Player 2 ";
		}
	}
	else
	{
		memcpy(&opponent_activatedGameMode, &packet->data[0], sizeof(int));
		text = "Opponent ";
	}

	//GameModes are different
	if (opponent_activatedGameMode != activatedGameMode || P1_activatedGameMode != P2_activatedGameMode)
	{
		text += "has set a custom game mode";
		// ImGuiSystem::SetNotification(text.c_str(), 10, true);
		text += '\n';
		g_imGuiLogger->Log("[system] %s", text.c_str());
	}
}