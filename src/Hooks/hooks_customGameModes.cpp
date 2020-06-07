#include "hooks_customGameModes.h"

#include "HookManager.h"

#include "Core/logger.h"
#include "Game/containers.h"
#include "Game/custom_gameModes.h"
#include "Game/gamestates.h"

#include <imgui.h>

void steroid_HealthModify();
void steroid_HeatModify();
void steroid_OverdriveCharge();
void vampire_HealthDrain();
void vampire_HealthModify();
void exVampire_HealthDrain();
void exVampire_HealthModify();
void onepunch_HealthModify();
void twopunch_HealthModify();
void fivepunch_HealthModify();
void tugofwar_HealthModify();
void infiniteheat_HeatModify();
void overdrive_FreezeOverdriveTimeleft();
void overdrive_KeepTimerGoing();

DWORD steroid_OverdriveChargeJmpBackAddr = 0;
DWORD steroid_HeatModifyJmpBackAddr = 0;
DWORD steroid_HealthModifyJmpBackAddr = 0;
DWORD vampire_HealthModifyJmpBackAddr = 0;
DWORD vampire_HealthDrainJmpBackAddr = 0;
DWORD exVampire_HealthModifyJmpBackAddr = 0;
DWORD exVampire_HealthDrainJmpBackAddr = 0;
DWORD onepunch_HealthModifyJmpBackAddr = 0;
DWORD twopunch_HealthModifyJmpBackAddr = 0;
DWORD fivepunch_HealthModifyJmpBackAddr = 0;
DWORD tugofwar_HealthModifyJmpBackAddr = 0;
DWORD infiniteheat_HeatModifyJmpBackAddr = 0;
DWORD overdrive_FreezeOverdriveTimeleftJmpBackAddr = 0;
DWORD overdrive_KeepTimerGoingJmpBackAddr = 0;

void __declspec(naked)steroid_OverdriveCharge()
{
#ifdef _DEBUG
	__asm pushad
	LOG(7, "steroid_OverdriveCharge\n");
	__asm popad
#endif

	static int addedOverdriveValue = 0;

	__asm
	{
		cmp esi, 7530h //do not change the value of 30000 if OD finisher hits
		je EXIT
		mov[addedOverdriveValue], esi
	}

	__asm pushad
	addedOverdriveValue *= steroid_overdriveChargeMultiplier;
	__asm popad

	__asm
	{
		mov esi, [addedOverdriveValue]
		EXIT:
		add[edi + 5AE4h], esi
			jmp[steroid_OverdriveChargeJmpBackAddr]
	}
}

void __declspec(naked)steroid_HeatModify()
{
#ifdef _DEBUG
	__asm pushad
	LOG(7, "steroid_HeatModify\n");
	__asm popad
#endif

	static int addedHeatValue = 0;
	_asm
	{
		mov[addedHeatValue], esi
		cmp esi, 0
		jl EXIT
		cmp esi, 1
		je LOW_HP_HEAT_GENERATION
		cmp esi, 2
		je HAKUMEN_AIR_HEAT_GENERATION
		cmp esi, 4
		je HAKUMEN_GROUND_HEAT_GENERATION
	}

	__asm pushad
	addedHeatValue *= steroid_heatModifyMultiplier;
	__asm popad

	__asm
	{
		jmp EXIT
		LOW_HP_HEAT_GENERATION :
	}

	__asm pushad
	addedHeatValue *= steroid_heatLowHPChargeMultiplier;
	__asm popad

	__asm
	{
		jmp EXIT
		HAKUMEN_AIR_HEAT_GENERATION :
		mov[addedHeatValue], 3
			jmp EXIT
	}

	__asm
	{
		jmp EXIT
		HAKUMEN_GROUND_HEAT_GENERATION :
		mov[addedHeatValue], 5
	}

	__asm
	{
	EXIT:
		mov esi, [addedHeatValue]
			add[ebx + 5AD4h], esi
			jmp[steroid_HeatModifyJmpBackAddr]
	}
}

