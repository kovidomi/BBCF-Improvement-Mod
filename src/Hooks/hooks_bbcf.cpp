#include "hooks_bbcf.h"

#include "hooks_palette.h"
#include "hooks_steamApiWrapper.h"

#include "Core/interfaces.h"
#include "Core/logger.h"
#include "Core/utils.h"
#include "Game/custom_gameModes.h"
#include "Game/gamestates.h"
#include "Hooks/HookManager.h"
#include "Overlay/Logger/ImGuiLogger.h"
#include "Overlay/WindowManager.h"
#include "PaletteManager/custom_palette.h"

DWORD GetGameStateTitleScreenJmpBackAddr = 0;
void __declspec(naked)GetGameStateTitleScreen()
{
	_asm pushad
	LOG(2, "GetGameStateTitleScreen\n");
	_asm popad

	_asm
	{
		pushad
		add edi, 10Ch
		lea ebx, g_gameVals.pGameState
		mov[ebx], edi
	}

	placeHooks_steamApiWrapper();

	WindowManager::GetInstance()
		.Initialize(g_gameProc.hWndGameWindow, g_interfaces.pD3D9ExWrapper);

	__asm
	{
		popad
		mov dword ptr[edi + 10Ch], 4 //original bytes
		jmp[GetGameStateTitleScreenJmpBackAddr]
	}
}

void ResetBackToMenu()
{
	//copying the same stuffs here as in GameStateLobby
	if (g_gameVals.opponentSteamID)
	{
		g_tempVals.opponentIDToBeDeleted = g_gameVals.opponentSteamID;
		g_gameVals.opponentSteamID = 0;
	}

	g_gameVals.thisPlayerNum = 0;
	g_gameVals.bOpponentUsingBBCFIM = false;
	g_gameVals.bP1UsingBBCFIM = false;
	g_gameVals.bP2UsingBBCFIM = false;
	ResettingDefaultPalettes();
	g_gameVals.P1_selectedCharID = -1;
	g_gameVals.P2_selectedCharID = -1;
	g_gameVals.P1CurPalette = 0;
	g_gameVals.P2CurPalette = 0;
	g_gameVals.startMatchPalettesInit = false;
	g_interfaces.pSteamApiHelper->UpdateNumberOfCurrentPlayers();
	g_tempVals.PlayersCharIDVersusScreenCounter = 0;
	g_gameVals.charSelectInit = false;
	g_gameVals.playersInMatch.clear();
	g_gameVals.totalReadPackets = 0;
	g_tempVals.paledit_show_sel_by_highlight = false;
	g_tempVals.paledit_show_placeholder = false;

	EndCustomGameMode();
}

DWORD GetGameStateMenuScreenJmpBackAddr = 0;
void __declspec(naked)GetGameStateMenuScreen()
{
	_asm pushad
	LOG(2, "GetGameStateMenuScreen\n");
	_asm popad

	_asm
	{
		pushad
		add eax, 10Ch
		lea ebx, g_gameVals.pGameState
		mov[ebx], eax
	}

	placeHooks_steamApiWrapper();
	
	WindowManager::GetInstance()
		.Initialize(g_gameProc.hWndGameWindow, g_interfaces.pD3D9ExWrapper);

	ResetBackToMenu();

	__asm
	{
		popad
		mov dword ptr[eax + 10Ch], 1Bh //original bytes
		jmp[GetGameStateMenuScreenJmpBackAddr]
	}
}

DWORD GetGameStateLobbyJmpBackAddress = 0;
void __declspec(naked)GetGameStateLobby()
{
	LOG_ASM(2, "GetGameStateLobby\n");

	__asm pushad

	ResetBackToMenu();

	__asm popad

	__asm
	{
		mov dword ptr[eax + 10Ch], 1Fh
		jmp[GetGameStateLobbyJmpBackAddress]
	}
}

