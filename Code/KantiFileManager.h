#ifndef KANTI_FILE_MANAGER

#include "KantiEngine.h"

class KantiFileManager
{
	public:

	k_internal platform_get_file_contents* PlatformGetFile;
	k_internal platform_set_file_contents* PlatformSetFile;

	k_internal inline k_string GetFileContents(k_string FileName)
	{
		return KantiFileManager::PlatformGetFile(FileName);
	}

	k_internal inline void SetFileContents(k_string FileName, k_string Contents)
	{
		KantiFileManager::PlatformSetFile(FileName, Contents);
	}
};

platform_get_file_contents* KantiFileManager::PlatformGetFile = nullptr;
platform_set_file_contents* KantiFileManager::PlatformSetFile = nullptr;

inline k_internal k_string GetFileContents(k_string FileName)
{
	return KantiFileManager::GetFileContents(FileName);
}

inline k_internal void SetFileContents(k_string FileName, k_string Contents)
{
	KantiFileManager::SetFileContents(FileName, Contents);
}

#define KANTI_FILE_MANAGER
#endif
