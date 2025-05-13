#include <cmath>
#include <vector>
#include <numeric>
#include <algorithm>
#include <random>

class PerlinNoise {
private:
    std::vector<int> p;

    double fade(double t) const {
        // Quintic fade function for smooth interpolation
        return t * t * t * (t * (t * 6 - 15) + 10);
    }

    double lerp(double t, double a, double b) const {
        return a + t * (b - a);
    }

    double grad(int hash, double x, double y) const {
        // Gradient directions
        int h = hash & 7;
        double u = h < 4 ? x : y;
        double v = h < 4 ? y : x;
        return ((h & 1) ? -u : u) + ((h & 2) ? -v : v);
    }

public:
	PerlinNoise(unsigned int seed = rand()) {
        p.resize(256);
        std::iota(p.begin(), p.end(), 0);

        std::mt19937 generator(seed);
        std::shuffle(p.begin(), p.end(), generator);

        p.insert(p.end(), p.begin(), p.end());  // Duplicate
    }

    double noise(double x, double y) const {
        int X = static_cast<int>(floor(x)) & 255;
        int Y = static_cast<int>(floor(y)) & 255;

        x -= floor(x);
        y -= floor(y);

        double u = fade(x);
        double v = fade(y);

        int A = p[X] + Y;
        int B = p[X + 1] + Y;

        return lerp(v,
            lerp(u, grad(p[A], x, y),
                grad(p[B], x - 1, y)),
            lerp(u, grad(p[A + 1], x, y - 1),
                grad(p[B + 1], x - 1, y - 1))
        );
    }

    // Fractal Perlin (Octaves)
    float octaveNoise(double x, double y, int octaves = 6,
        double persistence = 0.5, double lacunarity = 2.0) const {
        double total = 0.0;
        double amplitude = 1.0;
        double frequency = 1.0;
        double maxAmplitude = 0.0;

        for (int i = 0; i < octaves; ++i) {
            total += noise(x * frequency, y * frequency) * amplitude;
            maxAmplitude += amplitude;
            amplitude *= persistence;
            frequency *= lacunarity;
        }

        return (total / maxAmplitude + 1.0) / 2.0;  // Normalize to [0, 1]
    }
};
