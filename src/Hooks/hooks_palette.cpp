#include "hooks_palette.h"

#include "HookManager.h"

#include "Core/interfaces.h"
#include "Core/logger.h"
#include "Game/custom_gameModes.h"
#include "PaletteManager/custom_palette.h"

#include <steam_api.h>

//DWORD jmpBackAddr = hookAddress + hookLength;
DWORD GetPlayersPaletteIndexSPJmpBackAddr = 0;
DWORD GetCPUsPaletteIndexSPJmpBackAddr = 0;
DWORD P1PaletteIndexMPJmpBackAddr = 0;
DWORD P2PaletteIndexMPJmpBackAddr = 0;
DWORD GrabP1P2PalettePointersJmpBackAddr = 0;
DWORD GetOpponentSteamIDJmpBackAddr = 0;
DWORD GetOwnSteamIDJmpBackAddr = 0;
DWORD StartMatchWheelOfFateJmpBackAddr = 0;
DWORD MatchIntroStartsPlayingJmpBackAddr = 0;
DWORD GetP1P2CharIDCharSelectSPJmpBackAddr = 0;
DWORD GetOpponentCharIDCharSelectMPJmpBackAddr = 0;
DWORD GetPlayersCharIDVersusScreenSPJmpBackAddr = 0;
DWORD GetCPUsCharIDVersusScreenSPJmpBackAddr = 0;
DWORD GetP1CharIDVersusScreenMPJmpBackAddr = 0;
DWORD GetP2CharIDVersusScreenMPJmpBackAddr = 0;
DWORD GetGameStateCharacterSelectJmpBackAddr = 0;
DWORD GetP1CharIDTrainingModeCharIDChangeJmpBackAddr = 0;
DWORD GetP2CharIDTrainingModeCharIDChangeJmpBackAddr = 0;
int restoredGetP2CharIDTrainingModeCharAddr = 0;

DWORD GetP1P2CharSelectPalettePointerIndexesJmpBackAddr = 0;

int debug_id = 0;

DWORD* CharSelectPalettesBasePointer = 0;
DWORD GetCharacterSelScreenPalPointerJmpBackAddr = 0;
int restoredCallAddr = 0;
void __declspec(naked)GetCharacterSelScreenPalPointer()
{
	__asm pushad
	LOG(2, "GetCharacterSelScreenPalPointer\n");
	LOG(2, "\trestoredCallAddr: 0x%x\n", restoredCallAddr);
	__asm popad

	__asm
	{
		mov CharSelectPalettesBasePointer, eax
		mov ecx, eax
		call restoredCallAddr
		jmp[GetCharacterSelScreenPalPointerJmpBackAddr]
	}
}

DWORD GetOpponentSteamIDAtCharSelectJmpBackAddr = 0;
CSteamID* steamid;
void __declspec(naked)GetOpponentSteamIDAtCharSelect()
{
	__asm
	{
		mov[esi], eax
		mov steamid, eax
		pushad
	}
	LOG(2, "steamid: 0x%x\n", steamid);
	__asm
	{
		popad
		mov eax, [edi + 4]
		jmp[GetOpponentSteamIDAtCharSelectJmpBackAddr]
	}
}

DWORD GetCharObjPointersJmpBackAddr = 0;
void __declspec(naked)GetCharObjPointers()
{
	LOG_ASM(2, "GetCharObjPointers\n");

	__asm
	{
		pushad
		mov ebx, eax
		mov eax, 4
		mul edi
		add eax, ebx
		add eax, 25E8h //calculated eax + edi * 4 + 25E8h
		mov eax, [eax] //get from static addresses 00C19DF0 and 00C19DF4 the addresses of player objects
		cmp edi, 1
		je PLAYER2
		PLAYER1 :
		mov g_gameVals.P1CharObjPointer, eax
			jmp EXIT
			PLAYER2 :
		mov g_gameVals.P2CharObjPointer, eax
			EXIT :
		popad
			mov[eax + edi * 4 + 25E8h], esi
			jmp[GetCharObjPointersJmpBackAddr]
	}
}

