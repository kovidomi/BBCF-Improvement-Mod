#include "PaletteManager.h"

#include "Core/logger.h"
#include "Core/utils.h"
#include "Game/characters.h"
#include "Overlay/Logger/ImGuiLogger.h"
#include "custom_palette.h"

#include <atlstr.h>

#define MAX_NUM_OF_PAL_SLOTS 20

PaletteManager::PaletteManager()
{
	LOG(2, "PaletteManager::PaletteManager\n");

	LoadPaletteFiles();
	CreatePaletteFolders();
	//CreatePaletteSlotsFile();
}

PaletteManager::~PaletteManager()
{
}

void PaletteManager::CreatePaletteFolders()
{
	LOG(2, "CreatePaletteFolders\n");

	CreateDirectory(L"BBCF_IM\\Download", NULL);
	CreateDirectory(L"BBCF_IM\\Palettes", NULL);

	for (int i = 0; i < getCharactersCount(); i++)
	{
		std::wstring path = std::wstring(L"BBCF_IM\\Palettes\\") + getCharacterNameByIndexW(i);
		CreateDirectory(path.c_str(), NULL);
	}
}

void PaletteManager::InitCustomPaletteVector()
{
	LOG(2, "InitCustomPaletteVector\n");

	m_customPalettes.clear();
	m_customPalettes.resize(getCharactersCount());

	for (int i = 0; i < getCharactersCount(); i++)
	{
		//make the character palette array's 0th element an empty one, that will be used to set back to the default palette
		IMPL_data_t customPal { "Default" };
		m_customPalettes[i].push_back(customPal);
	}
}

void PaletteManager::LoadPalettesFromFolder()
{
	InitCustomPaletteVector();

	LOG(2, "LoadPaletteFiles\n");
	g_imGuiLogger->Log("[system] Loading local custom palettes...\n");

	for (int i = 0; i < getCharactersCount(); i++)
	{
		std::wstring wPath = std::wstring(L"BBCF_IM\\Palettes\\") + getCharacterNameByIndexW(i) + L"\\*";
		LoadPalettesIntoVector((CharIndex)i, wPath);
	}

	InitOnlinePalsIndexVector();

	g_imGuiLogger->Log("[system] Finished loading local custom palettes\n");
}

void PaletteManager::InitOnlinePalsIndexVector()
{
	m_onlinePalsStartIndex.clear();

	for (int i = 0; i < getCharactersCount(); i++)
	{
		m_onlinePalsStartIndex.push_back(m_customPalettes[i].size());
	}
}

void PaletteManager::ApplyDefaultCustomPalette(CharIndex charIndex, CharPaletteHandle & charPalHandle)
{
	LOG(2, "ApplyDefaultCustomPalette\n");

	if (charIndex > getCharactersCount())
		return;

	const int curPalIndex = charPalHandle.GetOrigPalIndex();
	const char* palName = m_paletteSlots[charIndex][curPalIndex].c_str();

	if (strcmp(palName, "") == 0 || strcmp(palName, "Default") == 0)
		return;

	int foundCustomPalIndex = 0;

	if (strcmp(palName, "Random") == 0)
		foundCustomPalIndex = rand() % m_customPalettes[charIndex].size();
	else
		foundCustomPalIndex = FindCustomPalIndex(charIndex, palName);

	if (foundCustomPalIndex < 0)
	{
		g_imGuiLogger->Log("[error] Palette file '%s' cannot be set as default: File not found.\n", palName);
		return;
	}

	SwitchPalette(charIndex, charPalHandle, foundCustomPalIndex);
}

