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

		tinyobj::attrib_t Attribute;
		std::vector<tinyobj::shape_t> Shapes;
		std::vector<tinyobj::material_t> Materials;
		std::string Error;

		if (!tinyobj::LoadObj(&Attribute, &Shapes, &Materials, &Error, FileName.Data())) 
		{
			throw std::runtime_error(Error);
		}

		std::unordered_map<KVertex, uint32> UniqueVertices = {};

		for (const auto& Shape : Shapes) 
		{
			for (const auto& Index : Shape.mesh.indices) 
			{
				KVertex Vertex = {};

				Vertex.Position = {
					Attribute.vertices[3 * Index.vertex_index + 0],
					Attribute.vertices[3 * Index.vertex_index + 1],
					Attribute.vertices[3 * Index.vertex_index + 2]
				};

				Vertex.UV = {
					Attribute.texcoords[2 * Index.texcoord_index + 0],
					1.0f - Attribute.texcoords[2 * Index.texcoord_index + 1]
				};

				Vertex.Color = { 1.0f, 1.0f, 1.0f };

				if (UniqueVertices.count(Vertex) == 0) 
				{
					UniqueVertices[Vertex] = ObjectData.Vertices.Count();
					ObjectData.Vertices.PushBack(Vertex);
				}

				ObjectData.Indices.PushBack(UniqueVertices[Vertex]);
			}
		}
		/*
		return;

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
		*/
	}

	k_internal inline void LoadImageFile(KString FileName, KImageData& Image)
	{
		stbi_uc* PicturePixels = stbi_load(FileName.Data(), &Image.Width, &Image.Height, &Image.Channels, STBI_rgb_alpha);
		Image.Data = PicturePixels;

		Image.Size = Image.Width * Image.Height * 4;
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