void __declspec(naked)steroid_HealthModify()
{
#ifdef _DEBUG
	__asm pushad
	LOG(7, "steroid_HealthModify\n");
	__asm popad
#endif

	static int newHP = 0;
	static int previousHP = 0;
	static int maxHP = 0;

	__asm pushad
	__asm
	{
		mov[newHP], eax
		mov edx, [ecx + 9D0h] //previous hp addr
		mov[previousHP], edx
		mov edx, [ecx + 9D8h] //max hp addr
		mov[maxHP], edx
	}

	//if (newHP > previousHP) // we healed (like ragna for example)
	//	newHP += (newHP - previousHP) * steroid_hp_multiplier;
	if (newHP > maxHP)
		newHP = maxHP;
	__asm popad

	__asm
	{
		mov eax, [newHP]
		mov[ecx + 9D4h], eax //current hp in eax
		jmp[steroid_HealthModifyJmpBackAddr]
	}
}

void __declspec(naked)vampire_HealthModify()
{
	//handles the lifesteal for all chars
#ifdef _DEBUG
	__asm pushad
	LOG(7, "vampire_HealthModify\n");
	__asm popad
#endif

	static int newHP = 0;
	//static int previousHP = 0;
	static int opponentHealing = 0;
	static CChar* thisPlayerObj = 0;
	static CChar* otherPlayerObj = 0;

	__asm
	{
		pushad
		mov[newHP], eax
		//mov edx, [ecx + 9D0h] //previous hp addr
		//mov [previousHP], edx <- decided to simply use thisplayerObj instead
		mov[thisPlayerObj], ecx
	}

	if (*Containers::gameVals.pMatchState == MATCH_STATE_FIGHT && newHP < thisPlayerObj->previousHP) // only if we took damage. avoiding running on healing (like ragna for example)
	{
		//LOG(2, "Vampire: \n");
		if (thisPlayerObj != Containers::gameVals.P1CharObjPointer && thisPlayerObj != Containers::gameVals.P2CharObjPointer)
		{
			//LOG(2, "NEITHER PLAYER OBJECTS!\n");
			__asm
			{
				jmp EXIT
			}
		}

		opponentHealing = (thisPlayerObj->previousHP - newHP) * vampire_healing_percent;

		if (thisPlayerObj == Containers::gameVals.P1CharObjPointer)
		{
			//LOG(2, "\tP1 took %d damage\nHealing P2 with %d HP...:\n", (thisPlayerObj->previousHP - newHP), opponentHealing);
			otherPlayerObj = Containers::gameVals.P2CharObjPointer;
			//LOG(2, "\tP2 currentHP: %d\n", otherPlayerObj->currentHP);
			//LOG(2, "\tP2 maxHP: %d\n", otherPlayerObj->maxHP);
			//LOG(2, "\tP2 ")
		}
		else
		{
			//LOG(2, "\tP2 took %d damage\nHealing P1 with %d HP...:\n", (thisPlayerObj->previousHP - newHP), opponentHealing);
			otherPlayerObj = Containers::gameVals.P1CharObjPointer;
			//LOG(2, "\tP1 currentHP: %d\n", otherPlayerObj->currentHP);
			//LOG(2, "\tP1 maxHP: %d\n", otherPlayerObj->maxHP);
			//LOG(2, "\tP1 ");
		}

		//LOG(2, "Healing: ((previousHP (%d) - newHP (%d)) * vampire_healing(%.3f)) = %d\n", thisPlayerObj->previousHP, newHP, vampire_healing_percent, opponentHealing);
		//if (thisPlayerObj->previousHP > thisPlayerObj->maxHP)
		//	LOG(2, "\tERROR, PREVIOUS HP TOO HIGH!\n");

		otherPlayerObj->currentHP += opponentHealing;
		//LOG(2, "\tafter healing: %d\n", otherPlayerObj->currentHP);
		if (otherPlayerObj->currentHP > otherPlayerObj->maxHP)
			otherPlayerObj->currentHP = otherPlayerObj->maxHP;

		//LOG(2, "\tsetting previousHP from: %d\n", otherPlayerObj->previousHP);
		otherPlayerObj->previousHP = otherPlayerObj->currentHP;
		thisPlayerObj->previousHP = newHP;
		//LOG(2, "\tsetting previousHP to: %d\n", otherPlayerObj->previousHP);
	}

	__asm
	{
	EXIT:
		popad
			mov[ecx + 9D4h], eax //current hp in eax
			jmp[vampire_HealthModifyJmpBackAddr]
	}
}

