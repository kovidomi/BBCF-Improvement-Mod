#pragma once
#include <cstdint>
// Generated using ReClass 2015

class CharData
{
public:
	class OBJ_CCharBase* obj_ccharbase; //0x0000
	char pad_0004[20]; //0x0004
	int32_t stateChangedCount; //0x0018
	char pad_001C[24]; //0x001C
	int32_t charIndex; //0x0034
	char pad_0038[4]; //0x0038
	class AA_CCollision_JON* aa_collision_jon; //0x003C
	char pad_0040[12]; //0x0040
	class JonbEntry* pJonbEntryBegin; //0x004C
	char pad_0050[68]; //0x0050
	int32_t hurtboxCount; //0x0094
	int32_t hitboxCount; //0x0098
	char pad_009C[60]; //0x009C
	char* currentSprite; //0x00D8
	char pad_00DC[208]; //0x00DC
	int32_t lambda_nu_drive_hitcount; //0x01AC
	char pad_01B0[16]; //0x01B0
	int32_t overdriveTimeleft; //0x01C0
	int32_t overdriveTimerStartedAt; //0x01C4
	char pad_01C8[20]; //0x01C8
	int32_t some_distortion_timer; //0x01DC
	int32_t isDoingDistortion; //0x01E0
	char pad_01E4[676]; //0x01E4
	int32_t heatUsageType; //0x0488
	char pad_048C[1348]; //0x048C
	int32_t previousHP; //0x09D0
	int32_t currentHP; //0x09D4
	int32_t maxHP; //0x09D8
	char pad_09DC[5656]; //0x09DC
	char lastAction[20]; //0x1FF4
	char pad_2008[104]; //0x2008
	char currentAction[20]; //0x2070
	char pad_2084[452]; //0x2084
	char charAbbr[4]; //0x2248
	char pad_224C[14472]; //0x224C
	int32_t heatMeter; //0x5AD4
	char pad_5AD8[4]; //0x5AD8
	int32_t heatGainCooldown; //0x5ADC
	char pad_5AE0[4]; //0x5AE0
	int32_t overdriveMeter; //0x5AE4
	char pad_5AE8[16]; //0x5AE8
	int32_t maxOverdriveTime; //0x5AF8
	char pad_5AFC[8]; //0x5AFC
	int32_t barrierMeter; //0x5B04
	char pad_5B08[108036]; //0x5B08
	int32_t Drive1; //0x2010C
	char pad_20110[12]; //0x20110
	int32_t Drive1_type; //0x2011C
	char pad_20120[16]; //0x20120
	int32_t Drive2; //0x20130
	char pad_20134[32]; //0x20134
	int32_t Drive3; //0x20154
	char pad_20158[4972]; //0x20158
}; //Size: 0x214C4
