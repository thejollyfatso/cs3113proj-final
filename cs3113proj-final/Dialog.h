#ifndef DIALOG_H
#define DIALOG_H

#include "glm/glm.hpp"
#include "ShaderProgram.h"
#include <string>

class Entity; // Forward declaration

class Dialog {
public:
    glm::vec3 m_position;
    glm::vec3 m_scale;
    glm::vec3 m_offset;
    glm::mat4 m_model_matrix;
    GLuint m_texture_id;
    Entity* m_entity;  // Pointer to an associated entity

    int m_animation_cols;
    int m_animation_rows;
    int m_animation_index = 0;

    Dialog();
    Dialog(GLuint texture_id, int animation_cols, int animation_rows);

    void set_texture(GLuint texture_id) { m_texture_id = texture_id; }
    void set_position(glm::vec3 position) { m_position = position; }
    void set_scale(float x_scale, float y_scale) { m_scale.x = x_scale; m_scale.y = y_scale; }
    void set_offset(float x_offset, float y_offset) { m_offset.x = x_offset; m_offset.y = y_offset; }
    void set_entity(Entity* entity) { m_entity = entity; }

    void update(float delta_time);
    void render(ShaderProgram* program, int index);
    void draw_sprite_from_texture_atlas(ShaderProgram* program, int index);
};

#endif