void __declspec(naked)vampire_HealthDrain()
{
	//handles the % hp loss per second
#ifdef _DEBUG
	__asm pushad
	LOG(7, "vampire_HealthDrain\n");
	__asm popad
#endif

	static float vampirism_timer = 0.0;
	static int previous_real_timer = 0;

	__asm pushad

	if (*Containers::gameVals.pMatchState == MATCH_STATE_FIGHT) //on match only
	{
		if (*Containers::gameVals.pGameMode == GAME_MODE_TRAINING || *Containers::gameVals.pMatchTimer == 5939) //if we are in training mode, calculate it a different way, since we have no match timer
		{
			//do not reduce hp while distortions / od finishers / astrals are happening:
			if (!Containers::gameVals.P1CharObjPointer->is_doing_distortion &&
				!Containers::gameVals.P2CharObjPointer->is_doing_distortion)
				vampirism_timer += ImGui::GetIO().DeltaTime;
		}
		else
		{
			//uninitialized value or new match started:
			if (previous_real_timer == 0 || previous_real_timer < *Containers::gameVals.pMatchTimer)
				previous_real_timer = *Containers::gameVals.pMatchTimer;

			vampirism_timer += (previous_real_timer - *Containers::gameVals.pMatchTimer);
			previous_real_timer = *Containers::gameVals.pMatchTimer;
		}

		if (vampirism_timer > 60) // 1 sec
		{
			//LOG(2, "!Vampire taking 1sec damage\n");
			int P1_lostHP = Containers::gameVals.P1CharObjPointer->maxHP * vampire_health_loss_percent_per_sec;
			int P2_lostHP = Containers::gameVals.P2CharObjPointer->maxHP * vampire_health_loss_percent_per_sec;
			Containers::gameVals.P1CharObjPointer->currentHP -= P1_lostHP;
			Containers::gameVals.P2CharObjPointer->currentHP -= P2_lostHP;
			if (Containers::gameVals.P1CharObjPointer->currentHP < 1) //prevent dying of this effect
				Containers::gameVals.P1CharObjPointer->currentHP = 1;
			if (Containers::gameVals.P2CharObjPointer->currentHP < 1) //prevent dying of this effect
				Containers::gameVals.P2CharObjPointer->currentHP = 1;

			Containers::gameVals.P1CharObjPointer->previousHP = Containers::gameVals.P1CharObjPointer->currentHP;
			Containers::gameVals.P2CharObjPointer->previousHP = Containers::gameVals.P2CharObjPointer->currentHP;

			vampirism_timer = 0.0;
		}
	}
	__asm popad

	__asm
	{
		mov ebx, [esi + 9D4h]
		jmp[vampire_HealthDrainJmpBackAddr]
	}
}