DWORD ForceBloomOnJmpBackAddr = 0;
int restoredForceBloomOffAddr = 0;
void __declspec(naked)ForceBloomOn()
{

	__asm pushad
	LOG(7, "ForceBloomOn\n");

	if (g_gameVals.P1_selectedCharID != -1)
	{
		if (g_gameVals.origP1PaletteIndex == 21)
			g_gameVals.isP1BloomOn = 1;
		else if (g_gameVals.P1_selected_custom_pal > 0)
			g_gameVals.isP1BloomOn = g_gameVals.customPalettes[g_gameVals.P1_selectedCharID][g_gameVals.P1_selected_custom_pal][9][0];
	}
	if (g_gameVals.P2_selectedCharID != -1)
	{
		if (g_gameVals.origP2PaletteIndex == 21)
			g_gameVals.isP2BloomOn = 1;
		else if (g_gameVals.P2_selected_custom_pal > 0)
			g_gameVals.isP2BloomOn = g_gameVals.customPalettes[g_gameVals.P2_selectedCharID][g_gameVals.P2_selected_custom_pal][9][0];
	}
	__asm popad

	__asm
	{
		mov eax, [g_gameVals.P1CharObjPointer]
		cmp edi, eax
		jne PLAYER2
		PLAYER1 :
		cmp g_gameVals.isP1BloomOn, 1
			je TURN_BLOOM_ON
			jmp[restoredForceBloomOffAddr]
			PLAYER2 :
			cmp g_gameVals.isP2BloomOn, 1
			je TURN_BLOOM_ON
			jmp[restoredForceBloomOffAddr]
			TURN_BLOOM_ON :
			jmp[ForceBloomOnJmpBackAddr]
	}
}

DWORD CPUsPaletteIndexAndCharIDArcadeFixJmpBackAddr = 0;
void __declspec(naked)CPUsPaletteIndexAndCharIDArcadeFix()
{
	__asm pushad
	LOG(2, "CPUsPaletteIndexAndCharIDArcadeFix\n");
	if (g_gameVals.isP1CPU)
		g_gameVals.P1DefaultPalBackup[0] = "";
	else
		g_gameVals.P2DefaultPalBackup[0] = "";
	__asm popad

	__asm
	{
		mov ebx, edi
		add ebx, 1650h
		cmp g_gameVals.isP1CPU, 1
		je SET_TO_PLAYER1
		mov g_gameVals.P2PaletteIndex, ebx
		mov g_gameVals.P2_selectedCharID, eax
		jmp EXIT
		SET_TO_PLAYER1 :
		mov g_gameVals.P1PaletteIndex, ebx
			mov g_gameVals.P1_selectedCharID, eax
			EXIT :
		mov[edi + 1648h], eax
			jmp[CPUsPaletteIndexAndCharIDArcadeFixJmpBackAddr]
	}
}

DWORD GetIsP1CPUJmpBackAddr = 0;
void __declspec(naked)GetIsP1CPU()
{
	LOG_ASM(2, "GetIsP1CPU\n");

	__asm
	{
		mov[eax + 1688h], edi
		mov g_gameVals.isP1CPU, edi;
		jmp[GetIsP1CPUJmpBackAddr]
	}
}

void __declspec(naked)GetP1CharIDTrainingModeCharIDChange()
{
	__asm pushad
	LOG(2, "GetP1CharIDTrainingModeCharIDChange\n");
	g_gameVals.P1DefaultPalBackup[0] = "";
	g_tempVals.paledit_show_sel_by_highlight = false;
	g_tempVals.paledit_show_placeholder = false;
	__asm popad

	__asm
	{
		mov ecx, [ebp - 1Ch]
		cmp[g_gameVals.isP1CPU], 1
		je PLAYER2
		PLAYER1 :
		cmp g_gameVals.P1_selectedCharID, ecx
			JNE CHANGE1
			KEEP1 :
		push ecx
			push eax
			mov ecx, g_gameVals.origP1PaletteIndex //*g_gameVals.P1PaletteIndex = g_gameVals.origP1PaletteIndex;
			mov eax, g_gameVals.P1PaletteIndex
			mov[eax], ecx
			pop eax
			pop ecx
			jmp[EXIT]
			CHANGE1:
		mov g_gameVals.P1_selectedCharID, ecx
			mov g_gameVals.P1_selected_custom_pal, 0
			mov g_gameVals.origP1PaletteIndex, 0
			jmp[EXIT]
			PLAYER2 :
			cmp g_gameVals.P2_selectedCharID, ecx
			JNE CHANGE2
			KEEP2 :
		push ecx
			push eax
			mov ecx, g_gameVals.origP2PaletteIndex //*g_gameVals.P2PaletteIndex = g_gameVals.origP2PaletteIndex;
			mov eax, g_gameVals.P2PaletteIndex
			mov[eax], ecx
			pop eax
			pop ecx
			jmp[EXIT]
			CHANGE2:
		mov g_gameVals.P2_selectedCharID, ecx
			mov g_gameVals.P2_selected_custom_pal, 0
			mov g_gameVals.origP2PaletteIndex, 0
			EXIT :
			mov[eax], ecx
			jmp[GetP1CharIDTrainingModeCharIDChangeJmpBackAddr]
	}
}

