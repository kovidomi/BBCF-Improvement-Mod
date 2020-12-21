#include "gamestates.h"

#include "Core/interfaces.h"

bool isPaletteEditingEnabledInCurrentGameMode()
{
	return *g_gameVals.pGameMode == GameMode_Training ||
		*g_gameVals.pGameMode == GameMode_Versus;
}

bool isHitboxOverlayEnabledInCurrentGameMode()
{
	return *g_gameVals.pGameMode == GameMode_Training ||
		*g_gameVals.pGameMode == GameMode_Versus ||
		*g_gameVals.pGameMode == GameMode_ReplayTheater;
}

bool isGameModeSelectorEnabledInCurrentGameState()
{
	return *g_gameVals.pGameState == GameState_CharacterSelectionScreen ||
		*g_gameVals.pGameState == GameState_ReplayMenu;
}

bool isGameModeSelectorEnabledInCurrentGameMode()
{
	return *g_gameVals.pGameMode == GameMode_Versus ||
		*g_gameVals.pGameMode == GameMode_Online ||
		*g_gameVals.pGameMode == GameMode_Training ||
		*g_gameVals.pGameState == GameState_ReplayMenu; // Using GameState instead for replay menu
}

bool isStageSelectorEnabledInCurrentGameMode()
{
	return *g_gameVals.pGameMode == GameMode_Online ||
		*g_gameVals.pGameMode == GameMode_Training ||
		*g_gameVals.pGameMode == GameMode_Versus;
}