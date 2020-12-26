#include "hooks_palette.h"

#include "HookManager.h"

#include "Core/interfaces.h"
#include "Core/logger.h"
#include "Game/gamestates.h"

DWORD GetCharObjPointersJmpBackAddr = 0;
void __declspec(naked)GetCharObjPointers()
{
	static char* addr = nullptr;

	LOG_ASM(2, "GetCharObjPointers\n");

	__asm
	{
		pushad
		add eax, 25E8h
		mov addr, eax
	}

	g_interfaces.player1.SetCharDataPtr(addr);
	addr += 0x4;
	g_interfaces.player2.SetCharDataPtr(addr);

	__asm
	{
		popad
		mov[eax + edi * 4 + 25E8h], esi
		jmp[GetCharObjPointersJmpBackAddr]
	}
}

DWORD ForceBloomOnJmpBackAddr = 0;
int restoredForceBloomOffAddr = 0;
void __declspec(naked)ForceBloomOn()
{
	static CharData* pCharObj = nullptr;
	static CharPaletteHandle* pCharHandle = nullptr;

	LOG_ASM(7, "ForceBloomOn\n");

	__asm
	{
		mov [pCharObj], edi
		pushad
	}

	if (pCharObj == g_interfaces.player1.GetData())
	{
		pCharHandle = &g_interfaces.player1.GetPalHandle();
	}
	else
	{
		pCharHandle = &g_interfaces.player2.GetPalHandle();
	}

	if (pCharHandle->IsCurrentPalWithBloom())
	{
		__asm jmp TURN_BLOOM_ON
	}

	__asm
	{
		popad
		jmp[restoredForceBloomOffAddr]
TURN_BLOOM_ON:
		popad
		jmp[ForceBloomOnJmpBackAddr]
	}
}