void __declspec(naked)GetP2CharIDTrainingModeCharIDChange()
{
	__asm pushad
	LOG(2, "GetP2CharIDTrainingModeCharIDChange\n");
	g_gameVals.P2DefaultPalBackup[0] = "";
	g_tempVals.paledit_show_sel_by_highlight = false;
	g_tempVals.paledit_show_placeholder = false;
	__asm popad

	__asm
	{
		push edi
		mov[eax], esi
		cmp[g_gameVals.isP1CPU], 1
		je PLAYER1
		cmp g_gameVals.P2_selectedCharID, esi
		JNE CHANGE2
		KEEP2 :
		push esi
			push eax
			mov esi, g_gameVals.origP2PaletteIndex //*g_gameVals.P2PaletteIndex = g_gameVals.origP2PaletteIndex;
			mov eax, g_gameVals.P2PaletteIndex
			mov[eax], esi
			pop eax
			pop esi
			jmp[EXIT]
			CHANGE2:
		mov g_gameVals.P2_selectedCharID, esi
			mov g_gameVals.P2_selected_custom_pal, 0
			mov g_gameVals.origP2PaletteIndex, 0
			jmp[EXIT]
			PLAYER1 :
			cmp g_gameVals.P1_selectedCharID, esi
			JNE CHANGE1
			KEEP1 :
		push esi
			push eax
			mov esi, g_gameVals.origP1PaletteIndex //*g_gameVals.P1PaletteIndex = g_gameVals.origP1PaletteIndex;
			mov eax, g_gameVals.P1PaletteIndex
			mov[eax], esi
			pop eax
			pop esi
			jmp[EXIT]
			CHANGE1:
		mov g_gameVals.P1_selectedCharID, esi
			mov g_gameVals.P1_selected_custom_pal, 0
			mov g_gameVals.origP1PaletteIndex, 0
			EXIT :
			call restoredGetP2CharIDTrainingModeCharAddr
			jmp[GetP2CharIDTrainingModeCharIDChangeJmpBackAddr]
	}
}

void __declspec(naked)GetP2CharIDVersusScreenMP()
{
	LOG_ASM(2, "GetP2CharIDVersusScreenMP\n");

	__asm
	{
		mov g_gameVals.P2_selectedCharID, ecx
		mov[eax], ecx
		push 1
		mov ecx, edi
		jmp[GetP2CharIDVersusScreenMPJmpBackAddr]
	}
}

void __declspec(naked)GetP1CharIDVersusScreenMP()
{
	LOG_ASM(2, "GetP1CharIDVersusScreenMP\n");

	__asm
	{
		mov g_gameVals.P1_selectedCharID, ecx
		mov[eax], ecx
		push 0
		mov ecx, edi
		jmp[GetP1CharIDVersusScreenMPJmpBackAddr]
	}
}

void __declspec(naked)GetCPUsCharIDVersusScreenSP()
{
#ifdef _DEBUG
	__asm pushad
	__asm mov debug_id, eax
	LOG(2, "GetCPUsCharIDVersusScreenSP, id:%d\n", debug_id);
	__asm popad
#endif

	__asm
	{
		cmp g_gameVals.isP1CPU, 1
		jne SET_TO_PLAYER2
		mov g_gameVals.P1_selectedCharID, eax
		jmp EXIT
		SET_TO_PLAYER2 :
		mov g_gameVals.P2_selectedCharID, eax
			EXIT :
		mov[esi + 1648h], eax
			jmp[GetCPUsCharIDVersusScreenSPJmpBackAddr]
	}
}