void PaletteManager::LoadPalettesIntoVector(CharIndex charIndex, std::wstring& wFolderPath)
{
	std::string folderPath(wFolderPath.begin(), wFolderPath.end());
	LOG(2, "LoadPalettesIntoContainer %s\n", folderPath);

	HANDLE hFind;
	WIN32_FIND_DATA data;

	hFind = FindFirstFile(wFolderPath.c_str(), &data);
	if (hFind != INVALID_HANDLE_VALUE) {
		do {

			//ignore current and parent directories
			if (_tcscmp(data.cFileName, TEXT(".")) == 0 || _tcscmp(data.cFileName, TEXT("..")) == 0)
				continue;
			//recursively search subfolders
			if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				std::wstring wSubfolderPath(wFolderPath.c_str());
				wSubfolderPath.pop_back(); //delete "*" at the end
				wSubfolderPath += data.cFileName;
				wSubfolderPath += L"\\*";
				LoadPalettesIntoVector(charIndex, wSubfolderPath);
				continue;
			}

			std::wstring wFileName(data.cFileName);
			std::string fileName(wFileName.begin(), wFileName.end());
			std::string fullPath(folderPath);
			fullPath.pop_back(); //delete "*" at the end
			fullPath += fileName;

			LOG(2, "\tFILE: %s", fileName.c_str());
			LOG(2, "\t\tFull path: %s\n", fullPath.c_str());

			if (fileName.find(".impl") == std::string::npos)
			{
				g_imGuiLogger->Log("[error] Unable to open '%s' : not an .impl file\n", fileName.c_str());
				continue;
			}

			IMPL_t fileContents;
			if(!utils_ReadFile(fullPath.c_str(), &fileContents, sizeof(IMPL_t), true))
			{
				LOG(2, "\tCouldn't open %s!\n", strerror(errno));
				g_imGuiLogger->Log("[error] Unable to open '%s' : %s\n", fileName.c_str(), strerror(errno));
				continue;
			}

			//check for errors
			if (strcmp(fileContents.header.filesig, "IMPL") != 0)
			{
				LOG(2, "ERROR, unrecognized file format!\n");
				g_imGuiLogger->Log("[error] '%s' unrecognized file format!\n", fileName.c_str());
				continue;
			}

			if (fileContents.header.datalen != sizeof(IMPL_data_t))
			{
				LOG(2, "ERROR, data size mismatch!\n");
				g_imGuiLogger->Log("[error] '%s' data size mismatch!\n", fileName.c_str());
				continue;
			}

			if (isCharacterIndexOutOfBound(charIndex))
			{
				LOG(2, "ERROR, '%s' has invalid character index in the header\n", fileName.c_str());
				g_imGuiLogger->Log("[error] '%s' has invalid character index in the header\n", fileName.c_str());
			}
			else if (charIndex != fileContents.header.charindex)
			{
				LOG(2, "ERROR, '%s' belongs to character %s, but is placed in folder %s\n",
					fileName.c_str(), getCharacterNameByIndexA(fileContents.header.charindex).c_str(),
					getCharacterNameByIndexA(charIndex).c_str());

				g_imGuiLogger->Log("[error] '%s' belongs to character '%s', but is placed in folder '%s'\n", 
					fileName.c_str(), getCharacterNameByIndexA(fileContents.header.charindex).c_str(),
					getCharacterNameByIndexA(charIndex).c_str());
			}
			else
			{
				OverwriteIMPLDataPalName(fileName, fileContents.paldata);
				PushImplFileIntoVector(charIndex, fileContents.paldata);
			}

		} while (FindNextFile(hFind, &data));
		FindClose(hFind);
	}
}

void PaletteManager::LoadPaletteSettingsFile()
{
	InitPaletteSlotsVector();

	LOG(2, "LoadPaletteSettingsFile\n");

	TCHAR pathBuf[MAX_PATH];
	GetModuleFileName(NULL, pathBuf, MAX_PATH);
	std::wstring::size_type pos = std::wstring(pathBuf).find_last_of(L"\\");
	std::wstring wFullPath = std::wstring(pathBuf).substr(0, pos);

	wFullPath += L"\\palettes.ini";

	if (!PathFileExists(wFullPath.c_str()))
	{
		LOG(2, "\t'palettes.ini' file was not found!\n");
		g_imGuiLogger->Log("[error] 'palettes.ini' file was not found!\n");
		return;
	}

	CString strBuffer;
	GetPrivateProfileString(L"General", L"OnlinePalettes", L"1", strBuffer.GetBuffer(MAX_PATH), MAX_PATH, wFullPath.c_str());
	strBuffer.ReleaseBuffer();
	m_loadOnlinePalettes = _ttoi(strBuffer);

	for (int i = 0; i < getCharactersCount(); i++)
	{
		for (int iSlot = 1; iSlot <= MAX_NUM_OF_PAL_SLOTS; iSlot++)
		{
			GetPrivateProfileString(getCharacterNameByIndexW(i).c_str(), std::to_wstring(iSlot).c_str(),
				L"", strBuffer.GetBuffer(MAX_PATH), MAX_PATH, wFullPath.c_str());
			strBuffer.ReleaseBuffer();

			strBuffer.Remove('\"');
			//delete file extension if found
			int pos = strBuffer.Find(L".impl", 0);
			if(pos >= 0)
			{ 
				strBuffer.Delete(pos, strBuffer.StringLength(strBuffer));
			}

			CT2CA pszConvertedAnsiString(strBuffer);
			m_paletteSlots[i][iSlot-1] = pszConvertedAnsiString;
		}
	}
}