DWORD GetCPUsPaletteIndexArcadeJmpBackAddr = 0;
void __declspec(naked)GetCPUsPaletteIndexArcade()
{
	static int* pPalIndex = nullptr;

	LOG_ASM(2, "GetCPUsPaletteIndexArcade\n");

	__asm
	{
		pushad
		mov ebx, edi
		add ebx, 1650h
		mov pPalIndex, ebx
	}

	if (g_gameVals.isP1CPU)
	{
		g_interfaces.player1.GetPalHandle().SetPointerPalIndex(pPalIndex);
	}
	else
	{
		g_interfaces.player2.GetPalHandle().SetPointerPalIndex(pPalIndex);
	}

	__asm
	{
		popad
		mov[edi + 1648h], eax
		jmp[GetCPUsPaletteIndexArcadeJmpBackAddr]
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

DWORD GetGameStateCharacterSelectJmpBackAddr = 0;
void __declspec(naked)GetGameStateCharacterSelect()
{
	LOG_ASM(2, "GetGameStateCharacterSelect\n");

	// Add logging

	__asm
	{
		mov dword ptr[ebx + 10Ch], 6
		jmp[GetGameStateCharacterSelectJmpBackAddr]
	}
}

DWORD GetPlayersPaletteIndexSPJmpBackAddr = 0;
void __declspec(naked) GetPlayersPaletteIndexSPPointer()
{
	static int* pPalIndex = nullptr;

	LOG_ASM(2, "GetPlayersPaletteIndexSPPointer\n");

	__asm
	{
		pushad

		mov eax, edi
		add eax, 1650h
		mov pPalIndex, eax
	}

	if (g_gameVals.isP1CPU || !g_interfaces.player1.GetPalHandle().IsNullPointerPalIndex())
	{
		g_interfaces.player2.GetPalHandle().SetPointerPalIndex(pPalIndex);
	}
	else
	{
		g_interfaces.player1.GetPalHandle().SetPointerPalIndex(pPalIndex);
	}

	__asm
	{
		popad
		mov[edi + 1650h], eax
		jmp[GetPlayersPaletteIndexSPJmpBackAddr]
	}
}

DWORD GetCPUsPaletteIndexSPJmpBackAddr = 0;
void __declspec(naked) GetCPUsPaletteIndexSPPointer()
{
	static int* pPalIndex = nullptr;

	LOG_ASM(2, "GetCPUsPaletteIndexSPPointer\n");

	__asm
	{
		pushad
		mov eax, esi
		add eax, 1650h
		mov pPalIndex, eax
	}

	if (g_gameVals.isP1CPU)
	{
		g_interfaces.player1.GetPalHandle().SetPointerPalIndex(pPalIndex);
	}
	else
	{
		g_interfaces.player2.GetPalHandle().SetPointerPalIndex(pPalIndex);
	}

	__asm {
		popad
		mov[esi + 1650h], eax
		jmp[GetCPUsPaletteIndexSPJmpBackAddr]
	}
}

DWORD P1PaletteIndexMPJmpBackAddr = 0;
void __declspec(naked) GetP1PaletteIndexMPPointer()
{
	static int* pPalIndex = nullptr;

	LOG_ASM(2, "GetP1PaletteIndexMPPointer\n");

	__asm
	{
		pushad
		add eax, 8
		mov pPalIndex, eax
	}
	
	g_interfaces.player1.GetPalHandle().SetPointerPalIndex(pPalIndex);

	__asm
	{
		popad
		//..
		mov[eax + 8], ecx //orig
		movzx esi, byte ptr[ebx + 000003A8h]
		jmp[P1PaletteIndexMPJmpBackAddr] //jump back
	}
}

DWORD P2PaletteIndexMPJmpBackAddr = 0;
void __declspec(naked) GetP2PaletteIndexMPPointer()
{
	static int* pPalIndex = nullptr;

	LOG_ASM(2, "GetP2PaletteIndexMPPointer\n");

	__asm
	{
		pushad
		add eax, 8
		mov pPalIndex, eax
	}

	g_interfaces.player2.GetPalHandle().SetPointerPalIndex(pPalIndex);

	__asm
	{
		popad
		//..
		mov[eax + 8], ecx
		movzx esi, byte ptr[ebx + 00000414h]
		jmp[P2PaletteIndexMPJmpBackAddr]
	}
}

DWORD GetPalBaseAddressesJmpBackAddr = 0;
void __declspec(naked) GetPalBaseAddresses()
{
	static int counter = 0;
	static char* palPointer = 0;

	LOG_ASM(2, "GetPalBaseAddresses\n");

	__asm
	{
		pushad

		mov palPointer, eax
	}

	if (counter == 0)
	{
		g_interfaces.player1.GetPalHandle().SetPointerBasePal(palPointer);
	}
	else if (counter == 1)
	{
		g_interfaces.player2.GetPalHandle().SetPointerBasePal(palPointer);
	}
	else
	{
		counter = -1;
	}

	counter++;

	__asm
	{
		popad

		mov[ecx + 830h], eax
		jmp[GetPalBaseAddressesJmpBackAddr]
	}
}

bool placeHooks_palette()
{
	GetCharObjPointersJmpBackAddr = HookManager::SetHook("GetCharObjPointers", "\x89\xb4\xb8\xe8\x25\x00\x00\x8b\x45\xfc",
		"xxxxxxxxxx", 7, GetCharObjPointers);

	GetPalBaseAddressesJmpBackAddr = HookManager::SetHook("GetPalBaseAddresses", "\x89\x81\x30\x08\x00\x00\x8b\xc8\xe8\x00\x00\x00\x00\x5f",
		"xxxxxxxxx????x", 6, GetPalBaseAddresses);

	GetPlayersPaletteIndexSPJmpBackAddr = HookManager::SetHook("GetPlayersPaletteIndexSPPointerFunc", "\x89\x87\x50\x16\x00\x00\x8b\x83\x48\x06\x00\x00",
		"xxxxxxxxxxxx", 6, GetPlayersPaletteIndexSPPointer);

	GetCPUsPaletteIndexSPJmpBackAddr = HookManager::SetHook("GetCPUsPaletteIndexSPPointerFunc", "\x89\x86\x50\x16\x00\x00\x8b\x84\x0a\x48\x06\x00\x00",
		"xxxxxxxxxxxxx", 6, GetCPUsPaletteIndexSPPointer);

	GetCPUsPaletteIndexArcadeJmpBackAddr = HookManager::SetHook("GetCPUsPaletteIndexArcade","\x89\x87\x48\x16\x00\x00\xc7\x87\x50\x16\x00\x00\x00\x00\x00\x00",
		"xxxxxxxxxxxxxxxx", 6, GetCPUsPaletteIndexArcade);

	P1PaletteIndexMPJmpBackAddr = HookManager::SetHook("P1PaletteIndexMPPointerFunc", "\x89\x48\x08\x0f\xb6\xb3\xa8\x03\x00\x00\x6a\x00",
		"xxxxxxxxxxxx", 10, GetP1PaletteIndexMPPointer);

	P2PaletteIndexMPJmpBackAddr = HookManager::SetHook("P2PaletteIndexMPPointerFunc", "\x89\x48\x08\x0f\xb6\xb3\x14\x04\x00\x00\x6a\x01",
		"xxxxxxxxxxxx", 10, GetP2PaletteIndexMPPointer);

	GetGameStateCharacterSelectJmpBackAddr = HookManager::SetHook("GetGameStateCharacterSelect", "\xc7\x83\x0c\x01\x00\x00\x06\x00\x00\x00\xe8",
		"xxxxxxxxxxx", 10, GetGameStateCharacterSelect);

	ForceBloomOnJmpBackAddr = HookManager::SetHook("ForceBloomOn", "\x83\xfe\x15\x75", "xxxx", 5, ForceBloomOn, false);
	restoredForceBloomOffAddr = ForceBloomOnJmpBackAddr + HookManager::GetBytesFromAddr("ForceBloomOn", 4, 1);
	HookManager::ActivateHook("ForceBloomOn");

	GetIsP1CPUJmpBackAddr = HookManager::SetHook("GetIsP1CPU", "\x89\xb8\x88\x16\x00\x00\x8b\x83\xa4\x64\x40\x00",
		"xxxxxxxxxxxx", 6, GetIsP1CPU);

	return true;
}