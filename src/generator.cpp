#include "generator.h"

#include <cstdio>

#include "engine.h"
#include "terrain.h"
#include "camera.h"

static Matrix2 rotate(float degrees)
{
	const float radians = mutil::radians(degrees);
	return Matrix2(
		mutil::cos(radians), -mutil::sin(radians),
		mutil::sin(radians), mutil::cos(radians)
	);
}

static constexpr int kWidth = 1024;
static constexpr int kHeight = 1024;
constexpr size_t kCount = (size_t)kWidth * (size_t)kHeight;

static constexpr float kFrequency = 1.0f / 1.0f; // Global frequency
static constexpr Vector2 kShift = Vector2(0.0f, 0.0f); // Global shift
static constexpr float kAmplitude = 1.0f; // Global amplitude
static constexpr float kOffset = 0.0f; // Global offset

/* Layer 1 */
static constexpr float kAmplitude1 = 112.0f; // Layer 1 amplitude
static constexpr float kOffset1 = 64.0f; // Layer 1 offset
static constexpr float kNoiseFrequency1 = 1.0f / 512.0f; // Layer 1 frequency
static constexpr int kNoiseOctaves1 = 8; // Layer 1 number of octaves
static constexpr float kNoisePersistence1 = 1.0f / 4.0f; // Layer 1 persistence
static constexpr Vector2 kNoiseOffset1 = Vector2(0.0f, 0.0f); // Layer 1 noise offset
static constexpr float kNoiseRotation1 = 0.0f; // Layer 1 noise rotation
static const Matrix2 kNoiseRotationMatrix1 = rotate(kNoiseRotation1);

/* Layer 2 */
static constexpr float kAmplitude2 = 24.0f; // Layer 2 amplitude
static constexpr float kOffset2 = 8.0f; // Layer 2 offset
static constexpr float kNoiseFrequency2 = 1.0f / 128.0f; // Layer 2 frequency
static constexpr int kNoiseOctaves2 = 4; // Layer 2 number of octaves
static constexpr float kNoisePersistence2 = 1.0f / 4.0f; // Layer 2 persistence
static constexpr Vector2 kNoiseOffset2 = Vector2(447.7f, 104.43f); // Layer 2 noise offset
static constexpr float kNoiseRotation2 = 13.77f; // Layer 2 noise rotation
static const Matrix2 kNoiseRotationMatrix2 = rotate(kNoiseRotation2);

/* Layer 3 */
static constexpr float kAmplitude3 = 2.0f; // Layer 3 amplitude
static constexpr float kOffset3 = -16.0f; // Layer 3 offset
static constexpr float kNoiseFrequency3 = 1.0f / 32.0f; // Layer 3 frequency
static constexpr int kNoiseOctaves3 = 4; // Layer 3 number of octaves
static constexpr float kNoisePersistence3 = 1.0f / 4.0f; // Layer 3 persistence
static constexpr Vector2 kNoiseOffset3 = Vector2(-22.0f, 204.1f); // Layer 3 noise offset
static constexpr float kNoiseRotation3 = 174.3f; // Layer 3 noise rotation
static const Matrix2 kNoiseRotationMatrix3 = rotate(kNoiseRotation3);

/* Transformer 1 */
static constexpr float kTransform1Scale = 1.0f; // Transformer 1 scale
static constexpr float kTransform1Offset = 8.0f; // Transformer 1 offset

/* Transformer 2 (hills) */
static constexpr float kHillsAmplitude = 4.0f; // Hills amplitude
static constexpr float kHillsFrequency = 1.0f / 16.0f; // Hills frequency

static constexpr float kHillsBias = 0.0f; // Hills bias
static constexpr float kHillsNoiseFrequency = 1.0f / 64.0f; // Hills noise frequency
static constexpr int kHillsOctaves = 8; // Hills noise octaves
static constexpr float kHillsPersistence = 1.0f / 2.0f; // Hills noise persistence
static Vector2 kHillsNoiseOffset = Vector2(89.75f, -153.1f); // Hills noise offset
static constexpr float kHillsNoiseRotation = 97.2f; // Hills noise rotation
static const Matrix2 kHillsNoiseRotationMatrix = rotate(kHillsNoiseRotation);

