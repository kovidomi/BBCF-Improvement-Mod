#include "hooks_steamApiWrapper.h"

#include "Core/interfaces.h"
#include "Core/logger.h"

bool placeHooks_steamApiWrapper()
{
	LOG(1, "placeHooks_steamApiWrapper\n");

	if (g_tempVals.ppSteamMatchmaking && !g_interfaces.pSteamMatchmakingWrapper)
	{
		g_interfaces.pSteamMatchmakingWrapper = new SteamMatchmakingWrapper(g_tempVals.ppSteamMatchmaking);
	}

	if (g_tempVals.ppSteamFriends && !g_interfaces.pSteamFriendsWrapper)
	{
		g_interfaces.pSteamFriendsWrapper = new SteamFriendsWrapper(g_tempVals.ppSteamFriends);
	}

	if (g_tempVals.ppSteamUser && !g_interfaces.pSteamUserWrapper)
	{
		g_interfaces.pSteamUserWrapper = new SteamUserWrapper(g_tempVals.ppSteamUser);
	}

	if (g_tempVals.ppSteamNetworking && !g_interfaces.pSteamNetworkingWrapper)
	{
		g_interfaces.pSteamNetworkingWrapper = new SteamNetworkingWrapper(g_tempVals.ppSteamNetworking);
	}

	if (g_tempVals.ppSteamUserStats && !g_interfaces.pSteamUserStatsWrapper)
	{
		g_interfaces.pSteamUserStatsWrapper = new SteamUserStatsWrapper(g_tempVals.ppSteamUserStats);
	}

	if (g_tempVals.ppSteamUtils && !g_interfaces.pSteamUtilsWrapper)
	{
		g_interfaces.pSteamUtilsWrapper = new SteamUtilsWrapper(g_tempVals.ppSteamUtils);
	}

	if (g_interfaces.pSteamUserStatsWrapper && g_interfaces.pSteamFriendsWrapper && !g_interfaces.pSteamApiHelper)
	{
		g_interfaces.pSteamApiHelper = new SteamApiHelper(g_interfaces.pSteamUserStatsWrapper, g_interfaces.pSteamFriendsWrapper);
	}

	/////////////////////////////////////////////////////////////////////////

	if (g_interfaces.pSteamNetworkingWrapper && g_interfaces.pSteamUserWrapper &&
		!g_interfaces.pNetworkManager)
	{
		g_interfaces.pNetworkManager = new NetworkManager(
			g_interfaces.pSteamNetworkingWrapper,
			g_interfaces.pSteamUserWrapper->GetSteamID()
		);
	}

	if (g_interfaces.pNetworkManager && g_interfaces.pSteamUserWrapper && g_interfaces.pSteamFriendsWrapper &&
		!g_interfaces.pRoomManager)
	{
		g_interfaces.pRoomManager = new RoomManager(
			g_interfaces.pNetworkManager,
			g_interfaces.pSteamFriendsWrapper,
			g_interfaces.pSteamUserWrapper->GetSteamID()
		);
	}

	if (g_interfaces.pPaletteManager && g_interfaces.pRoomManager &&
		!g_interfaces.pOnlinePaletteManager)
	{
		g_interfaces.pOnlinePaletteManager = new OnlinePaletteManager(
			g_interfaces.pPaletteManager,
			&g_interfaces.player1.GetPalHandle(),
			&g_interfaces.player2.GetPalHandle(),
			g_interfaces.pRoomManager
		);
	}

	if (!g_interfaces.pGameModeManager)
	{
		g_interfaces.pGameModeManager = new GameModeManager();
	}

	if (g_interfaces.pGameModeManager && g_interfaces.pRoomManager &&
		!g_interfaces.pOnlineGameModeManager)
	{
		g_interfaces.pOnlineGameModeManager = new OnlineGameModeManager(
			g_interfaces.pGameModeManager,
			g_interfaces.pRoomManager
		);
	}

	return true;
}