void __declspec(naked)exVampire_HealthModify()
{
	//handles the lifesteal for all chars
#ifdef _DEBUG
	__asm pushad
	LOG(7, "exVampire_HealthModify\n");
	__asm popad
#endif

	static int newHP = 0;
	//static int previousHP = 0;
	static int opponentHealing = 0;
	static CChar* thisPlayerObj = 0;
	static CChar* otherPlayerObj = 0;

	__asm
	{
		pushad
		mov[newHP], eax
		//mov edx, [ecx + 9D0h] //previous hp addr
		//mov [previousHP], edx <- decided to simply use thisplayerObj instead
		mov[thisPlayerObj], ecx
	}

	if (*Containers::gameVals.pMatchState == MATCH_STATE_FIGHT && newHP < thisPlayerObj->previousHP) // only if we took damage. avoiding running on healing (like ragna for example)
	{
		//LOG(2, "exVampire: \n");
		if (thisPlayerObj != Containers::gameVals.P1CharObjPointer && thisPlayerObj != Containers::gameVals.P2CharObjPointer)
		{
			//LOG(2, "NEITHER PLAYER OBJECTS!\n");
			__asm
			{
				jmp EXIT
			}
		}

		opponentHealing = (thisPlayerObj->previousHP - newHP) * exVampire_healing_percent;

		if (thisPlayerObj == Containers::gameVals.P1CharObjPointer)
		{
			//LOG(2, "\tP1 took %d damage\nHealing P2 with %d HP...:\n", (thisPlayerObj->previousHP - newHP), opponentHealing);
			otherPlayerObj = Containers::gameVals.P2CharObjPointer;
			//LOG(2, "\tP2 currentHP: %d\n", otherPlayerObj->currentHP);
			//LOG(2, "\tP2 maxHP: %d\n", otherPlayerObj->maxHP);
			//LOG(2, "\tP2 ")
		}
		else
		{
			//LOG(2, "\tP2 took %d damage\nHealing P1 with %d HP...:\n", (thisPlayerObj->previousHP - newHP), opponentHealing);
			otherPlayerObj = Containers::gameVals.P1CharObjPointer;
			//LOG(2, "\tP1 currentHP: %d\n", otherPlayerObj->currentHP);
			//LOG(2, "\tP1 maxHP: %d\n", otherPlayerObj->maxHP);
			//LOG(2, "\tP1 ");
		}

		//LOG(2, "Healing: ((previousHP (%d) - newHP (%d)) * exVampire_healing(%.3f)) = %d\n", thisPlayerObj->previousHP, newHP, exVampire_healing_percent, opponentHealing);
		//if (thisPlayerObj->previousHP > thisPlayerObj->maxHP)
		//	LOG(2, "\tERROR, PREVIOUS HP TOO HIGH!\n");

		otherPlayerObj->currentHP += opponentHealing;
		//LOG(2, "\tafter healing: %d\n", otherPlayerObj->currentHP);
		if (otherPlayerObj->currentHP > otherPlayerObj->maxHP)
			otherPlayerObj->maxHP = otherPlayerObj->currentHP;

		//LOG(2, "\tsetting previousHP from: %d\n", otherPlayerObj->previousHP);
		otherPlayerObj->previousHP = otherPlayerObj->currentHP;
		thisPlayerObj->previousHP = newHP;
		//LOG(2, "\tsetting previousHP to: %d\n", otherPlayerObj->previousHP);
	}

	__asm
	{
	EXIT:
		popad
			mov[ecx + 9D4h], eax //current hp in eax
			jmp[exVampire_HealthModifyJmpBackAddr]
	}
}