void __declspec(naked)GetPlayersCharIDVersusScreenSP()
{
#ifdef _DEBUG
	__asm pushad
	__asm mov debug_id, eax
	LOG(2, "GetPlayersCharIDVersusScreenSP, id:%d\n", debug_id);
	__asm popad
#endif

	__asm
	{
		cmp g_gameVals.isP1CPU, 1
		je SET_TO_PLAYER2
		cmp g_tempVals.PlayersCharIDVersusScreenCounter, 1
		je SET_TO_PLAYER2
		mov g_gameVals.P1_selectedCharID, eax
		jmp EXIT
		SET_TO_PLAYER2 :
		mov g_gameVals.P2_selectedCharID, eax
			EXIT :
		mov[edi + 1648h], eax
			jmp[GetPlayersCharIDVersusScreenSPJmpBackAddr]
	}
}

void __declspec(naked)GetP1P2CharSelectPalettePointerIndexes()
{
	LOG_ASM(2, "GetP1P2CharSelectPalettePointerIndexes\n");

	__asm
	{
		pushad
		add esi, 0A14h
		mov P1CharSelectPalIndex, esi
		add esi, 4h
		mov P2CharSelectPalIndex, esi
		popad

		mov[esi + 0A14h], ecx
		jmp[GetP1P2CharSelectPalettePointerIndexesJmpBackAddr]
	}
}

void __declspec(naked)GetP1P2CharIDCharSelectSP()
{
	int curSelection;

	__asm pushad
	LOG(7, "GetP1P2CharIDCharSelectSP\n");
	OverwriteStagesList();
	__asm popad

	__asm
	{
		pushad
		mov curSelection, eax
		cmp edi, 0
		jne PLAYER2
		PLAYER1 :
	}
	//reset custom palette selection if player hovers over an another character, so we avoid array out of bounds
	g_gameVals.thisPlayerNum = 1;
	if (curSelection != g_gameVals.P1_selectedCharID)
		g_gameVals.P1_selected_custom_pal = 0;
	__asm
	{
		mov g_gameVals.P1_selectedCharID, eax
		jmp EXIT
		PLAYER2 :
	}
	//reset custom palette selection if player hovers over an another character, so we avoid array out of bounds
	g_gameVals.thisPlayerNum = 2;
	if (curSelection != g_gameVals.P2_selectedCharID)
		g_gameVals.P2_selected_custom_pal = 0;
	__asm
	{
		mov g_gameVals.P2_selectedCharID, eax
		EXIT :
		popad
			mov[esi + edi * 4 + 0A0Ch], eax
			jmp[GetP1P2CharIDCharSelectSPJmpBackAddr]
	}
}

void __declspec(naked)GetGameStateCharacterSelect()
{
	LOG_ASM(2, "GetGameStateCharacterSelect\n");

	__asm pushad

	g_gameVals.startMatchPalettesInit = false;
	ResettingDefaultPalettes();

	//as of right now, the opponent's steamid is not known until the match starts
	//if (g_gameVals.opponentSteamID)
	//	ImGuiSystem::AddLog("[system] Online match has started.\n");
	if (g_tempVals.opponentIDToBeDeleted)
	{
		LOG(2, "Deleting previous opponentSteamID, resetting...\n");
		delete g_tempVals.opponentIDToBeDeleted;
		g_tempVals.opponentIDToBeDeleted = 0;
	}
	__asm popad

	__asm
	{
		mov dword ptr[ebx + 10Ch], 6
		jmp[GetGameStateCharacterSelectJmpBackAddr]
	}
}

void __declspec(naked)MatchIntroStartsPlayingFunc()
{
	//this function runs whenever the camera is forcibly taken control by the game

	__asm pushad
	LOG(2, "MatchIntroStartsPlayingFunc\n");

	InitCustomGameMode();

	if (!g_gameVals.startMatchPalettesInit &&
		g_gameVals.P1_selectedCharID != -1 &&
		g_gameVals.P2_selectedCharID != -1)
	{
		LOG(2, "Applying custom palettes, sending to opponent...\n");

		SaveDefaultP1P2Pals();

		////prevent sending both palettes in MP, since we only want to send OUR player's palette
		ReplaceP1Palette(false);
		ReplaceP2Palette(false);
		////send now once
		SendCustomDatas();
		g_gameVals.startMatchPalettesInit = true;
		HandleSavedPackets();
	}
	__asm
	{
		popad
		and dword ptr[eax + 40626Ch], 0FFFFFFFDh
		jmp[MatchIntroStartsPlayingJmpBackAddr]
	}
}

