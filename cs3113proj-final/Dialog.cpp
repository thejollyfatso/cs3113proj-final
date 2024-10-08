/**
* Author: Rafael de Leon
* Assignment: WimWam
* Date due: 2024-08-15, 1:00pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "Dialog.h"
#include "glm/gtc/matrix_transform.hpp"
#include "Entity.h"  // Include the Entity class
#include <thread>
#include <chrono>

Dialog::Dialog()
    : m_offset(0.0f), m_texture_id(0), m_entity(nullptr), m_animation_cols(1), m_animation_rows(1) {
    m_model_matrix = glm::mat4(1.0f);
    m_position = glm::vec3(0);
    m_scale = glm::vec3(1.0f);
}

Dialog::Dialog(GLuint texture_id, int animation_cols, int animation_rows)
    : m_texture_id(texture_id), m_offset(0.0f), m_entity(nullptr), m_animation_cols(animation_cols), m_animation_rows(animation_rows) {
    m_model_matrix = glm::mat4(1.0f);
    m_scale = glm::vec3(1.0f);
    m_position = glm::vec3(0);
}

void Dialog::speak(int index, float duration) {
    // Set the animation index and activate the dialog
    m_animation_index = index;
    m_active = true;

    // Record the start time
    auto start_time = std::chrono::high_resolution_clock::now();

    // Create a thread to deactivate the dialog after the specified duration
    std::thread([this, duration, start_time]() {
        // Continuously check the time elapsed
        while (true) {
            auto current_time = std::chrono::high_resolution_clock::now();
            std::chrono::duration<float> elapsed_time = current_time - start_time;

            // Deactivate the dialog when the duration has passed
            if (elapsed_time.count() >= duration) {
                m_active = false;
                break;
            }
        }
        }).detach(); // Detach the thread to let it run independently
}

void Dialog::update(float delta_time) {
    if (m_entity) {
        // If the entity pointer is not null, set position to match the entity's position plus offset
        m_position = m_entity->get_position() + m_offset;
    }

    // Update the model matrix with the new position and scale
    m_model_matrix = glm::mat4(1.0f);
    m_model_matrix = glm::translate(m_model_matrix, m_position);
    m_model_matrix = glm::scale(m_model_matrix, m_scale);
}

void Dialog::draw_sprite_from_texture_atlas(ShaderProgram* program, int index) {
    float u = (float)(index % m_animation_cols) / (float)m_animation_cols;
    float v = (float)(index / m_animation_cols) / (float)m_animation_rows;

    float sprite_width = 1.0f / (float)m_animation_cols;
    float sprite_height = 1.0f / (float)m_animation_rows;

    float tex_coords[] = {
        u, v + sprite_height,
        u + sprite_width, v + sprite_height,
        u + sprite_width, v,
        u, v + sprite_height,
        u + sprite_width, v,
        u, v
    };

    float vertices[] = {
        -0.5f, -0.5f,
         0.5f, -0.5f,
         0.5f,  0.5f,
        -0.5f, -0.5f,
         0.5f,  0.5f,
        -0.5f,  0.5f
    };

    glBindTexture(GL_TEXTURE_2D, m_texture_id);

    glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->get_position_attribute());

    glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, tex_coords);
    glEnableVertexAttribArray(program->get_tex_coordinate_attribute());

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(program->get_position_attribute());
    glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
}

void Dialog::render(ShaderProgram* program, int index) {
    if (m_active)
    {
        program->set_model_matrix(m_model_matrix);
        draw_sprite_from_texture_atlas(program, m_animation_index);
    }
}

