# map_generator.py
# Procedural Alien World Map Generator

import math

WIDTH = 512
HEIGHT = 256

def noise(x, y):
    n = x + y * 57
    n = (n << 13) ^ n
    return 1.0 - ((n * (n * n * 15731 + 789221) + 1376312589)
                  & 0x7fffffff) / 1073741824.0

def smooth_noise(x, y):
    return (
        noise(int(x), int(y)) * 0.5 +
        noise(int(x)+1, int(y)) * 0.25 +
        noise(int(x), int(y)+1) * 0.25
    )

def fractal_noise(x, y):
    return (
        0.6 * smooth_noise(x * 0.01, y * 0.01) +
        0.3 * smooth_noise(x * 0.02, y * 0.02) +
        0.1 * smooth_noise(x * 0.05, y * 0.05)
    )

def get_biome(temp, moisture, elevation):
    if elevation < 0.45: return "~"
    if elevation > 0.85: return "V"
    if elevation > 0.75: return "^"

    if temp < 0.2: return "I"
    if temp < 0.3: return "T"

    if temp > 0.7 and moisture < 0.3: return "D"
    if temp > 0.7 and moisture > 0.6: return "J"

    if moisture > 0.6: return "F"
    if moisture < 0.3: return "."

    return "#"

world = []

for y in range(HEIGHT):
    latitude = y / HEIGHT
    row = []

    for x in range(WIDTH):
        elevation = (fractal_noise(x, y) + 1) / 2
        moisture = (fractal_noise(x+100, y+100) + 1) / 2

        temp = math.cos(latitude * math.pi)
        temp += fractal_noise(x+200, y+200) * 0.2

        biome = get_biome(temp, moisture, elevation)
        row.append(biome)

    world.append(row)

with open("map.txt", "w") as f:
    for row in world:
        f.write("".join(row) + "\n")

print("Map generated: map.txt")
