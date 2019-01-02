#ifndef SIMPLEXNOISE_H
#define SIMPLEXNOISE_H

#include "Bang/Bang.h"

namespace Bang
{ /**
  * @file    SimplexNoise.h
  * @brief   A Perlin Simplex Noise C++ Implementation (1D, 2D, 3D).
  *
  * Copyright (c) 2014-2018 Sebastien Rombauts (sebastien.rombauts@gmail.com)
  *
  * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
  * or copy at http://opensource.org/licenses/MIT)
  */
#pragma once

#include <cstddef>  // size_t

/**
 * @brief A Perlin Simplex Noise C++ Implementation (1D, 2D, 3D, 4D).
 */
class SimplexNoise
{
public:
    /**
     * Constructor of to initialize a fractal noise summation
     *
     * @param[in] frequency    Frequency ("width") of the first octave of noise
     * (default to 1.0)
     * @param[in] amplitude    Amplitude ("height") of the first octave of noise
     * (default to 1.0)
     * @param[in] lacunarity   Lacunarity specifies the frequency multiplier
     * between successive octaves (default to 2.0).
     * @param[in] persistence  Persistence is the loss of amplitude between
     * successive octaves (usually 1/lacunarity)
     */
    explicit SimplexNoise(float frequency = 1.0f,
                          float amplitude = 1.0f,
                          float lacunarity = 2.0f,
                          float persistence = 0.5f);

    static float Noise(float x);
    static float Noise(float x, float y);
    static float Noise(float x, float y, float z);

    // Fractal/Fractional Brownian Motion (fBm) noise summation
    float Fractal(size_t octaves, float x) const;
    float Fractal(size_t octaves, float x, float y) const;
    float Fractal(size_t octaves, float x, float y, float z) const;

private:
    // Parameters of Fractional Brownian Motion (fBm) : sum of N "octaves" of
    // noise
    float m_frequency;    ///< Frequency ("width") of the first octave of noise
                          ///(default to 1.0)
    float m_amplitude;    ///< Amplitude ("height") of the first octave of noise
                          ///(default to 1.0)
    float m_lacunarity;   ///< Lacunarity specifies the frequency multiplier
                          /// between successive octaves (default to 2.0).
    float m_persistence;  ///< Persistence is the loss of amplitude between
                          /// successive octaves (usually 1/lacunarity)
};
}

#endif  // SIMPLEXNOISE_H