static constexpr float kHillsSmoothPower = 0.25f; // Hills smooth power (lower is smoother)

/* Transformer 3 (sea) */
static constexpr float kSeaLevel = 0.0f; // Sea level
static constexpr float kSeaFloor = -64.0f; // Sea floor
static constexpr float kSeaPower = 0.5f; // Sea floor interpolation power

/* Transformer 4 (mountains) */
static constexpr float kMountainBase = 96.0f; // Mountain base level
static constexpr float kMountainPeak = 224.0f; // Maximum mountain peak level
static constexpr float kMountainScale = 2.0f; // Mountain scale
static constexpr float kMountainPower = 2.5f; // Mountain power

static constexpr float kMountainNoisePower = 2.0f; // Mountain noise power
static constexpr float kMountainNoiseScale = 2.0f; // Mountain noise scale
static constexpr float kMountainNoiseFrequency = 1.0f / 96.0f; // Mountain noise frequency
static constexpr int kMountainNoiseOctaves = 8; // Mountain noise octaves
static constexpr float kMountainNoisePersistence = 1.0f / 3.0f; // Mountain noise persistence
static Vector2 kMountainNoiseOffset = Vector2(-2.0f, 33.7f); // Mountain noise offset
static constexpr float kMountainNoiseRotation = 74.2f; // Mountain noise rotation
static const Matrix2 kMountainNoiseRotationMatrix = rotate(kMountainNoiseRotation);

/* Transformer 5 (transition) */
static constexpr float kTransitionNoisePower = 3.0f; // Transition noise power
static constexpr float kTransitionNoiseScale = 0.0f; // Transition noise scale
static constexpr float kTransitionNoiseFrequency = 1.0f / 64.0f; // Transition noise frequency
static constexpr int kTransitionNoiseOctaves = 4; // Transition noise octaves
static constexpr float kTransitionNoisePersistence = 1.0f / 4.0f; // Transition noise persistence
static Vector2 kTransitionNoiseOffset = Vector2(54.0f, -111.2f); // Transition noise offset
static constexpr float kTransitionNoiseRotation = 174.3f; // Transition noise rotation
static const Matrix2 kTransitionNoiseRotationMatrix = rotate(kTransitionNoiseRotation);

static constexpr float smootherstep(float x)
{
	return smootherstep(0.0f, 1.0f, x);
}

static constexpr float smoothstep(float x)
{
	return smoothstep(0.0f, 1.0f, x);
}

static float layer1(const Vector2 &pos)
{
	Vector2 p = kNoiseRotationMatrix1 * ((pos + kNoiseOffset1) * kNoiseFrequency1);
	float factor = snoise(p, kNoisePersistence1, kNoiseOctaves1);
	return factor * kAmplitude1 + kOffset1;
}

static float layer2(const Vector2 &pos)
{
	Vector2 p = kNoiseRotationMatrix2 * ((pos + kNoiseOffset2) * kNoiseFrequency2);
	float factor = snoise(p, kNoisePersistence2, kNoiseOctaves2);
	return factor * kAmplitude2 + kOffset2;
}

static float layer3(const Vector2 &pos)
{
	Vector2 p = kNoiseRotationMatrix3 * ((pos + kNoiseOffset3) * kNoiseFrequency3);
	float factor = snoise(p, kNoisePersistence3, kNoiseOctaves3);
	return factor * kAmplitude3 + kOffset3;
}

static float transform1(const Vector2 &pos, float in)
{
	return in * kTransform1Scale + kTransform1Offset;
}

static float transform2(const Vector2 &pos, float in)
{
	Vector2 p = kHillsNoiseRotationMatrix * ((pos + kHillsNoiseOffset) * kHillsNoiseFrequency);
	float factor = kHillsAmplitude * snoise(p, kHillsPersistence, kHillsOctaves) + kHillsBias;
	factor = logistic(kHillsSmoothPower, factor); // Smooth, result is in [0, 1]
	factor = factor - 0.5f; // Center around 0

	return in + (kHillsAmplitude * factor);
}

