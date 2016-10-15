#ifndef KANTI_PRIMITIVES

#include "KantiPlatform.h"

k_internal void SetCubeVertex(KList<vertex>& Vertices, KList<uint32>& Indices)
{
	local_persist KList<vertex> CubeVertices
	{
		// front
		{ Vector3(-1.0f, -1.0f,  1.0f), Vector3(1.0f, 1.0f, 1.0f) },
		{ Vector3(1.0f, -1.0f,  1.0f), Vector3(1.0f, 1.0f, 1.0f) },
		{ Vector3(1.0f,  1.0f,  1.0f), Vector3(1.0f, 1.0f, 1.0f) },
		{ Vector3(-1.0f,  1.0f,  1.0f), Vector3(1.0f, 1.0f, 1.0f) },
		// back
		{ Vector3(-1.0f, -1.0f, -1.0f), Vector3(1.0f, 1.0f, 1.0f) },
		{ Vector3(1.0f, -1.0f, -1.0f), Vector3(1.0f, 1.0f, 1.0f) },
		{ Vector3(1.0f,  1.0f, -1.0f), Vector3(1.0f, 1.0f, 1.0f) },
		{ Vector3(-1.0f,  1.0f, -1.0f), Vector3(1.0f, 1.0f, 1.0f) },
	};

	Vertices = CubeVertices;

	local_persist KList<uint32> CubeIndices
	{
		// front
		0, 1, 2,
		2, 3, 0,
		// top
		1, 5, 6,
		6, 2, 1,
		// back
		7, 6, 5,
		5, 4, 7,
		// bottom
		4, 0, 3,
		3, 7, 4,
		// left
		4, 5, 1,
		1, 0, 4,
		// right
		3, 2, 6,
		6, 7, 3,
	};

	Indices = CubeIndices;
}

#define KANTI_PRIMITIVES
#endif