void __declspec(naked)GetOwnSteamID()
{
	LOG_ASM(2, "GetOwnSteamID\n");

	__asm
	{
		mov[edi], eax
		cmp g_gameVals.ownSteamID, 0
		jne EXIT
		mov g_gameVals.ownSteamID, edi
	}

	LOG_ASM(2, "OwnSteamID: 0x%x\n", *g_gameVals.ownSteamID);

	__asm
	{
	EXIT:
		mov eax, [esi + 4]
			jmp[GetOwnSteamIDJmpBackAddr]
	}
}

void __declspec(naked)StartMatchWheelOfFate()
{
	__asm pushad
	LOG(2, "StartMatchWheelOfFate\n");

	__asm
	{
		popad
		mov[ebx + 30h], 00000002
		jmp[StartMatchWheelOfFateJmpBackAddr]
	}
}

//single player
void __declspec(naked) GetPlayersPaletteIndexSPPointer()
{
	static int counter = 0;
#ifdef _DEBUG
	__asm pushad
	__asm
	{
		mov eax, edi
		add eax, 1650h
		mov debug_id, eax
	}
	LOG(2, "GetPlayersPaletteIndexSPPointer, addr:0x%x\n", debug_id);
	__asm popad
#endif

	__asm
	{
		push eax
		mov eax, edi // <---- esi to edi fixed here!!!!!!!!!!!!!!!!!!!!!!!
		add eax, 1650h
		cmp g_gameVals.isP1CPU, 1
		je SET_TO_PLAYER2
		cmp g_tempVals.PlayersCharIDVersusScreenCounter, 1
		je SET_TO_PLAYER2
		mov g_gameVals.P1PaletteIndex, eax //grab address
		pop eax
		mov g_gameVals.origP1PaletteIndex, eax //get value of paletteIndex
		jmp EXIT
		SET_TO_PLAYER2 :
		mov g_gameVals.P2PaletteIndex, eax //grab address
			pop eax
			mov g_gameVals.origP2PaletteIndex, eax //get value of paletteIndex
			EXIT :
		add g_tempVals.PlayersCharIDVersusScreenCounter, 1
			mov[edi + 1650h], eax //original opcodes here
			jmp[GetPlayersPaletteIndexSPJmpBackAddr] //jump back
	}
}

//single player
void __declspec(naked) GetCPUsPaletteIndexSPPointer()
{
#ifdef _DEBUG
	__asm pushad
	__asm
	{
		mov eax, esi
		add eax, 1650h
		mov debug_id, eax
	}
	LOG(2, "GetCPUsPaletteIndexSPPointer, addr:0x%x\n", debug_id);
	__asm popad
#endif

	__asm
	{
		push eax
		mov eax, esi
		add eax, 1650h
		cmp g_gameVals.isP1CPU, 1
		jne SET_TO_PLAYER2
		mov g_gameVals.P1PaletteIndex, eax
		pop eax
		mov g_gameVals.origP2PaletteIndex, eax //get value of paletteIndex
		jmp EXIT
		SET_TO_PLAYER2 :
		mov g_gameVals.P2PaletteIndex, eax //grab address
			pop eax
			mov g_gameVals.origP2PaletteIndex, eax //get value of paletteIndex
			EXIT :
		mov[esi + 1650h], eax //original opcodes here
			jmp[GetCPUsPaletteIndexSPJmpBackAddr] //jump back
	}
}

void __declspec(naked) GetP1PaletteIndexMPPointer()
{
	LOG_ASM(2, "GetP1PaletteIndexMPPointer\n");

	__asm
	{
		push eax
		add eax, 8
		mov[g_gameVals.P1PaletteIndex], eax
		pop eax
		//..
		mov[eax + 8], ecx //orig
		movzx esi, byte ptr[ebx + 000003A8h]
		jmp[P1PaletteIndexMPJmpBackAddr] //jump back
	}
}

void __declspec(naked) GetP2PaletteIndexMPPointer()
{
	LOG_ASM(2, "GetP2PaletteIndexMPPointer\n");

	__asm
	{
		push eax
		add eax, 8
		mov[g_gameVals.P2PaletteIndex], eax
		pop eax
		//..
		mov[eax + 8], ecx
		movzx esi, byte ptr[ebx + 00000414h]
		jmp[P2PaletteIndexMPJmpBackAddr] //jump back
	}
}