static float transform3(const Vector2 &pos, float in)
{
	if (in > kSeaLevel)
		return in; // No transformation

	if (in < kSeaFloor)
		return kSeaFloor; // Clamp to sea floor

	/* Smooth transition between sea floor and sea level */
	float t = (in - kSeaFloor) / (kSeaLevel - kSeaFloor);
	t = t * smootherstep(powf(t, kSeaPower));

	return kSeaFloor + t * (kSeaLevel - kSeaFloor);
}

static float transform4(const Vector2 &pos, float in)
{
	if (in < kMountainBase)
		return in; // No transformation

	/* Intensify mountain peaks */
	/* Twice-differentiable function at (0, 0) with f(0) = 0, f'(0) = 1, f''(0) = 0 */

	float t0 = (in - kMountainBase) / (kMountainPeak - kMountainBase);
	float u = 1.0f - t0;
	u = u * smootherstep(u);

	float a = kMountainScale * (1.0f - powf(u, kMountainPower));
	float b = 1.0f - u;
	float v = smootherstep(t0);
	float w = v * a + (1.0f - v) * b; // [0, kMountainScale]

	float out = kMountainBase + w * (kMountainPeak - kMountainBase);

	/* Add noise */
	Vector2 p = kMountainNoiseRotationMatrix * ((pos + kMountainNoiseOffset) * kMountainNoiseFrequency);
	float factor = snoise(p, kMountainNoisePersistence, kMountainNoiseOctaves);

	/* More noise towards the peaks */
	factor *= smootherstep(powf(v, kMountainNoisePower));

	return out + factor * kMountainNoiseScale;
}

static float transform5(const Vector2 &pos, float in)
{
	if (in >= kMountainBase || in <= kSeaLevel)
		return in; // No transformation

	/* No transformation at the edges */
	float t = (in - kSeaLevel) / (kMountainBase - kSeaLevel);

	/* Add noise */
	Vector2 p = kTransitionNoiseRotationMatrix * ((pos + kTransitionNoiseOffset) * kTransitionNoiseFrequency);
	float factor = snoise(p, kTransitionNoisePersistence, kTransitionNoiseOctaves);

	/* Reduce noise around edges */
	float mask = 1.0f - mutil::abs(2.0f * t - 1.0f); // [0, 1]
	mask = smootherstep(mask); // Ensure second derivative is 0 at boundaries

	return in + factor * mask * kTransitionNoiseScale;
}

/* Compute height at world position */
static float compute(const Vector2 &p)
{
	/* Position */
	Vector2 pos = (p + kShift) * kFrequency;

	/* Combine layers */
	float result = 0.0f;
	result += layer1(pos);
	result += layer2(pos);
	result += layer3(pos);

	/* Transform */
	result = transform1(pos, result);
	result = transform2(pos, result);
	result = transform3(pos, result);
	result = transform4(pos, result);
	result = transform5(pos, result);

	/* Post process */
	result = result * kAmplitude + kOffset;

	return result;
}

struct Imagef
{
	float *data;
	int32_t width, height;

	Imagef(int32_t width, int32_t height) :
		width(width), height(height)
	{
		data = new float[width * height];
		memset(data, 0, width * height * sizeof(float));
	}

	~Imagef()
	{
		delete[] data;
	}

	constexpr float fetch(int32_t x, int32_t y) const
	{
		/* Clamp to edge */
		x = clamp(x, 0, width - 1);
		y = clamp(y, 0, height - 1);
		return data[y * width + x];
	}

	constexpr void set(int32_t x, int32_t y, float value)
	{
		if (x < 0 || x >= width || y < 0 || y >= height)
			return;
		data[x + y * width] = value;
	}
};