DWORD GetGameStateMatchStartJmpBackAddr = 0;
void __declspec(naked)GetGameStateMatchStart()
{
	_asm pushad
	LOG(2, "GetGameStateMatchStart\n");
	_asm popad
	__asm pushad

	g_gameVals.startMatchPalettesInit = false;
	g_gameVals.isP1BloomOn = 0;
	g_gameVals.isP2BloomOn = 0;
	g_tempVals.PlayersCharIDVersusScreenCounter = 0;

	g_imGuiLogger->LogSeparator();

	if (*g_gameVals.pGameMode == GameMode_Online)
	{
		//g_interfaces.pSteamApiHelper->GetRequestUserInformation(*g_gameVals.opponentSteamID);
		//g_imGuiLogger->Log("[system] Online match against '%s' has started\n", g_interfaces.pSteamFriendsWrapper->GetFriendPersonaName(*g_gameVals.opponentSteamID));
		g_imGuiLogger->Log("[system] Online match has started\n");
#ifdef _DEBUG
		g_imGuiLogger->Log("[debug] Local player is from %s\n", g_interfaces.pSteamUtilsWrapper->GetIPCountry());
#endif
	}
	else
	{
		g_imGuiLogger->Log("[system] Local match has started\n");
	}

	__asm
	{
		popad
		mov dword ptr[eax + 10Ch], 0Fh
		jmp[GetGameStateMatchStartJmpBackAddr]
	}
}

DWORD GetGameStateVictoryScreenJmpBackAddr = 0;
void __declspec(naked)GetGameStateVictoryScreen()
{
	__asm pushad
	LOG(2, "GetGameStateVictoryScreen\n");

	ResettingDefaultPalettes();
	g_gameVals.startMatchPalettesInit = false;
	//restore original palette indexes so they will be correct on rematch
	if (g_gameVals.P1PaletteIndex)
		*g_gameVals.P1PaletteIndex = g_gameVals.origP1PaletteIndex;
	if (g_gameVals.P2PaletteIndex)
		*g_gameVals.P2PaletteIndex = g_gameVals.origP2PaletteIndex;

	__asm popad

	__asm
	{
		mov dword ptr[eax + 10Ch], 10h
		jmp[GetGameStateVictoryScreenJmpBackAddr]
	}
}

DWORD GetGameStateVersusScreenJmpBackAddr = 0;
void __declspec(naked)GetGameStateVersusScreen()
{
	LOG_ASM(2, "GetGameStateVersusScreen\n");

	g_tempVals.PlayersCharIDVersusScreenCounter = 0;

	__asm
	{
		mov dword ptr[eax + 10Ch], 0Eh
		jmp[GetGameStateVersusScreenJmpBackAddr]
	}
}

DWORD GetGameStateReplayMenuScreenJmpBackAddr = 0;
void __declspec(naked)GetGameStateReplayMenuScreen()
{
	__asm pushad
	LOG(2, "GetGameStateReplayMenuScreen\n");

	ResettingDefaultPalettes();
	g_gameVals.P1_selectedCharID = -1;
	g_gameVals.P2_selectedCharID = -1;
	g_gameVals.P1CurPalette = 0;
	g_gameVals.P2CurPalette = 0;
	g_gameVals.startMatchPalettesInit = false;
	g_tempVals.PlayersCharIDVersusScreenCounter = 0;

	EndCustomGameMode();
	__asm popad

	__asm
	{
		mov dword ptr[eax + 10Ch], 1Ah
		jmp[GetGameStateReplayMenuScreenJmpBackAddr];
	}
}

DWORD WindowMsgHandlerJmpBackAddr = 0;
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
void __declspec(naked)PassMsgToImGui()
{
	static bool isWindowManagerInitialized = false;

	__asm
	{
		mov edi, [ebp + 0Ch]
		mov ebx, ecx
	}

	LOG_ASM(7, "PassMsgToImGui\n");
	__asm pushad
	isWindowManagerInitialized = WindowManager::GetInstance().IsInitialized();
	__asm popad

	__asm
	{
		pushad
		movzx eax, isWindowManagerInitialized
		cmp eax, 0
		je SKIP
		push[ebp + 10h] //lparam
		push edi //wParam
		push esi //msg
		push dword ptr[ebx + 1Ch] //hwnd
		call ImGui_ImplWin32_WndProcHandler
		pop ebx //manually clearing stack...
		pop ebx
		pop ebx
		pop ebx
		cmp eax, 1
		je EXIT
SKIP:
		popad
		jmp[WindowMsgHandlerJmpBackAddr]
	}
EXIT:
	__asm
	{
		popad
		mov eax, 1
		retn 0Ch
	}
}