void PaletteManager::InitPaletteSlotsVector()
{
	LOG(2, "InitPaletteSlotsVector\n");

	m_paletteSlots.clear();
	m_paletteSlots.resize(getCharactersCount());

	for (int i = 0; i < getCharactersCount(); i++)
	{
		for (int iSlot = 0; iSlot < MAX_NUM_OF_PAL_SLOTS; iSlot++)
		{
			m_paletteSlots[i].push_back("");
		}
	}
}

bool PaletteManager::PushImplFileIntoVector(IMPL_t & filledPal)
{
	LOG(7, "PushImplFileIntoVector\n");
	return PushImplFileIntoVector((CharIndex)filledPal.header.charindex, filledPal.paldata);
}

bool PaletteManager::PushImplFileIntoVector(CharIndex charIndex, IMPL_data_t & filledPalData)
{
	LOG(7, "PushImplFileIntoVector <overload>\n");

	if (charIndex > getCharactersCount())
	{
		g_imGuiLogger->Log("[error] Custom palette couldn't be loaded: CharIndex out of bound.\n");
		LOG(2, "ERROR, CharIndex out of bound\n");
		return false;
	}
	if (FindCustomPalIndex(charIndex, filledPalData.palname) > 0)
	{
		g_imGuiLogger->Log(
			"[error] Custom palette couldn't be loaded: a palette with name '%s' is already loaded.\n",
			filledPalData.palname
		);
		LOG(2, "ERROR, A custom palette with name '%s' is already loaded.\n", filledPalData.palname);
		return false;
	}

	m_customPalettes[charIndex].push_back(filledPalData);

	g_imGuiLogger->Log(
		"[system] %s: Loaded '%s.impl'\n",
		getCharacterNameByIndexA(charIndex).c_str(),
		filledPalData.palname
	);
	return true;
}

bool PaletteManager::WritePaletteToFile(CharIndex charIndex, IMPL_data_t *filledPalData)
{
	LOG(2, "WritePaletteToFile\n");

	std::string path = std::string("BBCF_IM\\Palettes\\") + getCharacterNameByIndexA(charIndex) + "\\" + filledPalData->palname + ".impl";

	IMPL_t IMPL_file {};

	IMPL_file.header.headerlen = sizeof(IMPL_header_t);
	IMPL_file.header.datalen = sizeof(IMPL_data_t);
	IMPL_file.header.charindex = charIndex;
	IMPL_file.paldata = *filledPalData;

	if (!utils_WriteFile(path.c_str(), &IMPL_file, sizeof(IMPL_t), true))
	{
		LOG(2, "\tCouldn't open %s!\n", strerror(errno));
		g_imGuiLogger->Log("[error] Unable to open '%s' : %s\n", path.c_str(), strerror(errno));
		return false;
	}

	return true;
}

void PaletteManager::LoadAllPalettes()
{
	LOG(2, "LoadAllPalettes\n");

	LoadPalettesFromFolder();
	LoadPaletteSettingsFile();

	//if(m_loadOnlinePalettes)
	//	StartAsyncPaletteArchiveDownload();
}

void PaletteManager::ReloadAllPalettes()
{
	LOG(2, "ReloadAllPalettes\n");
	g_imGuiLogger->LogSeparator();
	g_imGuiLogger->Log("[system] Reloading custom palettes...\n");

	LoadAllPalettes();
}

int PaletteManager::GetOnlinePalsStartIndex(CharIndex charIndex)
{
	if (charIndex > getCharactersCount())
		return MAXINT32;

	return m_onlinePalsStartIndex[charIndex];
}