/* Generate terrain data a chunk (x, y) */
static void generateArea(int32_t x, int32_t y, half_float::half *heightmapOut, half_float::half *normalmapOut)
{
	IntVector2 start = IntVector2(x, y) * CHUNK_SIZE;
	IntVector2 end = start + IntVector2(CHUNK_SIZE);

	IntVector2 pos;
	int i, j;

	/* Generate heightmap */
	Imagef heights(CHUNK_SIZE, CHUNK_SIZE);
	for (j = 0, pos.y = start.y; pos.y < end.y; j++, pos.y++)	
	{
		for (i = 0, pos.x = start.x; pos.x < end.x; i++, pos.x++)
		{
			float r = compute(Vector2(pos));
			heights.set(i, j, r);
			heightmapOut[j * CHUNK_SIZE + i] = r;
		}
	}

	/* Blur heightmap */
	Imagef blurred(CHUNK_SIZE, CHUNK_SIZE);
	for (j = 0; j < CHUNK_SIZE; j++)	
	{
		for (i = 0; i < CHUNK_SIZE; i++)
		{
			float a = heights.fetch(i - 1, j - 1);
			float b = heights.fetch(i, j - 1);
			float c = heights.fetch(i + 1, j - 1);

			float d = heights.fetch(i - 1, j);
			float e = heights.fetch(i, j);
			float f = heights.fetch(i + 1, j);

			float g = heights.fetch(i - 1, j + 1);
			float h = heights.fetch(i, j + 1);
			float k = heights.fetch(i + 1, j + 1);

			float value = (a + c + g + k) + (b + d + f + h) * 2.0f + e * 4.0f;
			value /= 16.0f;

			blurred.set(i, j, value);
		}
	}

	/* Compute normals */
	for (j = 0; j < CHUNK_SIZE; j++)
	{
		for (i = 0; i < CHUNK_SIZE; i++)
		{
			/* Compute image gradient */
			float gradx = (blurred.fetch(i + 1, j) - blurred.fetch(i - 1, j)) / 2.0f;
			float grady = (blurred.fetch(i, j + 1) - blurred.fetch(i, j - 1)) / 2.0f;

			/* Compute normal */
			Vector3 normal = normalize(Vector3(-gradx, 1.0f, -grady));

			/* Store normal */
			int offset = (i + j * CHUNK_SIZE) * 3;
			normalmapOut[offset + 0] = normal.x;
			normalmapOut[offset + 1] = normal.y;
			normalmapOut[offset + 2] = normal.z;
		}
	}
}

static void writeChunk(const Chunk &chunk, const char *path)
{
	ls_handle file = ls_open(path, LS_FILE_WRITE, LS_SHARE_NONE, LS_CREATE_ALWAYS);
	if (!file)
	{
		ls_perror("ls_open");
		fatal("Failed to write chunk to %s", path);
	}

	ls_write(file, chunk.heights, CHUNK_SIZE_SQ * sizeof(half_float::half));
	ls_write(file, chunk.normals, CHUNK_SIZE_SQ * 3 * sizeof(half_float::half));

	ls_close(file);
}

static void clearTerrainCache()
{
	/* Clear terrain cache */
	ls_handle dirh = ls_opendir(TERRAIN_CACHE_DIR);
	if (dirh)
	{
		struct ls_dir *dir;
		while ((dir = ls_readdir(dirh)))
		{
			if (dir->type == LS_FT_FILE)
			{
				char path[256];
				snprintf(path, sizeof(path), TERRAIN_CACHE_DIR "/%s", dir->name);

				if (ls_delete(path) == -1)
					ls_perror("ls_delete");
			}
		}

		ls_close(dirh);
	}
}

/* Allocate memory for chunk heightmap and normalmap */
static void allocChunk(Chunk *chunk)
{
	chunk->heights = (half_float::half *)malloc(CHUNK_SIZE_SQ * sizeof(half_float::half));
	if (!chunk->heights)
		fatal("Failed to allocate chunk heightmap");

	chunk->normals = (half_float::half *)malloc(CHUNK_SIZE_SQ * 3 * sizeof(half_float::half));
	if (!chunk->normals)
		fatal("Failed to allocate chunk normalmap");
}

static void freeChunk(Chunk *chunk)
{
	if (chunk->heights)
	{
		free(chunk->heights);
		chunk->heights = nullptr;
	}

	if (chunk->normals)
	{
		free(chunk->normals);
		chunk->normals = nullptr;
	}

	if (chunk->terrain)
	{
		delete chunk->terrain;
		chunk->terrain = nullptr;
	}
}

static void pathForChunk(char *path, size_t size, int x, int y)
{
	snprintf(path, size, TERRAIN_CACHE_DIR "/%d_%d", x, y);
}