void __declspec(naked) GrabP1P2PalettePointers()
{
	static int counter = 0;

	LOG_ASM(2, "GrabP1P2PalettePointers\n");

	__asm
	{
		//dostuffs here
		pushad // save registers

		cmp DWORD PTR counter, 0
		jne SHORT COUNTERCOMPARE2
		mov DWORD PTR g_gameVals.P1PaletteBase, eax
		jmp SHORT COUNTERCOMPARE3
		COUNTERCOMPARE2 :
		cmp DWORD PTR counter, 1
			jne SHORT COUNTERCOMPARE3
			mov DWORD PTR g_gameVals.P2PaletteBase, eax
			COUNTERCOMPARE3 :
		mov eax, DWORD PTR counter
			add eax, 1
			mov DWORD PTR counter, eax
			cmp DWORD PTR counter, 3
			jne SHORT EXIT
			EXIT :
	}
	if (counter == 3)
	{
		if (g_gameVals.P1PaletteIndex != 0 && g_gameVals.P2PaletteIndex != 0)
		{
			//backup the values of palette indexes as we will switch them, so we know what they were originally
			if (g_gameVals.startMatchPalettesInit == false)
			{
				g_gameVals.origP1PaletteIndex = *g_gameVals.P1PaletteIndex;
				g_gameVals.origP2PaletteIndex = *g_gameVals.P2PaletteIndex;
			}
			DereferenceP1P2PalettePointers();
		}
		counter = 0;
	}
	__asm
	{
		popad // restore registers
		//...
		mov[ecx + 830h], eax //original opcodes here
		jmp[GrabP1P2PalettePointersJmpBackAddr] //jump back
	}
}

