#include "hooks_steamApiWrapper.h"

#include "Core/logger.h"
#include "Game/containers.h"

bool placeHooks_steamApiWrapper()
{
	LOG(1, "placeHooks_steamApiWrapper\n");
	
	if (Containers::tempVals.ppSteamMatchmaking)
	{
		Containers::g_interfaces.pSteamMatchmakingWrapper = new SteamMatchmakingWrapper(Containers::tempVals.ppSteamMatchmaking);
	}

	if (Containers::tempVals.ppSteamNetworking)
	{
		Containers::g_interfaces.pSteamNetworkingWrapper = new SteamNetworkingWrapper(Containers::tempVals.ppSteamNetworking);
		Containers::g_interfaces.pNetworkManager = new NetworkManager(Containers::g_interfaces.pSteamNetworkingWrapper);
	}

	if (Containers::tempVals.ppSteamFriends)
	{
		Containers::g_interfaces.pSteamFriendsWrapper = new SteamFriendsWrapper(Containers::tempVals.ppSteamFriends);
	}

	if (Containers::tempVals.ppSteamUser)
	{
		Containers::g_interfaces.pSteamUserWrapper = new SteamUserWrapper(Containers::tempVals.ppSteamUser);
	}

	if (Containers::tempVals.ppSteamUserStats)
	{
		Containers::g_interfaces.pSteamUserStatsWrapper = new SteamUserStatsWrapper(Containers::tempVals.ppSteamUserStats);
	}

	if (Containers::tempVals.ppSteamUtils)
	{
		Containers::g_interfaces.pSteamUtilsWrapper = new SteamUtilsWrapper(Containers::tempVals.ppSteamUtils);
	}

	return true;
}