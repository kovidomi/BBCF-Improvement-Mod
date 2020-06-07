#include "custom_gameModes.h"

#include "cchar.h"

#include "Core/logger.h"
#include "Game/containers.h"
#include "Game/gamestates.h"
#include "Hooks/HookManager.h"
#include "Hooks/hooks_customGameModes.h"
#include "ImGui/ImGuiSystem.h"

#include <imgui.h>

float steroid_hp_multiplier = 2.5;
float steroid_overdriveChargeMultiplier = 2.0;
int steroid_maximumHeat = 15000;
float steroid_heatModifyMultiplier = 1.2;
float steroid_heatLowHPChargeMultiplier = 2.0;
float vampire_healing_percent = 0.60; //60%
float vampire_health_loss_percent_per_sec = 0.01; // 1%
float exVampire_healing_percent = 2; //200%
float exVampire_health_loss_percent_per_sec = 0.015; // 1.5%

std::vector<GameMode_t> GameModes = std::vector<GameMode_t>();
int activatedGameMode = customGameMode_none;
int opponent_activatedGameMode = customGameMode_none;
int P1_activatedGameMode = customGameMode_none;
int P2_activatedGameMode = customGameMode_none;

void SetCustomGameMode(int mode);
void ActivateCustomGameMode();
void InitCustomGameMode();
void ResetAllHooks();
void InitSteroidMode();
void InitVampireMode();
void InitExVampireMode();
void InitOnePunchMode();
void InitTwoPunchMode();
void InitFivePunchMode();
void InitTugOfWar();
void InitInfiniteHeatMode();
void InitOverdriveMode();


void SetCustomGameMode(int mode)
{
	LOG(2, "SetCustomGameMode\n");
	activatedGameMode = mode;
}

void ActivateCustomGameMode()
{
	LOG(2, "ActivateCustomGameMode\n");

	ResetAllHooks(); //making sure we dont leave anything turned on

	//if we are spectators, set up activatedGameMode
	if (Containers::gameVals.thisPlayerNum == 0)
		activatedGameMode = P1_activatedGameMode;

	if (activatedGameMode < GameModes.size())
		GameModes[activatedGameMode].init_func();
	else
		EndCustomGameMode();
}

void InitCustomGameMode()
{
	LOG(2, "InitCustomGameMode\n");
	//if (activatedGameMode != customGameMode_none)
	//	InitIngamePointers();

	if (Containers::gameVals.P1CharObjPointer == 0 || Containers::gameVals.P2CharObjPointer == 0) //|| customGameModeInitialized)
		return;

	if (*Containers::gameVals.pGameMode == GAME_MODE_ONLINE) //online
	{
		if (activatedGameMode == opponent_activatedGameMode || P1_activatedGameMode == P2_activatedGameMode) // only run in online, and if opponent has selected the same mode as us
		{
			ActivateCustomGameMode();
		}
		else
		{
			ImGuiSystem::AddLog("[system] Players couldn't agree on the same game mode. Resetting back to 'Normal'\n");
			EndCustomGameMode();
		}
	}
	else //offline
	{
		ActivateCustomGameMode();
	}
}

void ResetAllHooks()
{
	LOG(2, "ResetAllHooks\n");
	
	for (auto GameMode : GameModes)
	{
		for (auto hookLabel : GameMode.hook_labels)
		{
			HookManager::DeactivateHook(hookLabel);
		}
	}

	int result = HookManager::OverWriteBytes(
		(char*)steroid_HeatModifyJmpBackAddr,
		(char*)steroid_HeatModifyJmpBackAddr + 0x28,
		"\x98\x3A\x00\x00",
		"xxxx",
		"\x10\x27\x00\x00"); //change heat limits of 15000 back to 10000
#ifdef _DEBUG
	if (result != 3)
		LOG(2, "Error, steroid_HeatModify OverWriteBytes didnt reset all 3 addresses, only %d!!\n", result);
#endif
}

