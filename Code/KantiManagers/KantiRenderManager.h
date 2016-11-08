#ifndef KANTI_RENDER_MANAGER

class KantiRenderManager
{
	public:

	platform_game_render_initialize* PlatformInitializeRenderer;
	platform_game_update_and_render* PlatformRenderer;
	platform_create_mesh_buffer* PlatformCreateMeshBuffer;



public:

	void Render()
	{
		PlatformRenderer();
	}

	void Initialize();

	void CreateMeshBuffer(KMeshRenderer* MeshRenderer)
	{
		PlatformCreateMeshBuffer(MeshRenderer);
	}
};

#define KANTI_RENDER_MANAGER
#endif

