#include "noise_terrain_chunk.h"
#include "../../asset/asset_manager.h"
#include "../../rendering/shader_manager.h"
#include "../terrain_shader.h"
#include "../../rendering/shaders/lighting_shader.h"
#include "../../rendering/environment.h"
#include "../../util/random/worley_noise_generator.h"

#include <noise/noise.h>
#include <noise/module/ridgedmulti.h>
//using namespace noise;

#include "../../util/random/open_simplex_noise.h"

#define MOUNTAIN_SCALE_WIDTH 0.05
#define MOUNTAIN_SCALE_LENGTH 0.05
#define MOUNTAIN_SCALE_HEIGHT 8.0

#define ROUGH_SCALE_WIDTH 0.8
#define ROUGH_SCALE_LENGTH 0.8
#define ROUGH_SCALE_HEIGHT 1.3

#define SMOOTH_SCALE_WIDTH 0.08
#define SMOOTH_SCALE_LENGTH 0.08
#define SMOOTH_SCALE_HEIGHT 1.0

#define MASK_SCALE_WIDTH 0.02
#define MASK_SCALE_LENGTH 0.02

namespace apex {

std::vector<double> NoiseTerrainChunk::GenerateHeights(int seed, const ChunkInfo &chunk_info)
{
    std::vector<double> heights;

    WorleyNoiseGenerator worley(seed);

    SimplexNoiseData data;
    for (int i = 0; i < OSN_OCTAVE_COUNT; i++) {
        open_simplex_noise(seed, &data.octaves[i]);
        data.frequencies[i] = pow(2.0, double(i));
        data.amplitudes[i] = pow(0.5, OSN_OCTAVE_COUNT - i);
    }

    heights.resize(chunk_info.m_width * chunk_info.m_length);

    for (int z = 0; z < chunk_info.m_length; z++) {
        for (int x = 0; x < chunk_info.m_width; x++) {
            const double x_offset = x + (chunk_info.m_position.x * (chunk_info.m_width - 1));
            const double z_offset = z + (chunk_info.m_position.y * (chunk_info.m_length - 1));

            const double biome_height = (GetSimplexNoise(&data, x_offset * 0.6, z_offset * 0.6) + 1) * 0.5;

            const double height = (GetSimplexNoise(&data, x_offset,
                z_offset)) * 30;

            const double mountain = ((worley.Noise((double)x_offset * 0.017, (double)z_offset * 0.017, 0))) * 80.0;

            const size_t index = ((x + chunk_info.m_width) % chunk_info.m_width) + ((z + chunk_info.m_length) % chunk_info.m_length) * chunk_info.m_width;
            heights[index] = MathUtil::Lerp(height, mountain, MathUtil::Clamp(biome_height, 0.0, 1.0));
        }
    }

    for (int i = 0; i < OSN_OCTAVE_COUNT; i++) {
        open_simplex_noise_free(data.octaves[i]);
    }

    return heights;
}

NoiseTerrainChunk::NoiseTerrainChunk(const std::vector<double> &heights, const ChunkInfo &chunk_info)
    : TerrainChunk(chunk_info),
    m_heights(heights)
{
}

void NoiseTerrainChunk::OnAdded()
{
    std::shared_ptr<Mesh> mesh = BuildMesh(m_heights);

    mesh->SetShader(ShaderManager::GetInstance()->GetShader<TerrainShader>({
        { "SHADOWS", Environment::GetInstance()->ShadowsEnabled() },
        { "NUM_SPLITS", Environment::GetInstance()->NumCascades() },
        { "NORMAL_MAPPING", 1 }
    }));
    m_entity = std::make_shared<Entity>("terrain_node");
    m_entity->SetRenderable(mesh);

    m_entity->GetMaterial().SetParameter("shininess", 0.01f);
    m_entity->GetMaterial().SetParameter("roughness", 0.3f);
    m_entity->GetMaterial().texture0 = AssetManager::GetInstance()->LoadFromFile<Texture>("res/textures/grass.jpg");
    m_entity->GetMaterial().normals0 = AssetManager::GetInstance()->LoadFromFile<Texture>("res/textures/grass_nrm.jpg");
    m_entity->GetMaterial().texture1 = AssetManager::GetInstance()->LoadFromFile<Texture>("res/textures/dirt.jpg");
    m_entity->GetMaterial().normals1 = AssetManager::GetInstance()->LoadFromFile<Texture>("res/textures/dirt_nrm.jpg");
    m_entity->GetMaterial().texture2 = AssetManager::GetInstance()->LoadFromFile<Texture>("res/textures/brdfLUT.png");
}

int NoiseTerrainChunk::HeightIndexAt(int x, int z)
{
    const int size = m_chunk_info.m_width;
    return ((x + size) % size) + ((z + size) % size) * size;
}


double NoiseTerrainChunk::GetSimplexNoise(SimplexNoiseData *data, int x, int z)
{
    double result = 0.0;

    for (int i = 0; i < OSN_OCTAVE_COUNT; i++) {
        result += open_simplex_noise2(data->octaves[i], x / data->frequencies[i], z / data->frequencies[i]) * data->amplitudes[i];
    }

    return result;
}

} // namespace apex