void EndCustomGameMode()
{
	LOG(2, "EndCustomGameMode\n");
	activatedGameMode = customGameMode_none;
	opponent_activatedGameMode = customGameMode_none;
	P1_activatedGameMode = customGameMode_none;
	P2_activatedGameMode = customGameMode_none;
	ResetAllHooks();
}

void InitSteroidMode()
{
	LOG(2, "InitSteroidMode\n");
	ImGuiSystem::AddLog("[system] Starting Steroid Mode\n");

	Containers::gameVals.P1CharObjPointer->maxHP *= steroid_hp_multiplier;
	Containers::gameVals.P2CharObjPointer->maxHP *= steroid_hp_multiplier;
	Containers::gameVals.P1CharObjPointer->currentHP *= steroid_hp_multiplier;
	Containers::gameVals.P2CharObjPointer->currentHP *= steroid_hp_multiplier;

	HookManager::ActivateHook("steroid_OverdriveCharge");
	HookManager::ActivateHook("steroid_HeatModify");
	HookManager::ActivateHook("steroid_HealthModify");

	int result = HookManager::OverWriteBytes(
		(char*)steroid_HeatModifyJmpBackAddr,
		(char*)steroid_HeatModifyJmpBackAddr + 0x28,
		"\x10\x27\x00\x00",
		"xxxx",
		"\x98\x3A\x00\x00"); //change heat limits of 10000 to 15000
#ifdef _DEBUG
	if (result != 3)
		LOG(2, "Error, steroid_HeatModify OverWriteBytes didnt reset all 3 addresses!!\n");
#endif
}

void InitVampireMode()
{
	LOG(2, "InitVampireMode\n");
	ImGuiSystem::AddLog("[system] Starting Vampire Mode\n");

	HookManager::ActivateHook("vampire_HealthModify");
	HookManager::ActivateHook("vampire_HealthDrain");
}

void InitExVampireMode()
{
	LOG(2, "InitExVampireMode\n");
	ImGuiSystem::AddLog("[system] Starting exVampire Mode\n");

	HookManager::ActivateHook("exVampire_HealthModify");
	HookManager::ActivateHook("exVampire_HealthDrain");
}

void InitOnePunchMode()
{
	LOG(2, "InitOnePunchMode\n");
	ImGuiSystem::AddLog("[system] Starting One Punch Mode\n");

	*Containers::gameVals.pMatchRounds = 5;
	*Containers::gameVals.pMatchTimer = 60 * 15;
	//new round, recharge 50% of the burst
	Containers::gameVals.P1CharObjPointer->overdrive_meter += 50000;
	if (Containers::gameVals.P1CharObjPointer->overdrive_meter > 100000)
		Containers::gameVals.P1CharObjPointer->overdrive_meter = 100000;

	Containers::gameVals.P2CharObjPointer->overdrive_meter += 50000;
	if (Containers::gameVals.P2CharObjPointer->overdrive_meter > 100000)
		Containers::gameVals.P2CharObjPointer->overdrive_meter = 100000;
	HookManager::ActivateHook("onepunch_HealthModify");
}

void InitTwoPunchMode()
{
	LOG(2, "InitTwoPunchMode\n");
	ImGuiSystem::AddLog("[system] Starting Two Punch Mode\n");

	*Containers::gameVals.pMatchRounds = 5;
	*Containers::gameVals.pMatchTimer = 60 * 15;
	//new round, recharge 50% of the burst
	Containers::gameVals.P1CharObjPointer->overdrive_meter += 50000;
	if (Containers::gameVals.P1CharObjPointer->overdrive_meter > 100000)
		Containers::gameVals.P1CharObjPointer->overdrive_meter = 100000;

	Containers::gameVals.P2CharObjPointer->overdrive_meter += 50000;
	if (Containers::gameVals.P2CharObjPointer->overdrive_meter > 100000)
		Containers::gameVals.P2CharObjPointer->overdrive_meter = 100000;

	HookManager::ActivateHook("twopunch_HealthModify");
}

