#pragma once

#include <lysys/lysys.hpp>
#include <half.hpp>

#include "material.h"
#include "terrain.h"

// Chunk size
#define CHUNK_SIZE 512
#define CHUNK_SIZE_SQ (CHUNK_SIZE * CHUNK_SIZE)

// Chunk size in world units
#define CHUNK_WORLD_SIZE 2048

// Number of chunks past the center chunk to load
#define VIEW_DISTANCE 1

// Number of chunks in view on one axis
#define CHUNK_VIEW_EXTENT (VIEW_DISTANCE * 2 + 1)

// Number of chunks in view
#define CHUNK_VIEW_SIZE (CHUNK_VIEW_EXTENT * CHUNK_VIEW_EXTENT)

#define TERRAIN_CACHE_DIR ".tcache"

class Shader;

struct Chunk
{
	int32_t x, y; // Chunk coordinates
	half_float::half *heights; // Heightmap
	half_float::half *normals; // Normalmap
	Terrain *terrain; // Terrain renderable
};

class Generator
{
public:
	void render(Shader *shader) const;

	constexpr TerrainMaterials &getMaterials() { return _materials; }

	void update();

	Generator();
	~Generator();
private:
	Chunk _chunks[CHUNK_VIEW_SIZE]; // Chunks in view
	TerrainMaterials _materials; // Terrain materials
};