DWORD PacketProcessingFuncJmpBackAddr = 0;
void __declspec(naked)PacketProcessingFunc()
{
	im_packet_internal_t* packet;

	LOG_ASM(7, "PacketProcessingFunc\n");

	__asm mov packet, ecx;

	__asm
	{
		movzx eax, [ecx + 2] // <-- - load the packet code into eax
		cmp eax, 98h // <-- - palette packet code of BBCFIM 2.00 > is 0x98
		je CUSTOMPACKET
		cmp eax, 99h // <-- - palette packet code of BBCFIM 2.00 < is 0x99 (unsupported version but we still process the packet)
		jne SHORT NOTCUSTOMPACKET
		CUSTOMPACKET :
		pushad
			sub ecx, 2 //<-- - set ecx at packet.length2
			mov packet, ecx //<-- - align packet pointer ontop of packet.length2
	}
	g_interfaces.pNetworkManager->PacketProcesser(packet);
	_asm
	{
		popad
		jmp SHORT EXIT
		NOTCUSTOMPACKET :
		mov edx, [ebp - 08h]
			lea edi, [ebp - 24h]
			mov eax, [edx]
			push edi
			push ecx
			mov ecx, edx
			call dword ptr[eax + 00000090h]
			EXIT:
		jmp[PacketProcessingFuncJmpBackAddr]
	}
}

DWORD GetPlayerAvatarBaseAddr = 0;
void __declspec(naked)GetPlayerAvatarBaseFunc()
{
	LOG_ASM(2, "GetPlayerAvatarBaseFunc\n");

	__asm
	{
		mov[ebx + 0CAh], eax
		pushad
		mov eax, ebx
		add eax, 0CAh
		add eax, 6h
		mov g_gameVals.playerAvatarBaseAddr, eax
	}

	g_gameVals.playerAvatarAddr = reinterpret_cast<int*>(g_gameVals.playerAvatarBaseAddr + 0x610C);
	g_gameVals.playerAvatarColAddr = reinterpret_cast<int*>(g_gameVals.playerAvatarBaseAddr + 0x6110);
	g_gameVals.playerAvatarAcc1 = reinterpret_cast<BYTE*>(g_gameVals.playerAvatarBaseAddr + 0x61C4);
	g_gameVals.playerAvatarAcc2 = reinterpret_cast<BYTE*>(g_gameVals.playerAvatarBaseAddr + 0x61C5);

	LOG(2, "g_gameVals.playerAvatarBaseAddr: 0x%x\n", g_gameVals.playerAvatarBaseAddr);
	LOG(2, "g_gameVals.playerAvatarAddr: 0x%x\n", g_gameVals.playerAvatarAddr);
	LOG(2, "g_gameVals.playerAvatarColAddr: 0x%x\n", g_gameVals.playerAvatarColAddr);
	LOG(2, "g_gameVals.playerAvatarAcc1: 0x%x\n", g_gameVals.playerAvatarAcc1);
	LOG(2, "g_gameVals.playerAvatarAcc2: 0x%x\n", g_gameVals.playerAvatarAcc2);
	
	//restore the original optcodes after grabbing the addresses, nothing else to do here
	HookManager::DeactivateHook("GetPlayerAvatarBaseFunc");

	_asm
	{
		popad
		jmp[GetPlayerAvatarBaseAddr]
	}
}

DWORD CpuUsageFixJmpBackAddr = 0;
void __declspec(naked)CpuUsageFix()
{
	__asm pushad
	Sleep(1);
	__asm popad
	__asm
	{
		push esi
		lea eax, [ebp - 8h]
		push eax
		jmp[CpuUsageFixJmpBackAddr]
	}
}

DWORD GetGameModeIndexPointerJmpBackAddr = 0;
void __declspec(naked)GetGameModeIndexPointer()
{
	LOG_ASM(2, "GetGameModeIndexPointer\n");

	__asm
	{
		push eax
		add eax, 108h
		mov[g_gameVals.pGameMode], eax
		pop eax
		mov dword ptr[eax + 108h], 0Dh
		jmp[GetGameModeIndexPointerJmpBackAddr]
	}
}

