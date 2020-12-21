#include "interfaces.h"

#include "logger.h"
#include "utils.h"

interfaces_t g_interfaces = {};
gameProc_t g_gameProc = {};
temps_t g_tempVals = {};
gameVals_t g_gameVals = {};

void CleanupInterfaces()
{
	LOG(1, "CleanupInterfaces\n");

	SAFE_DELETE(g_interfaces.pNetworkManager);
	SAFE_DELETE(g_interfaces.pPaletteManager);
	SAFE_DELETE(g_interfaces.pRoomManager);
	SAFE_DELETE(g_interfaces.pOnlinePaletteManager);
	SAFE_DELETE(g_interfaces.pGameModeManager);
	SAFE_DELETE(g_interfaces.pD3D9ExWrapper);

	SAFE_DELETE(g_interfaces.pSteamFriendsWrapper);
	SAFE_DELETE(g_interfaces.pSteamMatchmakingWrapper);
	SAFE_DELETE(g_interfaces.pSteamNetworkingWrapper);
	SAFE_DELETE(g_interfaces.pSteamUserStatsWrapper);
	SAFE_DELETE(g_interfaces.pSteamUserWrapper);
	SAFE_DELETE(g_interfaces.pSteamUtilsWrapper);
	SAFE_DELETE(g_interfaces.pSteamApiHelper);
}
