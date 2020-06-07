#pragma once

#include "Network/network_manager.h"

#include <string>

#define PALETTE_DATALEN 1020
#define HPAL_HEADER_LEN 0x24

extern char HPAL_header[HPAL_HEADER_LEN];
extern DWORD* P1CharSelectPalIndex;
extern DWORD* P2CharSelectPalIndex;

void DereferenceP1P2PalettePointers();
//fileID 0 to get sprite palette, fileID 1 to get first effect file, fileID 2 to get second effect file etc...
DWORD* getPaletteArray(DWORD* base, DWORD palIndex, DWORD fileID);
DWORD* getCharSelPaletteArray(DWORD* base, DWORD pid, bool isP1);
void DereferencePalettePointer1(DWORD* palettePointer, DWORD paletteIndex);
void DereferencePalettePointer2(DWORD* palettePointer, DWORD paletteIndex);
void DEBUGDereferencePalettePointer1(DWORD* palettePointer, DWORD paletteIndex);
void HandleSavedPackets();
void LoadPalettePacket(im_packet_internal_t* packet);
void LoadEffectPacket(im_packet_internal_t* packet);
void LoadBloomPacket(im_packet_internal_t* packet);
void SendCustomEffectBloom();
void OverwriteStagesList();
void SendCustomPalette();
void SendCustomEffects();
void SendCustomDatas();
void CreateCustomFolders();
void CreatePaletteDirectories();
void InitCustomPaletteVector();
void LoadPaletteFiles();
void LoadCustomPalettesIntoContainer(int charIndex, LPCWSTR path);
int FindCustomPaletteIndex(std::string paletteName, int charID);
void ReloadCustomPalettes();
void ReplaceP1Palette(bool sendToOpponent = true);
void ReplaceP2Palette(bool sendToOpponent = true);
void ReplacePaletteInMemory(DWORD* Dst, const void* Src);
void SaveDefaultP1P2Pals();
void ResettingDefaultPalettes();
void BackupP1PaletteEffects();
void BackupP2PaletteEffects();
void ReplaceP1PaletteEffects();
void ReplaceP2PaletteEffects();
void ReplaceP1Palette_PaletteEditor(char* pPaletteData, int file);
void ReplaceP2Palette_PaletteEditor(char* pPaletteData, int file);