bool placeHooks_palette()
{
	//single player
	GetPlayersPaletteIndexSPJmpBackAddr = HookManager::SetHook(
		"GetPlayersPaletteIndexSPPointerFunc",
		"\x89\x87\x50\x16\x00\x00\x8b\x83\x48\x06\x00\x00",
		"xxxxxxxxxxxx",
		6,
		GetPlayersPaletteIndexSPPointer);
	
	GetCPUsPaletteIndexSPJmpBackAddr = HookManager::SetHook("GetCPUsPaletteIndexSPPointerFunc",
		"\x89\x86\x50\x16\x00\x00\x8b\x84\x0a\x48\x06\x00\x00",
		"xxxxxxxxxxxxx", 6, GetCPUsPaletteIndexSPPointer);
	
	CPUsPaletteIndexAndCharIDArcadeFixJmpBackAddr = HookManager::SetHook(
		"P2PaletteIndexSPVersusFix",
		"\x89\x87\x48\x16\x00\x00\xc7\x87\x50\x16\x00\x00\x00\x00\x00\x00",
		"xxxxxxxxxxxxxxxx",
		6,
		CPUsPaletteIndexAndCharIDArcadeFix);
	
	P1PaletteIndexMPJmpBackAddr = HookManager::SetHook(
		"P1PaletteIndexMPPointerFunc",
		"\x89\x48\x08\x0f\xb6\xb3\xa8\x03\x00\x00\x6a\x00",
		"xxxxxxxxxxxx",
		10,
		GetP1PaletteIndexMPPointer);
	
	P2PaletteIndexMPJmpBackAddr = HookManager::SetHook(
		"P2PaletteIndexMPPointerFunc",
		"\x89\x48\x08\x0f\xb6\xb3\x14\x04\x00\x00\x6a\x01",
		"xxxxxxxxxxxx",
		10,
		GetP2PaletteIndexMPPointer);
	
	GrabP1P2PalettePointersJmpBackAddr = HookManager::SetHook(
		"GrabP1P2PalettePointerFunc",
		"\x89\x81\x30\x08\x00\x00\x8b\xc8\xe8\x00\x00\x00\x00\x5f",
		"xxxxxxxxx????x",
		6,
		GrabP1P2PalettePointers);
	
	GetOwnSteamIDJmpBackAddr = HookManager::SetHook(
		"GetOwnSteamID",
		"\x89\x07\x8b\x46\x04\x89\x47\x04\x8d\x46\x08",
		"xxxxxxxxxxx",
		5,
		GetOwnSteamID);

	StartMatchWheelOfFateJmpBackAddr = HookManager::SetHook(
		"StartMatchWheelOfFate",
		"\xc7\x43\x30\x02\x00\x00\x00\xc7\x43\x34\x01\x00\x00\x00\xe8\x00\x00\x00\x00\x83",
		"xxxxxxxxxxxxxxx????x",
		7,
		StartMatchWheelOfFate);
	
	MatchIntroStartsPlayingJmpBackAddr = HookManager::SetHook(
		"MatchIntroStartsPlaying",
		"\x83\xa0\x6c\x62\x40\x00\xfd\x83\x66\x24\xef\x5f\x5e\x33\xc0",
		"xxxxxxxxxxxxxxx",
		7,
		MatchIntroStartsPlayingFunc);

	GetGameStateCharacterSelectJmpBackAddr = HookManager::SetHook(
		"GetGameStateCharacterSelect",
		"\xc7\x83\x0c\x01\x00\x00\x06\x00\x00\x00\xe8",
		"xxxxxxxxxxx",
		10,
		GetGameStateCharacterSelect);

	GetP1P2CharIDCharSelectSPJmpBackAddr = HookManager::SetHook(
		"GetP1P2CharIDCharSelectSP",
		"\x89\x84\xbe\x0c\x0a\x00\x00\xe8",
		"xxxxxxxx",
		7,
		GetP1P2CharIDCharSelectSP);

	GetPlayersCharIDVersusScreenSPJmpBackAddr = HookManager::SetHook(
		"GetPlayersCharIDVersusScreenSP",
		"\x89\x87\x48\x16\x00\x00\x8b\x83\x44\x06\x00\x00",
		"xxxxxxxxxxxx",
		6,
		GetPlayersCharIDVersusScreenSP);
	
	GetCPUsCharIDVersusScreenSPJmpBackAddr = HookManager::SetHook(
		"GetCPUsCharIDVersusScreenSP",
		"\x89\x86\x48\x16\x00\x00\x8b\x84\x0a\x44\x06\x00\x00",
		"xxxxxxxxxxxxx",
		6,
		GetCPUsCharIDVersusScreenSP);
	
	GetP1CharIDVersusScreenMPJmpBackAddr = HookManager::SetHook(
		"GetP1CharIDVersusScreenMP",
		"\x89\x08\x6a\x00\x8b\xcf",
		"xxxxxx",
		6,
		GetP1CharIDVersusScreenMP);
	
	GetP2CharIDVersusScreenMPJmpBackAddr = HookManager::SetHook(
		"GetP2CharIDVersusScreenMP",
		"\x89\x08\x6a\x01\x8b\xcf",
		"xxxxxx",
		6,
		GetP2CharIDVersusScreenMP);

	GetP1CharIDTrainingModeCharIDChangeJmpBackAddr = HookManager::SetHook(
		"GetP1CharIDTrainingModeCharChange",
		"\x8b\x4d\xe4\x89\x08\x68",
		"xxxxxx",
		5,
		GetP1CharIDTrainingModeCharIDChange);

	GetP2CharIDTrainingModeCharIDChangeJmpBackAddr = HookManager::SetHook(
		"GetP2CharIDTrainingModeCharChange",
		"\x57\x89\x30\xe8\x00\x00\x00\x00\x8b\xc8",
		"xxxx????xx",
		8,
		GetP2CharIDTrainingModeCharIDChange,
		false);
	restoredGetP2CharIDTrainingModeCharAddr =
		GetP2CharIDTrainingModeCharIDChangeJmpBackAddr + HookManager::GetBytesFromAddr(
		"GetP2CharIDTrainingModeCharChange",
		4,
		4);
	HookManager::ActivateHook("GetP2CharIDTrainingModeCharChange");

	GetCharObjPointersJmpBackAddr = HookManager::SetHook(
		"GetCharObjPointers",
		"\x89\xb4\xb8\xe8\x25\x00\x00\x8b\x45\xfc",
		"xxxxxxxxxx",
		7,
		GetCharObjPointers);

	ForceBloomOnJmpBackAddr = HookManager::SetHook(
		"ForceBloomOn",
		"\x83\xfe\x15\x75",
		"xxxx",
		5,
		ForceBloomOn,
		false);
	restoredForceBloomOffAddr =
		ForceBloomOnJmpBackAddr + HookManager::GetBytesFromAddr(
		"ForceBloomOn",
		4,
		1);
	HookManager::ActivateHook("ForceBloomOn");

	GetIsP1CPUJmpBackAddr = HookManager::SetHook(
		"GetIsP1CPU",
		"\x89\xb8\x88\x16\x00\x00\x8b\x83\xa4\x64\x40\x00",
		"xxxxxxxxxxxx",
		6,
		GetIsP1CPU);

	return true;
}