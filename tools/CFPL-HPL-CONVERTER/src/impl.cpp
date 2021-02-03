#include "impl.h"

#include "utils.h"

#include <atlstr.h>
#include <fstream>
#include <iostream>

// Delete allocated memory after use
IMPL_t* LoadImplFromFile(std::wstring wFolderPath)
{
	std::string folderPath(wFolderPath.begin(), wFolderPath.end());

	IMPL_t* fileContents = new IMPL_t();

	HANDLE hFind;
	WIN32_FIND_DATA data;

	hFind = FindFirstFile(wFolderPath.c_str(), &data);
	if (hFind != INVALID_HANDLE_VALUE) {
		do {

			//ignore current and parent directories
			if (_tcscmp(data.cFileName, TEXT(".")) == 0 || _tcscmp(data.cFileName, TEXT("..")) == 0)
				continue;

			std::wstring wFileName(data.cFileName);
			std::string fileName(wFileName.begin(), wFileName.end());
			std::string fullPath(folderPath);
			fullPath.pop_back(); //delete "*" at the end
			fullPath += fileName;

			if (fileName.find(IMPL_FILE_EXTENSION) == std::string::npos)
				continue;

			std::ifstream file(fullPath, std::ios::binary);

			if (!file.is_open())
			{
				printf("ERROR, Unable to open '%s' : %s\n", fileName.c_str(), strerror(errno));
				continue;
			}

			printf("Loading %s\n", fileName.c_str());

			file.read((char*)fileContents, sizeof(IMPL_t)); // What if the file is smaller then sizeof(IMPL_t) ?
			file.close();

			// Check filesig
			if (strcmp(fileContents->header.fileSig, IMPL_FILESIG) != 0)
			{
				printf("ERROR, unrecognized file format of '%s' !\n", fileName.c_str());
				SAFE_DELETE(fileContents);
				break;
			}
			if (fileContents->header.dataLen != sizeof(IMPL_data_t))
			{
				printf("ERROR, data size mismatch in '%s' !\n", fileName.c_str());
				SAFE_DELETE(fileContents);
				break;
			}

		} while (FindNextFile(hFind, &data));

		FindClose(hFind);
		return fileContents;
	}

	SAFE_DELETE(fileContents);
	return nullptr;
}

bool WriteImplToFile(IMPL_t *filledImpl)
{
	std::string path = std::string(filledImpl->palData.palInfo.palName) + IMPL_FILE_EXTENSION;

	printf("\nWriting '%s'\n", path.c_str());

	std::ofstream file;
	file.open(path, std::fstream::binary);

	if (!file.is_open())
	{
		printf("ERROR, Unable to open '%s' : %s\n", path.c_str(), strerror(errno));
		return false;
	}

	file.write((char*)filledImpl, sizeof(IMPL_t));
	file.close();

	return true;
}