static void uploadChunk(Chunk *chunk)
{
	/* Create terrain */
	Terrain *terrain = chunk->terrain = new Terrain();
	terrain->load(CHUNK_SIZE, CHUNK_SIZE, chunk->heights, chunk->normals, 20);

	/* Set terrain scale and position */
	Vector3 scale = Vector3(
		CHUNK_WORLD_SIZE / terrain->width(),
		1.0f,
		CHUNK_WORLD_SIZE / terrain->height());
	terrain->setScale(scale);

	Vector3 position = Vector3(
		CHUNK_WORLD_SIZE * chunk->x,
		0.0f,
		CHUNK_WORLD_SIZE * chunk->y);
	terrain->setPosition(position);

	/* Release CPU memory */
	free(chunk->heights), chunk->heights = nullptr;
	free(chunk->normals), chunk->normals = nullptr;
}

// Load chunk at (chunkX, chunkY)
static void loadChunk(Chunk *chunk, int chunkX, int chunkY)
{
	/* Check cache */
	char path[256];
	pathForChunk(path, sizeof(path), chunkX, chunkY);

	allocChunk(chunk);
	chunk->x = chunkX;
	chunk->y = chunkY;

	ls_handle file = ls_open(path, LS_FILE_READ, LS_SHARE_READ, LS_OPEN_EXISTING);
	if (file)
	{
		printf("loadChunk: Cache hit for chunk %d, %d\n", chunkX, chunkY);

		/* Cache hit, load from disk */
		ls_read(file, chunk->heights, CHUNK_SIZE_SQ * sizeof(half_float::half));
		ls_read(file, chunk->normals, CHUNK_SIZE_SQ * 3 * sizeof(half_float::half));

		ls_close(file);
	}
	else
	{
		printf("loadChunk: Cache miss for chunk %d, %d\n", chunkX, chunkY);

		/* Cache miss, generate terrain */
		generateArea(chunkX, chunkY, chunk->heights, chunk->normals);

		/* Write to cache */
		writeChunk(*chunk, path);
	}

	/* Upload to GPU */
	uploadChunk(chunk);
}

// Load area around chunk (chunkX, chunkY)
static void loadArea(Chunk *chunks, int chunkX, int chunkY)
{
	const int startX = chunkX - VIEW_DISTANCE;
	const int endX = chunkX + VIEW_DISTANCE;
	const int startY = chunkY - VIEW_DISTANCE;
	const int endY = chunkY + VIEW_DISTANCE;

	/* Load chunks */
	for (int y = startY; y <= endY; y++)
	{
		for (int x = startX; x <= endX; x++)
		{
			Chunk *chunk = &chunks[(y - startY) * CHUNK_VIEW_EXTENT + (x - startX)];
			if (!chunk->terrain) // Not loaded
				loadChunk(chunk, x, y);
		}
	}
}

void Generator::render(Shader *shader) const
{
	for (int i = 0; i < CHUNK_VIEW_SIZE; i++)
	{
		const Chunk &chunk = _chunks[i];
		if (chunk.terrain)
			chunk.terrain->render(shader);
	}
}

void Generator::update()
{
	/* Compute current view coordinates */
	Camera *camera = getCamera();
	const Vector3 &position = camera->position();

	int viewX = (int)position.x / CHUNK_SIZE;
	int viewY = (int)position.z / CHUNK_SIZE;

	loadArea(_chunks, viewX, viewY);

	for (int i = 0; i < CHUNK_VIEW_SIZE; i++)
	{
		Chunk &chunk = _chunks[i];
		if (chunk.terrain)
		{
			chunk.terrain->update();
			chunk.terrain->setMaterials(_materials);
		}
	}
}

Generator::Generator()
{
	clearTerrainCache();

	memset(_chunks, 0, sizeof(_chunks));

	/* Create terrain cache directory */
	if (ls_createdir(TERRAIN_CACHE_DIR) == -1)
		ls_perror("ls_createdir");
}

Generator::~Generator()
{
	for (Chunk *chunk = _chunks; chunk < _chunks + CHUNK_VIEW_SIZE; chunk++)
		freeChunk(chunk);

	clearTerrainCache();
}
