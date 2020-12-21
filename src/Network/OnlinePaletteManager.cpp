#include "OnlinePaletteManager.h"

#include "Palette/impl_format.h"

#include "Core/logger.h"

OnlinePaletteManager::OnlinePaletteManager(PaletteManager* pPaletteManager, CharPaletteHandle* pP1CharPalHandle,
	CharPaletteHandle* pP2CharPalHandle, RoomManager* pRoomManager)
	: m_pPaletteManager(pPaletteManager), m_pP1CharPalHandle(pP1CharPalHandle), 
	m_pP2CharPalHandle(pP2CharPalHandle), m_pRoomManager(pRoomManager)
{
}

void OnlinePaletteManager::SendPalettePacket()
{
	LOG(2, "OnlinePaletteManager::SendPalettePacket\n");

	if (m_pRoomManager->IsThisPlayerSpectator())
		return;

	uint16_t thisPlayerMatchPlayerIndex = m_pRoomManager->GetThisPlayerMatchPlayerIndex();
	CharPaletteHandle* pCharPalHandle = thisPlayerMatchPlayerIndex == 0 ? m_pP1CharPalHandle : m_pP2CharPalHandle;

	for (int palFileIndex = 0; palFileIndex < IMPL_PALETTE_FILES_COUNT; palFileIndex++)
	{
		const char* palAddr = m_pPaletteManager->GetCurPalFileAddr((PaletteFile)palFileIndex, *pCharPalHandle);

		Packet packet = Packet((char*)palAddr, (uint16_t)IMPL_PALETTE_DATALEN, PacketType_Palette, thisPlayerMatchPlayerIndex, palFileIndex);

		m_pRoomManager->SendPacketToSameMatchIMPlayers(&packet);
	}
}

void OnlinePaletteManager::RecvPalettePacket(Packet* packet)
{
	LOG(2, "OnlinePaletteManager::RecvPalettePacket\n");

	uint16_t matchPlayerIndex = m_pRoomManager->GetPlayerMatchPlayerIndexByRoomMemberIndex(packet->roomMemberIndex);
	CharPaletteHandle* pCharPalHandle = matchPlayerIndex == 0 ? m_pP1CharPalHandle : m_pP2CharPalHandle;

	if (pCharPalHandle->IsNullPointerPalBasePtr())
	{
		m_unprocessedPalettesFiles.push(UnprocessedPaletteFile(matchPlayerIndex, (PaletteFile)packet->part, (char*)packet->data));
		return;
	}

	m_pPaletteManager->ReplacePaletteFile((const char*)packet->data, (PaletteFile)packet->part, *pCharPalHandle);
}

void OnlinePaletteManager::ProcessSavedPaletteFiles()
{
	LOG(2, "OnlinePaletteManager::ProcessSavedPaletteFiles\n");

	if (!m_pRoomManager->IsRoomFunctional())
		return;

	for (int i = 0; i < m_unprocessedPalettesFiles.size(); i++)
	{
		UnprocessedPaletteFile& palfile = m_unprocessedPalettesFiles.front();

		CharPaletteHandle* pCharPalHandle = palfile.matchPlayerIndex == 0 ? m_pP1CharPalHandle : m_pP2CharPalHandle;

		m_pPaletteManager->ReplacePaletteFile(palfile.palData, palfile.palFile, *pCharPalHandle);

		m_unprocessedPalettesFiles.pop();
	}
}

void OnlinePaletteManager::ClearSavedPalettesQueue()
{
	LOG(2, "OnlinePaletteManager::ClearSavedPalettesQueue\n");

	m_unprocessedPalettesFiles = {};
}

void OnlinePaletteManager::OnMatchInit()
{
	LOG(2, "OnlinePaletteManager::OnMatchInit\n");

	SendPalettePacket();
	ProcessSavedPaletteFiles();
}