void __declspec(naked)exVampire_HealthDrain()
{
	//handles the % hp loss per second
#ifdef _DEBUG
	__asm pushad
	LOG(7, "exVampire_HealthDrain\n");
	__asm popad
#endif

	static float vampirism_timer = 0.0;
	static int previous_real_timer = 0;

	__asm pushad

	if (*Containers::gameVals.pMatchState == MATCH_STATE_FIGHT) //on match only
	{
		if (*Containers::gameVals.pGameMode == GAME_MODE_TRAINING || *Containers::gameVals.pMatchTimer == 5939) //if we are in training mode, calculate it a different way, since we have no match timer. pMatchTimer is set to 5939 when infinite timer
		{
			//do not reduce hp while distortions / od finishers / astrals are happening:
			if (!Containers::gameVals.P1CharObjPointer->is_doing_distortion &&
				!Containers::gameVals.P2CharObjPointer->is_doing_distortion)
				vampirism_timer += ImGui::GetIO().DeltaTime;
		}
		else
		{
			//uninitialized value or new match started:
			if (previous_real_timer == 0 || previous_real_timer < *Containers::gameVals.pMatchTimer)
				previous_real_timer = *Containers::gameVals.pMatchTimer;

			vampirism_timer += (previous_real_timer - *Containers::gameVals.pMatchTimer);
			previous_real_timer = *Containers::gameVals.pMatchTimer;
		}

		if (vampirism_timer > 60) // 1 sec
		{
			//LOG(2, "!exVampire taking 1sec damage\n");
			int P1_lostHP = Containers::gameVals.P1CharObjPointer->maxHP * exVampire_health_loss_percent_per_sec;
			int P2_lostHP = Containers::gameVals.P2CharObjPointer->maxHP * exVampire_health_loss_percent_per_sec;
			Containers::gameVals.P1CharObjPointer->currentHP -= P1_lostHP;
			Containers::gameVals.P2CharObjPointer->currentHP -= P2_lostHP;
			if (Containers::gameVals.P1CharObjPointer->currentHP < 1) //prevent dying of this effect
				Containers::gameVals.P1CharObjPointer->currentHP = 1;
			if (Containers::gameVals.P2CharObjPointer->currentHP < 1) //prevent dying of this effect
				Containers::gameVals.P2CharObjPointer->currentHP = 1;

			Containers::gameVals.P1CharObjPointer->previousHP = Containers::gameVals.P1CharObjPointer->currentHP;
			Containers::gameVals.P2CharObjPointer->previousHP = Containers::gameVals.P2CharObjPointer->currentHP;

			vampirism_timer = 0.0;
		}
	}
	__asm popad

	__asm
	{
		mov ebx, [esi + 9D4h]
		jmp[exVampire_HealthDrainJmpBackAddr]
	}
}

void __declspec(naked)onepunch_HealthModify()
{
#ifdef _DEBUG
	__asm pushad
	LOG(7, "onepunch_HealthModify\n");
	__asm popad
#endif

	static int newHP = 0;

	__asm mov[newHP], eax
	__asm pushad
	if (*Containers::gameVals.pMatchState == MATCH_STATE_FIGHT) //on match only
		newHP = 0;
	__asm popad

	__asm
	{
		mov eax, [newHP]
		mov[ecx + 9D4h], eax //current hp in eax
		jmp[onepunch_HealthModifyJmpBackAddr]
	}
}

void __declspec(naked)twopunch_HealthModify()
{
#ifdef _DEBUG
	__asm pushad
	LOG(7, "twopunch_HealthModify\n");
	__asm popad
#endif

	static int newHP = 0;
	static CChar* thisPlayerObj = 0;

	__asm
	{
		mov[newHP], eax
		mov[thisPlayerObj], ecx
		pushad
	}

	if (*Containers::gameVals.pMatchState == MATCH_STATE_FIGHT) //on match only
	{
		if (thisPlayerObj != Containers::gameVals.P1CharObjPointer && thisPlayerObj != Containers::gameVals.P2CharObjPointer)
		{
			//LOG(2, "NEITHER PLAYER OBJECTS!\n");
			__asm jmp EXIT
		}
		if (newHP > thisPlayerObj->previousHP) // we healed, ignore it.
			newHP = thisPlayerObj->previousHP;
		else //took damage
		{
			newHP = thisPlayerObj->previousHP; //we want to do exactly 50% max hp damage
			newHP -= thisPlayerObj->maxHP / 2;
			thisPlayerObj->previousHP = newHP;
		}
	}

	__asm
	{
	EXIT:
		popad
			mov eax, [newHP]
			mov[ecx + 9D4h], eax //current hp in eax
			jmp[twopunch_HealthModifyJmpBackAddr]
	}
}

