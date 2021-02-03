#include "hpl.h"

#include <atlstr.h>
#include <fstream>
#include <sstream>
#include <vector>

#define HPAL_HEADER_LEN 32

const char HPAL_HEADER[] = { '\x48', '\x50', '\x41', '\x4C', '\x25', '\x01', '\x00', '\x00', '\x20', '\x04', '\x00','\x00', '\x00',
'\x01', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x00', '\x01', '\x00','\x00', '\x10', '\x00', '\x00', 
'\x00', '\x00' };

// Delete allocated memory after use
bool LoadHplsFromFile(std::wstring wFolderPath, IMPL_t& templateFile)
{
	std::string folderPath(wFolderPath.begin(), wFolderPath.end());

	HANDLE hFind;
	WIN32_FIND_DATA data;

	hFind = FindFirstFile(wFolderPath.c_str(), &data);
	if (hFind != INVALID_HANDLE_VALUE) {
		do {

			// Ignore current and parent directories
			if (_tcscmp(data.cFileName, TEXT(".")) == 0 || _tcscmp(data.cFileName, TEXT("..")) == 0)
				continue;

			std::wstring wFileName(data.cFileName);
			std::string fileName(wFileName.begin(), wFileName.end());
			std::string fullPath(folderPath);
			fullPath.pop_back(); // Delete "*" at the end
			fullPath += fileName;

			if (fileName.find(HPL_FILE_EXTENSION) == std::string::npos)
				continue;

			char* pDst = 0;

			if (fileName.find("_effect0") == std::string::npos && fileName.find("_effectbloom") == std::string::npos)
			{
				printf("Loading %s\n", fileName.c_str());

				// Replace palname section with the filename, so renaming the file has effect on the actual ingame palname
				std::string fileNameWithoutExt = fileName.substr(0, fileName.length() - strlen(HPL_FILE_EXTENSION));
				memset(templateFile.palData.palInfo.palName, 0, IMPL_PALNAME_LENGTH);
				memcpy(templateFile.palData.palInfo.palName, fileNameWithoutExt.c_str(), strlen(fileNameWithoutExt.c_str()));
				pDst = templateFile.palData.file0;
			}
			else if (fileName.find("_effectbloom") != std::string::npos)
			{
				printf("Loading %s\n", fileName.c_str());

				templateFile.palData.palInfo.hasBloom = true;
				continue;
			}
			else
			{
				printf("Loading %s\n", fileName.c_str());

				int pos = fileName.find("_effect0");
				std::string index = fileName.substr(pos + 7, 2);

				if (!(std::istringstream(index) >> pos))
					pos = -1;

				if (pos > 7 || pos == -1)
				{
					printf("ERROR, effect file '%s' has wrong index!\n", fileName.c_str());
					return false;
				}
				pDst = templateFile.palData.file0 + (pos * IMPL_PALETTE_DATALEN);
			}

			// Read binary file into string
			std::ifstream file(fullPath, std::ios::binary);

			if (!file.is_open())
			{
				printf("ERROR, Unable to open '%s' : %s\n", fileName.c_str(), strerror(errno));
				return false;
			}
			file.seekg(HPAL_HEADER_LEN);
			file.read(pDst, IMPL_PALETTE_DATALEN); // What if the file is smaller then sizeof(IMPL_t) ?
			file.close();

		} while (FindNextFile(hFind, &data));
		FindClose(hFind);
	}

	return true;
}

bool WriteHplsToFile(IMPL_t& filledImplFile)
{
	const int NUMBER_OF_PAL_FILES = 8;

	const char* pSrc = filledImplFile.palData.file0;

	for (int i = 0; i < NUMBER_OF_PAL_FILES; i++)
	{
		std::string path = std::string(filledImplFile.palData.palInfo.palName);
		if (i > 0)
		{
			path += "_effect0";
			path += std::to_string(i);
		}
		path += HPL_FILE_EXTENSION;

		std::ofstream file;
		file.open(path, std::fstream::binary);

		printf("\nWriting '%s'", path.c_str());

		if (!file.is_open())
		{
			printf("ERROR, Unable to open '%s' : %s\n", path.c_str(), strerror(errno));
			return false;
		}

		file.write((char*)HPAL_HEADER, sizeof(HPAL_HEADER));
		file.write(pSrc, IMPL_PALETTE_DATALEN);
		file.close();

		pSrc += IMPL_PALETTE_DATALEN;
	}

	printf("\n");
	return true;
}

const char* GetCreatorName()
{
	printf("\nEnter creator's name (optional):\n");
	printf(">");

	static char buf[IMPL_CREATOR_LENGTH];
	if (fgets(buf, sizeof(buf), stdin))
	{
		buf[strcspn(buf, "\r\n")] = 0;
		return buf;
	}

	return "";
}

const char* GetDescription()
{
	printf("Enter description (optional):\n");
	printf(">");

	static char buf[IMPL_DESC_LENGTH];
	if (fgets(buf, sizeof(buf), stdin))
	{
		buf[strcspn(buf, "\r\n")] = 0;
		return buf;
	}

	return "";
}
