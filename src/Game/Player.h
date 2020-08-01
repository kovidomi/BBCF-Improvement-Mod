#pragma once
#include "CharData.h"
#include "CharPaletteHandle.h"

class Player
{
public:
	const CharData* GetData() const;
	CharPaletteHandle& GetPalHandle();

	void SetCharDataPtr(const void* addr);
	bool IsCharDataNullPtr() const;

private:
	CharData** m_charData;
	CharPaletteHandle m_charPalHandle;
};