void __declspec(naked)fivepunch_HealthModify()
{
#ifdef _DEBUG
	__asm pushad
	LOG(7, "fivepunch_HealthModify\n");
	__asm popad
#endif

	static int newHP = 0;
	static CChar* thisPlayerObj = 0;

	__asm
	{
		mov[newHP], eax
		mov[thisPlayerObj], ecx
		pushad
	}

	if (*Containers::gameVals.pMatchState == MATCH_STATE_FIGHT) //on match only
	{
		if (thisPlayerObj != Containers::gameVals.P1CharObjPointer && thisPlayerObj != Containers::gameVals.P2CharObjPointer)
		{
			//LOG(2, "NEITHER PLAYER OBJECTS!\n");
			__asm jmp EXIT
		}
		if (newHP > thisPlayerObj->previousHP) // we healed, ignore it.
			newHP = thisPlayerObj->previousHP;
		else //took damage
		{
			newHP = thisPlayerObj->previousHP; //we want to do exactly 20% max hp damage
			newHP -= thisPlayerObj->maxHP / 5;
			thisPlayerObj->previousHP = newHP;
		}
	}

	__asm
	{
	EXIT:
		popad
			mov eax, [newHP]
			mov[ecx + 9D4h], eax //current hp in eax
			jmp[fivepunch_HealthModifyJmpBackAddr]
	}
}

void __declspec(naked)tugofwar_HealthModify()
{
#ifdef _DEBUG
	__asm pushad
	LOG(7, "tugofwar_HealthModify\n");
	__asm popad
#endif

	static int newHP = 0;
	static CChar* thisPlayerObj = 0;
	static CChar* otherPlayerObj = 0;

	__asm
	{
		mov[newHP], eax
		mov[thisPlayerObj], ecx
		pushad
	}

	if (*Containers::gameVals.pMatchState == MATCH_STATE_FIGHT) //on match only
	{
		if (thisPlayerObj != Containers::gameVals.P1CharObjPointer && thisPlayerObj != Containers::gameVals.P2CharObjPointer)
		{
			//LOG(2, "NEITHER PLAYER OBJECTS!\n");
			__asm jmp EXIT
		}
		if (newHP > thisPlayerObj->previousHP) // we healed, ignore it.
			newHP = thisPlayerObj->previousHP;
		else //took damage
		{
			newHP = thisPlayerObj->previousHP; //we want to do exactly 20% max hp damage
			newHP -= thisPlayerObj->maxHP / 10;

			if (thisPlayerObj == Containers::gameVals.P1CharObjPointer)
				otherPlayerObj = Containers::gameVals.P2CharObjPointer;
			else
				otherPlayerObj = Containers::gameVals.P1CharObjPointer;

			//heal the other player
			otherPlayerObj->currentHP += otherPlayerObj->maxHP / 10;
			//prevent healing past maxhp
			if (otherPlayerObj->currentHP > otherPlayerObj->maxHP)
				otherPlayerObj->maxHP = otherPlayerObj->currentHP;

			otherPlayerObj->previousHP = otherPlayerObj->currentHP;
		}

		thisPlayerObj->previousHP = newHP;

	}

	__asm
	{
	EXIT:
		popad
			mov eax, [newHP]
			mov[ecx + 9D4h], eax //current hp in eax
			jmp[tugofwar_HealthModifyJmpBackAddr]
	}
}

void __declspec(naked)infiniteheat_HeatModify()
{
#ifdef _DEBUG
	__asm pushad
	LOG(7, "infiniteheat_HeatModify\n");
	__asm popad
#endif

	__asm
	{
		mov esi, 0
		add[ebx + 5AD4h], esi
		jmp[infiniteheat_HeatModifyJmpBackAddr]
	}
}

void __declspec(naked)overdrive_FreezeOverdriveTimeleft()
{
#ifdef _DEBUG
	__asm pushad
	LOG(7, "overdrive_FreezeOverdriveTimeleft\n");
	__asm popad
#endif

	__asm
	{
		//dec dword ptr[ebx + 1C0h]
		jmp[overdrive_FreezeOverdriveTimeleftJmpBackAddr]
	}
}

void __declspec(naked)overdrive_KeepTimerGoing()
{
#ifdef _DEBUG
	__asm pushad
	LOG(7, "overdrive_KeepTimerGoing\n");
	__asm popad
#endif
	__asm
	{
		nop
		jmp[overdrive_KeepTimerGoingJmpBackAddr]
	}
}