DWORD GetSetMatchVariablesJmpBackAddr = 0;
void __declspec(naked)GetSetMatchVariables()
{
	LOG_ASM(2, "GetSetMatchVariables\n");

	__asm
	{
		//grab the pointers
		push ecx
		add ecx, 30h
		mov[g_gameVals.pMatchState], ecx //ecx +30h
		sub ecx, 18h
		mov[g_gameVals.pMatchTimer], ecx //ecx +18h
		sub ecx, 14h
		mov[g_gameVals.pMatchRounds], ecx //ecx +4h
		pop ecx
	}

	__asm
	{
		mov dword ptr[ecx + 20Ch], 0
		jmp[GetSetMatchVariablesJmpBackAddr]
	}
}

DWORD MatchStateFightStartJmpBackAddr = 0;
void __declspec(naked)MatchStateFightStart()
{
	LOG_ASM(2, "MatchStateFightStart\n");

	__asm pushad
	if (activatedGameMode == customGameMode_overdrive)
	{
		g_gameVals.P1CharObjPointer->overdriveTimeleft = 1;
		g_gameVals.P2CharObjPointer->overdriveTimeleft = 1;
	}
	__asm popad
	__asm
	{
		mov dword ptr[esi + 30h], 3
		jmp[MatchStateFightStartJmpBackAddr]
	}
}

DWORD GetStageSelectAddrJmpBackAddr = 0;
void __declspec(naked)GetStageSelectAddr()
{
	LOG_ASM(2, "GetStageSelectAddr\n");

	__asm
	{
		mov dword ptr[ecx + 0F54h], 0
		push eax
		mov eax, ecx
		add eax, 0A0h
		mov g_gameVals.stageListMemory, eax
		add eax, 0EB0h
		mov g_gameVals.stageSelect_X, eax
		add eax, 4h
		mov g_gameVals.stageSelect_Y, eax
		pop eax
		jmp[GetStageSelectAddrJmpBackAddr]
	}
}

DWORD GetMusicSelectAddrJmpBackAddr = 0;
void __declspec(naked)GetMusicSelectAddr()
{
	LOG_ASM(2, "GetMusicSelectAddr\n");

	__asm
	{
		mov dword ptr[ecx + 4], 0
		mov g_gameVals.musicSelect_X, ecx
		push eax
		mov eax, ecx
		add eax, 4h
		mov g_gameVals.musicSelect_Y, eax
		pop eax
		jmp[GetMusicSelectAddrJmpBackAddr]
	}
}

DWORD GetP1ScreenPosXJmpBackAddr = 0;
void __declspec(naked)GetP1ScreenPosX()
{
	//LOG_ASM(2, "GetP1ScreenPosX\n");

	__asm
	{
		mov esi, [edi]

		push esi
		add esi, 1C0h
		mov g_gameVals.P1ScreenPosX, esi
	}

	//LOG_ASM(2, "g_gameVals.P1ScreenPosX: 0x%x\n", g_gameVals.P1ScreenPosX);

	__asm
	{
		add esi, 4
		mov g_gameVals.P1ScreenPosY, esi
		pop esi

		add esi, edx
		push ecx
		jmp[GetP1ScreenPosXJmpBackAddr]
	}
}

