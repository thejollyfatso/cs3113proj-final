/**
* Author: Rafael de Leon
* Assignment: WimWam
* Date due: 2024-08-15, 1:00pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "Meter.h"

void Meter::update(float delta_time)
{
	m_animation_time += delta_time;
	float frames_per_second = (float)1 / SECONDS_PER_FRAME;

	if (m_animation_time >= frames_per_second) {
		m_animation_time = 0.0f;
		if (++m_frame >= m_FRAMES) m_frame = 0;
	}
}
