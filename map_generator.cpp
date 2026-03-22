// map_generator.cpp
// Procedural Alien World Map Generator

#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <cstdlib>

const int WIDTH = 512;
const int HEIGHT = 256;

struct Tile {
    float elevation;
    float moisture;
    float temperature;
    char biome;
};

// Simple pseudo-noise
float noise(int x, int y) {
    int n = x + y * 57;
    n = (n << 13) ^ n;
    return (1.0 - ((n * (n * n * 15731 + 789221) + 1376312589)
        & 0x7fffffff) / 1073741824.0);
}

float smoothNoise(float x, float y) {
    return noise((int)x, (int)y) * 0.5 +
           noise((int)x + 1, (int)y) * 0.25 +
           noise((int)x, (int)y + 1) * 0.25;
}

float fractalNoise(float x, float y) {
    return 0.6 * smoothNoise(x * 0.01, y * 0.01) +
           0.3 * smoothNoise(x * 0.02, y * 0.02) +
           0.1 * smoothNoise(x * 0.05, y * 0.05);
}

char getBiome(float temp, float moisture, float elevation) {
    if (elevation < 0.45) return '~'; // ocean
    if (elevation > 0.85) return 'V'; // volcano
    if (elevation > 0.75) return '^'; // mountain

    if (temp < 0.2) return 'I'; // ice
    if (temp < 0.3) return 'T'; // tundra

    if (temp > 0.7 && moisture < 0.3) return 'D'; // desert
    if (temp > 0.7 && moisture > 0.6) return 'J'; // jungle

    if (moisture > 0.6) return 'F'; // forest
    if (moisture < 0.3) return '.'; // grassland

    return '#'; // mixed biome
}

int main() {
    std::vector<std::vector<Tile>> world(HEIGHT, std::vector<Tile>(WIDTH));

    for (int y = 0; y < HEIGHT; y++) {
        float latitude = (float)y / HEIGHT;

        for (int x = 0; x < WIDTH; x++) {
            float elevation = (fractalNoise(x, y) + 1) / 2;
            float moisture = (fractalNoise(x + 100, y + 100) + 1) / 2;

            float temp = cos(latitude * M_PI);
            temp += fractalNoise(x + 200, y + 200) * 0.2;

            char biome = getBiome(temp, moisture, elevation);

            world[y][x] = { elevation, moisture, temp, biome };
        }
    }

    std::ofstream file("map.txt");

    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            file << world[y][x].biome;
        }
        file << "\n";
    }

    file.close();

    std::cout << "Map generated: map.txt\n";
    return 0;
}