void InitFivePunchMode()
{
	LOG(2, "InitFivePunchMode\n");
	ImGuiSystem::AddLog("[system] Starting Five Punch Mode\n");

	*Containers::gameVals.pMatchRounds = 5;
	*Containers::gameVals.pMatchTimer = 60 * 60;
	//new round, recharge 50% of the burst
	Containers::gameVals.P1CharObjPointer->overdrive_meter += 50000;
	if (Containers::gameVals.P1CharObjPointer->overdrive_meter > 100000)
		Containers::gameVals.P1CharObjPointer->overdrive_meter = 100000;

	Containers::gameVals.P2CharObjPointer->overdrive_meter += 50000;
	if (Containers::gameVals.P2CharObjPointer->overdrive_meter > 100000)
		Containers::gameVals.P2CharObjPointer->overdrive_meter = 100000;

	HookManager::ActivateHook("fivepunch_HealthModify");
}

void InitTugOfWar()
{
	LOG(2, "InitTugOfWarMode\n");
	ImGuiSystem::AddLog("[system] Starting Tug of War Mode\n");

	*Containers::gameVals.pMatchRounds = 5;
	*Containers::gameVals.pMatchTimer = 60 * 60;

	Containers::gameVals.P1CharObjPointer->currentHP /= 2;
	Containers::gameVals.P2CharObjPointer->currentHP /= 2;

	//new round, recharge 50% of the burst
	Containers::gameVals.P1CharObjPointer->overdrive_meter += 50000;
	if (Containers::gameVals.P1CharObjPointer->overdrive_meter > 100000)
		Containers::gameVals.P1CharObjPointer->overdrive_meter = 100000;

	Containers::gameVals.P2CharObjPointer->overdrive_meter += 50000;
	if (Containers::gameVals.P2CharObjPointer->overdrive_meter > 100000)
		Containers::gameVals.P2CharObjPointer->overdrive_meter = 100000;

	HookManager::ActivateHook("tugofwar_HealthModify");
}

void InitInfiniteHeatMode()
{
	LOG(2, "InitInfiniteHeatMode\n");
	ImGuiSystem::AddLog("[system] Starting Infinite Heat Mode\n");

	//*Containers::gameVals.pMatchTimer = 60 * 60;
	//new round, recharge 50% of the burst
	Containers::gameVals.P1CharObjPointer->heat = 10000;
	Containers::gameVals.P2CharObjPointer->heat = 10000;

	HookManager::ActivateHook("infiniteheat_HeatModify");
}

void InitOverdriveMode()
{
	LOG(2, "InitOverdriveMode\n");
	ImGuiSystem::AddLog("[system] Starting Overdrive Mode\n");

	//*Containers::gameVals.pMatchRounds = 5;
	//*Containers::gameVals.pMatchTimer = 60 * 60;
	//new round, recharge 100% of the burst
	//Containers::gameVals.P1CharObjPointer->overdrive_meter = 0;
	//Containers::gameVals.P2CharObjPointer->overdrive_meter = 0;
	//Containers::gameVals.P1CharObjPointer->overdrive_timeleft = 500;
	//Containers::gameVals.P2CharObjPointer->overdrive_timeleft = 500;

	HookManager::ActivateHook("overdrive_FreezeOverdriveTimeleft");
	HookManager::ActivateHook("overdrive_KeepTimerGoing");
}

