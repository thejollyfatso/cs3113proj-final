#ifndef METER_H
#define METER_H

#include "glm/glm.hpp"
#include "ShaderProgram.h"

class Meter {
public:
    void update(float delta_time);

    // ————— ATTRIBUTES ————— //
    static constexpr int SECONDS_PER_FRAME = 16;
    int m_FRAMES = 8;
    int m_frame = 0;
    float m_animation_time = 0.0f;
};

#endif // METER_H
