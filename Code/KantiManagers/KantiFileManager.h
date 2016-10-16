#ifndef KANTI_FILE_MANAGER

#include "KantiEngine.h"
#include "KantiConversionManager.h"

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

	k_internal inline void LoadOBJFile(k_string FileName, render_data& ObjectData)
	{
		k_string FileContents = GetFileContents(FileName);

		char CurrentFoundChar = ' ';
		for (char* Character = FileContents.Data(); *Character; ++Character)
		{
			switch (*Character)
			{
			case 'v':
			{
				++Character;
				if (*Character != ' ')
				{
					break;
				}
				++Character;

				vertex Vertex = {};

				for (uint32 Index = 0; Index < 3; ++Index)
				{
					if (*Character == ' ')
					{
						++Character;
					}

					std::string Value;
					while (*Character != ' ')
					{
						if (*Character == '\n' || *Character == 'v')
						{
							break;
						}

						Value += *Character;
						++Character;
					}

					Vertex.Position.E[Index] = ToFloat((char *)Value.data());
				}

				ObjectData.Vertices.PushBack(Vertex);

				break;
			}
				
			case 'f':
			{
				++Character;
				if (*Character != ' ')
				{
					break;
				}
				++Character;

				uint32 Indices[3] = {};

				for (uint32 Index = 0; Index < 3; ++Index)
				{
					if (*Character == ' ')
					{
						++Character;
					}

					std::string Value;
					while (*Character != ' ')
					{
						if (*Character == '\n' || *Character == 'f')
						{
							break;
						}

						Value += *Character;
						++Character;
					}

					Indices[Index] = ToInt((char *)Value.data()) - 1;
				}

				ObjectData.Indices.PushBack(Indices);

				break;
			}

			default:
				break;
			}
		}


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
