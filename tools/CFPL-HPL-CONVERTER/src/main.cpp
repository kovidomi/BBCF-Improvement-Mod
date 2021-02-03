#include "hpl.h"
#include "impl.h"

#include <atlstr.h>
#include <vector>

#define TITLE "CFPL-HPL-CONVERTER (v1.00) - by KoviDomi\n\n"

void ConvertHplsToImpl()
{
	printf("HPL to CFPL mode selected based on input files\n");
	printf("Converting HPLs to CFPL\n\n");

	IMPL_t* loadedTemplate = LoadImplFromFile(L"*");
	if (!loadedTemplate)
	{
		printf("ERROR reading .cfpl file\n");
		return;
	}

	LoadHplsFromFile(L"*", *loadedTemplate);

	const char* creatorName = GetCreatorName();
	strncpy(loadedTemplate->palData.palInfo.creator, creatorName, IMPL_CREATOR_LENGTH);

	const char* description = GetDescription();
	strncpy(loadedTemplate->palData.palInfo.desc, description, IMPL_DESC_LENGTH);

	if (!WriteImplToFile(loadedTemplate))
	{
		printf("ERROR writing .cfpl file\n");
	}

	delete loadedTemplate;
}

void ConvertImplToHpls()
{
	printf("CFPL to HPL mode selected based on input files\n");
	printf("Converting CFPL to HPLs\n\n");

	IMPL_t* implFile = LoadImplFromFile(L"*");
	if (!implFile)
	{
		printf("ERROR reading .cfpl file\n");
		return;
	}

	if (!WriteHplsToFile(*implFile))
	{
		printf("ERROR writing .hpl file\n");
	}

	delete implFile;
}

void ExitPrompt()
{
	printf("\n\nDone. Press any key to exit.\n");
	getchar();
}

int CountNumberOfValidFilesInFolder(int& implFiles, int& hplFiles)
{
	int count = 0;

	HANDLE hFind;
	WIN32_FIND_DATA data;

	hFind = FindFirstFile(L"*", &data);
	if (hFind != INVALID_HANDLE_VALUE) {
		do 
		{
			//ignore current and parent directories
			if (_tcscmp(data.cFileName, TEXT(".")) == 0 || _tcscmp(data.cFileName, TEXT("..")) == 0)
				continue;

			std::wstring wFileName(data.cFileName);
			std::string fileName(wFileName.begin(), wFileName.end());

			if (fileName.find(IMPL_FILE_EXTENSION) != std::string::npos)
			{
				count++;
				implFiles++;
				continue;
			}
			else if (fileName.find(HPL_FILE_EXTENSION) != std::string::npos)
			{
				count++;
				hplFiles++;
				continue;
			}
			
		} while (FindNextFile(hFind, &data));
		FindClose(hFind);
	}

	return count;
}

int main(int argc, char* argv[])
{
	printf(TITLE);

	int totalFiles = 0;
	int implFiles = 0;
	int hplFiles = 0;

	totalFiles = CountNumberOfValidFilesInFolder(implFiles, hplFiles);
	printf("Number of .cfpl files found: %d\n", implFiles);
	printf("Number of .hpl files found: %d\n\n", hplFiles);

	if (implFiles < 1)
	{
		printf("ERROR, there must be AT LEAST ONE .cfpl file present in the folder\n");
		ExitPrompt();
		return 2;
	}
	else if (implFiles > 1)
	{
		printf("ERROR, there must be ONLY ONE .cfpl file present in the folder\n");
		ExitPrompt();
		return 1;
	}

	if (hplFiles > 9)
	{
		printf("ERROR, too many .hpl files (9 max! 1 char, 7 effects, 1 bloom)\n");
		ExitPrompt();
		return 3;
	}

	if (totalFiles == 1) //one file, can only be cfpl
	{
		ConvertImplToHpls();
	}
	else if (totalFiles >= 2) //two or more files (one cfpl and multiple hpls)
	{
		ConvertHplsToImpl();
	}

	ExitPrompt();
	return 0;
}
