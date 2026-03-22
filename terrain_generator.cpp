#include <cmath>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

struct TerrainSettings {
    int width = 256;
    int depth = 256;
    float baseHeight = 8.0f;

    // Fractal noise settings
    int octaves = 6;
    float persistence = 0.5f;
    float lacunarity = 2.0f;

    // Feature scales
    float continentScale = 180.0f;
    float mountainScale = 90.0f;
    float detailScale = 24.0f;

    // Feature strengths
    float continentStrength = 20.0f;
    float mountainStrength = 28.0f;
    float detailStrength = 4.0f;

    // Optional island shaping
    bool useIslandFalloff = false;
    float islandFalloffStrength = 1.8f;

    // Random seed
    uint32_t seed = 1337;
};

class TerrainGenerator {
public:
    explicit TerrainGenerator(uint32_t seed) : seed_(seed) {}

    std::vector<std::vector<float>> generate(const TerrainSettings& settings) const {
        std::vector<std::vector<float>> heights(
            settings.depth,
            std::vector<float>(settings.width, 0.0f)
        );

        for (int z = 0; z < settings.depth; ++z) {
            for (int x = 0; x < settings.width; ++x) {
                const float continent = fbm(
                    x / settings.continentScale,
                    z / settings.continentScale,
                    settings.octaves,
                    settings.persistence,
                    settings.lacunarity
                );

                const float mountain = ridgedFbm(
                    x / settings.mountainScale,
                    z / settings.mountainScale,
                    std::max(3, settings.octaves - 1),
                    0.55f,
                    2.1f
                );

                const float detail = fbm(
                    x / settings.detailScale,
                    z / settings.detailScale,
                    3,
                    0.45f,
                    2.6f
                );

                float height = settings.baseHeight;
                height += continent * settings.continentStrength;
                height += mountain * settings.mountainStrength;
                height += detail * settings.detailStrength;

                if (settings.useIslandFalloff) {
                    height *= islandFalloff(
                        static_cast<float>(x),
                        static_cast<float>(z),
                        static_cast<float>(settings.width),
                        static_cast<float>(settings.depth),
                        settings.islandFalloffStrength
                    );
                }

                heights[z][x] = height;
            }
        }

        return heights;
    }

    static bool saveHeightmapCsv(
        const std::vector<std::vector<float>>& heights,
        const std::string& filename
    ) {
        std::ofstream out(filename);
        if (!out.is_open()) {
            return false;
        }

        for (size_t z = 0; z < heights.size(); ++z) {
            for (size_t x = 0; x < heights[z].size(); ++x) {
                out << heights[z][x];
                if (x + 1 < heights[z].size()) {
                    out << ",";
                }
            }
            out << "\n";
        }

        return true;
    }

    static void printSummary(const std::vector<std::vector<float>>& heights) {
        if (heights.empty() || heights[0].empty()) {
            std::cout << "Terrain is empty.\n";
            return;
        }

        float minHeight = heights[0][0];
        float maxHeight = heights[0][0];
        double total = 0.0;
        size_t count = 0;

        for (const auto& row : heights) {
            for (float value : row) {
                minHeight = std::min(minHeight, value);
                maxHeight = std::max(maxHeight, value);
                total += value;
                ++count;
            }
        }

        std::cout << "Terrain summary\n";
        std::cout << "---------------\n";
        std::cout << "Width:  " << heights[0].size() << "\n";
        std::cout << "Depth:  " << heights.size() << "\n";
        std::cout << "Min Y:  " << minHeight << "\n";
        std::cout << "Max Y:  " << maxHeight << "\n";
        std::cout << "Avg Y:  " << (count == 0 ? 0.0 : total / static_cast<double>(count)) << "\n";
    }

private:
    uint32_t seed_;

    static float fade(float t) {
        return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
    }

    static float lerp(float a, float b, float t) {
        return a + t * (b - a);
    }

    static float grad(uint32_t hash, float x, float z) {
        switch (hash & 0x7U) {
            case 0: return  x + z;
            case 1: return -x + z;
            case 2: return  x - z;
            case 3: return -x - z;
            case 4: return  x;
            case 5: return -x;
            case 6: return  z;
            default: return -z;
        }
    }