bool placeHooks_bbcf()
{
	LOG(2, "placeHooks_bbcf\n");

	if (Settings::settingsIni.cpuusagefix)
		CpuUsageFixJmpBackAddr = HookManager::SetHook(
			"CpuBottleneckFix_new", 
			"\x56\x8d\x45\xf8\x50\x8b\xf1",
			"xxxxxxx", 
			5, 
			CpuUsageFix);

	GetGameStateTitleScreenJmpBackAddr = HookManager::SetHook(
		"GetGameStateTitleScreen",
		"\xc7\x87\x0c\x01\x00\x00\x04\x00\x00\x00\x83\xc4\x1c",
		"xxxxxxxxxxxxx",
		10,
		GetGameStateTitleScreen);
	
	GetGameStateMenuScreenJmpBackAddr = HookManager::SetHook(
		"GetGameStateMenuScreen",
		"\xc7\x80\x0c\x01\x00\x00\x1b\x00\x00\x00\xe8\x00\x00\x00\x00",
		"xxxxxxxxxxx????",
		10,
		GetGameStateMenuScreen);
	
	GetGameStateLobbyJmpBackAddress = HookManager::SetHook(
		"GetGameStateLobby",
		"\xc7\x80\x0c\x01\x00\x00\x1f\x00\x00\x00\xe8",
		"xxxxxxxxxxx",
		10,
		GetGameStateLobby);
	
	GetGameStateMatchStartJmpBackAddr = HookManager::SetHook(
		"GetGameStateMatchStart",
		"\xc7\x80\x0c\x01\x00\x00\x0f\x00\x00\x00\xe8",
		"xxxxxxxxxxx",
		10,
		GetGameStateMatchStart);
	
	GetGameStateVictoryScreenJmpBackAddr = HookManager::SetHook(
		"GetGameStateVictoryScreen",
		"\xc7\x80\x0c\x01\x00\x00\x10\x00\x00\x00\xe8",
		"xxxxxxxxxxx",
		10,
		GetGameStateVictoryScreen);
	
	GetGameStateVersusScreenJmpBackAddr = HookManager::SetHook(
		"GetGameStateVersusScreen",
		"\xc7\x80\x0c\x01\x00\x00\x0e\x00\x00\x00\xe8",
		"xxxxxxxxxxx",
		10,
		GetGameStateVersusScreen);
	
	GetGameStateReplayMenuScreenJmpBackAddr = HookManager::SetHook(
		"GetGameStateReplayMenuScreen",
		"\xc7\x80\x0c\x01\x00\x00\x1a\x00\x00\x00\xe8",
		"xxxxxxxxxxx",
		10,
		GetGameStateReplayMenuScreen);
	
	WindowMsgHandlerJmpBackAddr = HookManager::SetHook(
		"WindowMsgHandler",
		"\x8b\x7d\x0c\x8b\xd9\x83\xfe\x10\x77\x00\x74\x00\x8b\xc6",
		"xxxxxxxxx?x?xx",
		5,
		PassMsgToImGui);
	
	PacketProcessingFuncJmpBackAddr = HookManager::SetHook(
		"PacketProcessingFunc",
		"\x8b\x55\xf8\x8d\x7d\xdc\x8b\x02",
		"xxxxxxxx",
		18,
		PacketProcessingFunc);
	
	GetPlayerAvatarBaseAddr = HookManager::SetHook(
		"GetPlayerAvatarBaseFunc",
		"\x89\x83\xca\x00\x00\x00\xe8",
		"xxxxxxx",
		6,
		GetPlayerAvatarBaseFunc);
	
	GetGameModeIndexPointerJmpBackAddr = HookManager::SetHook(
		"GetGameModeIndexPointer",
		"\xc7\x80\x08\x01\x00\x00\x0d\x00\x00\x00\x6a\x00",
		"xxxxxxxxxxxx",
		10,
		GetGameModeIndexPointer);
	
	GetSetMatchVariablesJmpBackAddr = HookManager::SetHook(
		"GetSetMatchVariables",
		"\xc7\x81\x0c\x02\x00\x00\x00\x00\x00\x00\x8b\xce",
		"xxxxxxxxxxxx",
		10,
		GetSetMatchVariables);
	
	MatchStateFightStartJmpBackAddr = HookManager::SetHook(
		"MatchStateFightStart",
		"\xc7\x46\x30\x03\x00\x00\x00\xb8",
		"xxxxxxxx",
		7,
		MatchStateFightStart);
	
	GetStageSelectAddrJmpBackAddr = HookManager::SetHook(
		"GetStageSelectAddr",
		"\xc7\x81\x54\x0f\x00\x00\x00\x00\x00\x00\x8d\x41\x0c",
		"xxxxxxxxxxxxx",
		10,
		GetStageSelectAddr);
	
	GetMusicSelectAddrJmpBackAddr = HookManager::SetHook(
		"GetMusicSelectAddr",
		"\xc7\x41\x04\x00\x00\x00\x00\x8d\x41\x0c",
		"xxxxxxxxxx",
		7,
		GetMusicSelectAddr);

	GetP1ScreenPosXJmpBackAddr = HookManager::SetHook(
		"GetP1ScreenPosX",
		"\x8b\x37\x03\xf2\x51",
		"xxxxx",
		5,
		GetP1ScreenPosX);

	HookManager::RegisterHook(
		"GetMoneyAddr",
		"\xff\x35\x00\x00\x00\x00\x8d\x45\xb4\x68\x00\x00\x00\x00\x50",
		"xx????xxxx????x",
		6);
	g_gameVals.pGameMoney = (int*)HookManager::GetBytesFromAddr("GetMoneyAddr", 2, 4);

	placeHooks_palette();
	CustomGameModeHooks();

	return true;
}