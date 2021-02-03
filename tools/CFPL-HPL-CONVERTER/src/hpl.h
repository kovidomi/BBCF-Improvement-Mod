#pragma once
#include "impl_format.h"

#include <string>

#define HPL_FILE_EXTENSION ".hpl"

// Delete allocated memory after use
bool LoadHplsFromFile(std::wstring wFolderPath, IMPL_t& templateFile);
bool WriteHplsToFile(IMPL_t& filledImplFile);
const char* GetCreatorName();
const char* GetDescription();