    uint32_t hash(int x, int z) const {
        uint32_t h = seed_;
        h ^= static_cast<uint32_t>(x) * 374761393U;
        h = (h << 13U) ^ h;
        h ^= static_cast<uint32_t>(z) * 668265263U;
        h = (h << 15U) ^ h;
        h *= 1274126177U;
        return h;
    }

    float perlin(float x, float z) const {
        const int x0 = static_cast<int>(std::floor(x));
        const int z0 = static_cast<int>(std::floor(z));
        const int x1 = x0 + 1;
        const int z1 = z0 + 1;

        const float sx = x - static_cast<float>(x0);
        const float sz = z - static_cast<float>(z0);

        const float n00 = grad(hash(x0, z0), sx, sz);
        const float n10 = grad(hash(x1, z0), sx - 1.0f, sz);
        const float n01 = grad(hash(x0, z1), sx, sz - 1.0f);
        const float n11 = grad(hash(x1, z1), sx - 1.0f, sz - 1.0f);

        const float u = fade(sx);
        const float v = fade(sz);

        const float nx0 = lerp(n00, n10, u);
        const float nx1 = lerp(n01, n11, u);
        const float value = lerp(nx0, nx1, v);

        // Approximate normalization to [-1, 1]
        return std::clamp(value * 0.9f, -1.0f, 1.0f);
    }

    float fbm(float x, float z, int octaves, float persistence, float lacunarity) const {
        float amplitude = 1.0f;
        float frequency = 1.0f;
        float sum = 0.0f;
        float maxValue = 0.0f;

        for (int i = 0; i < octaves; ++i) {
            sum += perlin(x * frequency, z * frequency) * amplitude;
            maxValue += amplitude;
            amplitude *= persistence;
            frequency *= lacunarity;
        }

        if (maxValue == 0.0f) {
            return 0.0f;
        }

        return sum / maxValue;
    }

    float ridgedFbm(float x, float z, int octaves, float persistence, float lacunarity) const {
        float amplitude = 1.0f;
        float frequency = 1.0f;
        float sum = 0.0f;
        float maxValue = 0.0f;

        for (int i = 0; i < octaves; ++i) {
            float n = perlin(x * frequency, z * frequency);
            n = 1.0f - std::fabs(n);
            n *= n;

            sum += n * amplitude;
            maxValue += amplitude;
            amplitude *= persistence;
            frequency *= lacunarity;
        }

        if (maxValue == 0.0f) {
            return 0.0f;
        }

        return sum / maxValue;
    }

    static float islandFalloff(float x, float z, float width, float depth, float strength) {
        const float cx = width * 0.5f;
        const float cz = depth * 0.5f;

        const float nx = (x - cx) / cx;
        const float nz = (z - cz) / cz;
        const float d = std::sqrt(nx * nx + nz * nz);

        const float falloff = std::clamp(1.0f - std::pow(d, strength), 0.0f, 1.0f);
        return falloff;
    }
};

int main() {
    TerrainSettings settings;
    settings.width = 256;
    settings.depth = 256;
    settings.baseHeight = 10.0f;
    settings.octaves = 6;
    settings.persistence = 0.5f;
    settings.lacunarity = 2.0f;
    settings.continentScale = 180.0f;
    settings.mountainScale = 90.0f;
    settings.detailScale = 24.0f;
    settings.continentStrength = 18.0f;
    settings.mountainStrength = 32.0f;
    settings.detailStrength = 4.0f;
    settings.useIslandFalloff = false;
    settings.seed = 2026;

    TerrainGenerator generator(settings.seed);
    const auto heights = generator.generate(settings);

    TerrainGenerator::printSummary(heights);

    const std::string outputFile = "terrain_heightmap.csv";
    if (TerrainGenerator::saveHeightmapCsv(heights, outputFile)) {
        std::cout << "Saved heightmap to " << outputFile << "\n";
    } else {
        std::cerr << "Failed to save heightmap.\n";
        return 1;
    }

    return 0;
}
