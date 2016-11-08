#ifndef KANTI_FILE_MANAGER

class KantiFileManager
{
	public:

	k_internal platform_get_file_contents* PlatformGetFile;
	k_internal platform_set_file_contents* PlatformSetFile;

	k_internal inline KString GetFileContents(KString FileName)
	{
		return KantiFileManager::PlatformGetFile(FileName);
	}

	k_internal inline void SetFileContents(KString FileName, KString Contents)
	{
		KantiFileManager::PlatformSetFile(FileName, Contents);
	}

	k_internal inline void LoadOBJFile(KString FileName, KMeshData& ObjectData)
	{
		KString FileContents = GetFileContents(FileName);

		if (!FileContents.Data())
		{
			return;
		}

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

				KVertex Vertex = {};

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

				Vertex.Color = { 1.0f, 1.0f, 1.0f };
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

inline k_internal KString GetFileContents(KString FileName)
{
	return KantiFileManager::GetFileContents(FileName);
}

inline k_internal void SetFileContents(KString FileName, KString Contents)
{
	KantiFileManager::SetFileContents(FileName, Contents);
}

#define KANTI_FILE_MANAGER
#endif
