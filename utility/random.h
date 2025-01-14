#pragma once

#include <random>


namespace detail
{
    static std::mt19937 random_engine;
}


/// random number between min and max
inline float random(float min, float max)
{
    std::uniform_real_distribution<float> dist{ min, max };
    return dist(detail::random_engine);
}



/// random number between 0 and max
inline float random(float max)
{
    return random(0.0f, max);
}


/// random number between min and max, Inclusive of both min and max
inline int random(int min, int max)
{
    std::uniform_int_distribution<int> dist{ min, max };
    return dist(detail::random_engine);
}


/// random number between 0 and max, Inclusive of max
inline int random(int max)
{
    return random(0, max);
}


