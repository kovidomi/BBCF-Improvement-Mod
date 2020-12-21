#pragma once

#include "RoomManager.h"

#include "Palette/PaletteManager.h"

#include <queue>

class OnlinePaletteManager
{
public:
	OnlinePaletteManager(PaletteManager* pPaletteManager, CharPaletteHandle* pP1CharPalHandle,
		CharPaletteHandle* pP2CharPalHandle, RoomManager* pRoomManager);
	void SendPalettePacket();
	void RecvPalettePacket(Packet* packet);
	void ProcessSavedPaletteFiles();
	void ClearSavedPalettesQueue();
	void OnMatchInit();

private:
	struct UnprocessedPaletteFile
	{
		uint16_t matchPlayerIndex;
		PaletteFile palFile;
		char palData[IMPL_PALETTE_DATALEN];

		UnprocessedPaletteFile(uint16_t matchPlayerIndex, PaletteFile palFile, char* pPalSrc)
			: matchPlayerIndex(matchPlayerIndex), palFile(palFile)
		{
			memcpy_s(palData, IMPL_PALETTE_DATALEN, pPalSrc, IMPL_PALETTE_DATALEN);
		}
	};

	std::queue<UnprocessedPaletteFile> m_unprocessedPalettesFiles;

	CharPaletteHandle* m_pP1CharPalHandle;
	CharPaletteHandle* m_pP2CharPalHandle;

	// Interfaces
	PaletteManager* m_pPaletteManager;
	RoomManager* m_pRoomManager;
};