void CustomGameModeHooks()
{
	LOG(2, "CustomGameModeHooks\n");

	steroid_OverdriveChargeJmpBackAddr = HookManager::SetHook(
		"steroid_OverdriveCharge",
		"\x01\xb7\xe4\x5a\x00\x00\x8b\x87\xe4\x5a\x00\x00",
		"xxxxxxxxxxxx",
		6,
		steroid_OverdriveCharge,
		false);

	steroid_HeatModifyJmpBackAddr = HookManager::SetHook(
		"steroid_HeatModify",
		"\x01\xb3\xd4\x5a\x00\x00\x8b\x83\xd4\x5a\x00\x00",
		"xxxxxxxxxxxx",
		6,
		steroid_HeatModify,
		false);

	steroid_HealthModifyJmpBackAddr = HookManager::SetHook(
		"steroid_HealthModify",
		"\x89\x81\xd4\x09\x00\x00\x5d",
		"xxxxxxx",
		6,
		steroid_HealthModify,
		false);

	vampire_HealthModifyJmpBackAddr = HookManager::SetHook(
		"vampire_HealthModify",
		"\x89\x81\xd4\x09\x00\x00\x5d",
		"xxxxxxx",
		6,
		vampire_HealthModify,
		false);

	vampire_HealthDrainJmpBackAddr = HookManager::SetHook(
		"vampire_HealthDrain",
		"\x8b\x9e\xd4\x09\x00\x00\x6b\xc9\x64",
		"xxxxxxxxx",
		6,
		vampire_HealthDrain,
		false);

	exVampire_HealthModifyJmpBackAddr = HookManager::SetHook(
		"exVampire_HealthModify",
		"\x89\x81\xd4\x09\x00\x00\x5d",
		"xxxxxxx",
		6,
		exVampire_HealthModify,
		false);

	exVampire_HealthDrainJmpBackAddr = HookManager::SetHook(
		"exVampire_HealthDrain",
		"\x8b\x9e\xd4\x09\x00\x00\x6b\xc9\x64",
		"xxxxxxxxx",
		6,
		exVampire_HealthDrain,
		false);

	onepunch_HealthModifyJmpBackAddr = HookManager::SetHook(
		"onepunch_HealthModify",
		"\x89\x81\xd4\x09\x00\x00\x5d",
		"xxxxxxx",
		6,
		onepunch_HealthModify,
		false);

	twopunch_HealthModifyJmpBackAddr = HookManager::SetHook(
		"twopunch_HealthModify",
		"\x89\x81\xd4\x09\x00\x00\x5d",
		"xxxxxxx", 6,
		twopunch_HealthModify,
		false);

	fivepunch_HealthModifyJmpBackAddr = HookManager::SetHook(
		"fivepunch_HealthModify",
		"\x89\x81\xd4\x09\x00\x00\x5d",
		"xxxxxxx", 6,
		fivepunch_HealthModify,
		false);

	tugofwar_HealthModifyJmpBackAddr = HookManager::SetHook(
		"tugofwar_HealthModify",
		"\x89\x81\xd4\x09\x00\x00\x5d",
		"xxxxxxx", 6,
		tugofwar_HealthModify,
		false);

	infiniteheat_HeatModifyJmpBackAddr = HookManager::SetHook(
		"infiniteheat_HeatModify",
		"\x01\xb3\xd4\x5a\x00\x00\x8b\x83\xd4\x5a\x00\x00",
		"xxxxxxxxxxxx", 6,
		infiniteheat_HeatModify,
		false);

	overdrive_FreezeOverdriveTimeleftJmpBackAddr = HookManager::SetHook(
		"overdrive_FreezeOverdriveTimeleft",
		"\xff\x8b\xc0\x01\x00\x00\x83\xbb\xc0\x01\x00\x00\x00",
		"xxxxxxxxxxxxx", 6,
		overdrive_FreezeOverdriveTimeleft,
		false);

	overdrive_KeepTimerGoingJmpBackAddr = HookManager::SetHook(
		"overdrive_KeepTimerGoing",
		"\x39\x45\xcc\x0f\x84",
		"xxxxx",
		9,
		overdrive_KeepTimerGoing,
		false);

	InitCustomGameModesVector();
}