void PaletteManager::OverwriteIMPLDataPalName(std::string fileName, IMPL_data_t & palData)
{
	//overwrite palname in data section with the filename, so renaming the file has effect on the actual ingame palname

	int pos = fileName.find('/');
	if(pos != std::string::npos)
		fileName = fileName.substr(pos + 1);

	std::string fileNameWithoutExt = fileName.substr(0, fileName.length() - strlen(".impl"));
	memset(palData.palname, 0, IMPL_PALNAME_LENGTH);
	strncpy(palData.palname, fileNameWithoutExt.c_str(), IMPL_PALNAME_LENGTH - 1);
}

//return values:
// ret > 0, index found
// ret == -1, index not found
// ret == -2, charindex out of bound
// ret == -3, default palette or no name given
int PaletteManager::FindCustomPalIndex(CharIndex charIndex, const char * palNameToFind)
{
	LOG(2, "FindCustomPalIndex\n");

	if (charIndex > getCharactersCount())
		return -2;

	if (strcmp(palNameToFind, "") == 0 || strcmp(palNameToFind, "Default") == 0)
		return -3;

	for (int i = 0; i < m_customPalettes[charIndex].size(); i++)
	{
		if (strcmp(palNameToFind, m_customPalettes[charIndex][i].palname) == 0)
		{
			return i;
		}
	}

	return -1;
}

bool & PaletteManager::PaletteArchiveDownloaded()
{
	return m_PaletteArchiveDownloaded;
}

bool PaletteManager::SwitchPalette(CharIndex charIndex, CharPaletteHandle& palHandle, int newCustomPalIndex)
{
	int totalCharPals = m_customPalettes[charIndex].size();
	if (newCustomPalIndex >= totalCharPals)
		return false;

	palHandle.SetSelectedCustomPalIndex(newCustomPalIndex);
	palHandle.ReplaceAllPalFiles(&m_customPalettes[charIndex][newCustomPalIndex]);
	return true;
}

void PaletteManager::ReplacePaletteFile(const char * newPalData, PaletteFile palFile, CharPaletteHandle& palHandle)
{
	palHandle.ReplaceSinglePalFile(newPalData, palFile);
}

const char * PaletteManager::GetPalFileAddr(PaletteFile palFile, CharPaletteHandle& palHandle)
{
	return palHandle.GetCurPalFileAddr(palFile);
}

const char * PaletteManager::GetCustomPalFile(CharIndex charIndex, int palIndex, PaletteFile palFile, CharPaletteHandle& palHandle)
{
	if (charIndex > getCharactersCount())
		charIndex = CharIndex_Ragna;

	if (palIndex > m_customPalettes[charIndex].size())
		palIndex = 0;

	const char* ptr = 0;

	if (palIndex == 0)
		ptr = palHandle.GetOrigPalFileAddr(palFile);
	else
	{
		ptr = m_customPalettes[charIndex][palIndex].file0;
		ptr += palFile * IMPL_PALETTE_DATALEN;
	}

	return ptr;
}

int PaletteManager::GetCurrentCustomPalIndex(CharPaletteHandle& palHandle)
{
	return palHandle.GetSelectedCustomPalIndex();
}

const IMPL_data_t & PaletteManager::GetCurrentPalData(CharPaletteHandle& palHandle)
{
	return palHandle.GetCurrentPalData();
}

void PaletteManager::OnUpdate(CharPaletteHandle & P1, CharPaletteHandle & P2)
{
	P1.UnlockUpdate();
	P2.UnlockUpdate();
}

void PaletteManager::OnMatchInit(Player& playerOne, Player& playerTwo)
{
	LoadPaletteSettingsFile();

	playerOne.GetPalHandle().OnMatchInit();
	playerTwo.GetPalHandle().OnMatchInit();

	ApplyDefaultCustomPalette((CharIndex)playerOne.GetData()->charIndex, playerOne.GetPalHandle());
	ApplyDefaultCustomPalette((CharIndex)playerTwo.GetData()->charIndex, playerTwo.GetPalHandle());
}

void PaletteManager::OnMatchEnd(Player& playerOne, Player& playerTwo)
{
	playerOne.GetPalHandle().OnMatchEnd();
	playerTwo.GetPalHandle().OnMatchEnd();
}

std::vector<std::vector<IMPL_data_t>>& PaletteManager::GetCustomPalettesVector()
{
	return m_customPalettes;
}