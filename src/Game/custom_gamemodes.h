#pragma once
#include <vector>

struct GameMode_t;
extern int activatedGameMode;
extern int opponent_activatedGameMode;
extern int P1_activatedGameMode;
extern int P2_activatedGameMode;
extern std::vector<GameMode_t> GameModes;

extern float steroid_hp_multiplier;
extern float steroid_overdriveChargeMultiplier;
extern int steroid_maximumHeat;
extern float steroid_heatModifyMultiplier;
extern float steroid_heatLowHPChargeMultiplier;
extern float vampire_healing_percent;
extern float vampire_health_loss_percent_per_sec;
extern float exVampire_healing_percent;
extern float exVampire_health_loss_percent_per_sec;

enum customGameModes
{
	customGameMode_none,
	customGameMode_steroid,
	customGameMode_vampire,
	customGameMode_ex_vampire,
	customGameMode_onepunch,
	customGameMode_twopunch,
	customGameMode_fivepunch,
	customGameMode_tugofwar,
	customGameMode_infiniteheat,
	customGameMode_overdrive,
};

struct GameMode_t
{
	char* name;
	int id;
	char* desc;
	void (*init_func)();
	std::vector<char*> hook_labels;
};

void InitCustomGameModesVector();
void CustomGameModeHooks();
void SetCustomGameMode(int mode);
void InitCustomGameMode();
void EndCustomGameMode();