void InitCustomGameModesVector()
{
	GameMode_t normal{
		"Normal",
		customGameMode_none,
		NULL,
		EndCustomGameMode };

	GameMode_t steroid{
		"Steroid",
		customGameMode_steroid,
		"Playable in Online, Training, and Versus modes\nApplies to both players:\n" \
		"- Max HP: +150%%\n- Max Heat Gauge: +50%%\n- Heat gain: +20%%\n"\
		"- Automatic Heat gain on low HP: +100%%\n- Burst Gauge gain: +100%%",
		InitSteroidMode,
		{ "steroid_OverdriveCharge", "steroid_HeatModify", "steroid_HealthModify" } };

	GameMode_t vampire{
		"Vampire",
		customGameMode_vampire,
		"Playable in Online, Training, and Versus modes\nApplies to both players:\n" \
		"- 60%% of damage dealt is healed back\n- Losing 1%% max HP per second",
		InitVampireMode,
		{"vampire_HealthModify", "vampire_HealthDrain"} };

	GameMode_t ex_vampire{
		"Extreme Vampire",
		customGameMode_ex_vampire,
		"Playable in Online, Training, and Versus modes\nApplies to both players:\n" \
		"- 200%% of damage dealt is healed back\n- Healing beyond full HP increases max HP\n" \
		"- Losing 1.5%% max HP per second",
		InitExVampireMode,
		{"exVampire_HealthModify", "exVampire_HealthDrain"} };

	GameMode_t onepunch{
		"One Punch",
		customGameMode_onepunch,
		"Playable in Online, and Versus modes\nApplies to both players:\n" \
		"- Any damage results in instakill, unless blocked with barrier\n" \
		"- Each round 50%% of the Burst Gauge is regenerated\n- Each round lasts 15 seconds\n- 5 rounds",
		InitOnePunchMode,
		{"onepunch_HealthModify"} };

	GameMode_t twopunch{
		"Two Punch",
		customGameMode_twopunch,
		"Playable in Online, and Versus modes\nApplies to both players:\n" \
		"- Taking damage twice results in death, unless blocked with barrier\n" \
		"- Each round 50%% of the Burst Gauge is regenerated\n- Each round lasts 15 seconds\n- 5 rounds",
		InitTwoPunchMode,
		{"twopunch_HealthModify"} };

	GameMode_t fivepunch{
		"Five Punch",
		customGameMode_fivepunch,
		"Playable in Online, and Versus modes\nApplies to both players:\n" \
		"- Taking damage five times results in death, unless blocked with barrier\n" \
		"- Each round 50%% of the Burst Gauge is regenerated\n- Each round lasts 60 seconds\n- 5 rounds",
		InitFivePunchMode,
		{"fivepunch_HealthModify"} };

	GameMode_t tugofwar{
		"Tug of War",
		customGameMode_tugofwar,
		"Playable in Online, and Versus modes\nApplies to both players:\n" \
		"- Start with 50%% HP\n- All damages hurt/heal 10%% HP, unless blocked with barrier\n" \
		"- Each round 50%% of the Burst Gauge is regenerated\n- Each round lasts 60 seconds\n- 5 rounds",
		InitTugOfWar,
		{"tugofwar_HealthModify"} };

	GameMode_t infiniteheat{
		"Infinite Heat",
		customGameMode_infiniteheat,
		"Playable in Online, Training, and Versus modes\nApplies to both players:\n" \
		"- Heat Gauge is always full",
		InitInfiniteHeatMode,
		{ "infiniteheat_HeatModify" } };

	GameMode_t overdrive{
		"Overdrive", customGameMode_overdrive,
		"Playable in Online, Training, and Versus modes\nApplies to both players:\n" \
		"- Overdrive is always on",
		InitOverdriveMode,
		{"overdrive_FreezeOverdriveTimeleft", "overdrive_KeepTimerGoing"} };

	GameModes.push_back(normal);
	GameModes.push_back(steroid);
	GameModes.push_back(vampire);
	GameModes.push_back(ex_vampire);
	GameModes.push_back(onepunch);
	GameModes.push_back(twopunch);
	GameModes.push_back(fivepunch);
	GameModes.push_back(tugofwar);
	GameModes.push_back(infiniteheat);
	//GameModes.push_back(overdrive);
}
