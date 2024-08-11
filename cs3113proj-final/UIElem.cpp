#include "UIElem.h"

UIElem::UIElem()
    : m_position(0.0f), m_scale(1.0f, 1.0f, 0.0f), m_texture_id(1.0),
    m_animation_cols(0), m_animation_rows(0),
    m_animation_frames(0), m_animation_index(0), m_animation_indices(nullptr),
    m_animation_time(0.0f), m_animation_lock(false), m_hidden(false), m_entity(nullptr),
    m_parent_elem(nullptr), m_type(NONE)
    { 
    m_model_matrix = glm::mat4(1.0f);
}

UIElem::UIElem(GLuint texture_id, int animation_cols, int animation_rows, Entity* entity, ElemType type)
    : m_position(0.0f), m_scale(1.0f, 1.0f, 0.0f), m_texture_id(texture_id),
    m_animation_cols(animation_cols), m_animation_rows(animation_rows),
    m_animation_frames(0), m_animation_index(0), m_animation_indices(nullptr),
    m_animation_time(0.0f), m_animation_lock(false), m_hidden(false), m_entity(entity),
    m_parent_elem(nullptr), m_type(type)
    { 
    m_model_matrix = glm::mat4(1.0f);
    if (m_entity) {
        m_position = m_entity->get_position();
    }
}

UIElem::UIElem(GLuint texture_id, int animation_cols, int animation_rows, UIElem* parent, ElemType type)
    : m_position(0.0f), m_scale(1.0f, 1.0f, 0.0f), m_texture_id(texture_id),
    m_animation_cols(animation_cols), m_animation_rows(animation_rows),
    m_animation_frames(0), m_animation_index(0), m_animation_indices(nullptr),
    m_animation_time(0.0f), m_animation_lock(false), m_hidden(false), m_entity(nullptr),
    m_parent_elem(parent), m_type(type)
    { 
    m_model_matrix = glm::mat4(1.0f);
    if (m_entity) {
        m_position = m_entity->get_position();
    }
}

void UIElem::set_animation(std::string animation_name, int* indices, int frames) {
    m_animations[animation_name] = { std::vector<int>(indices, indices + frames) };
}

void UIElem::switch_animation(std::string animation_name, bool locked) {
	m_animation_index = 0;
	m_current_animation = animation_name;
	m_animation_indices = m_animations[animation_name].indices.data();
	m_animation_frames = m_animations[animation_name].indices.size();
}

void UIElem::update(float delta_time) {
    if (m_entity) {
        // Update the UIElem position based on the associated entity's position and offset
        m_position = m_entity->get_position() + m_offset;
    }
    else if (m_parent_elem) {
        m_position = m_parent_elem->m_position + m_offset;
    }

    // Update animation timing
    if (m_animation_indices != nullptr && !m_current_animation.empty()) {
        m_animation_time += delta_time;
        float frames_per_second = (float)1 / SECONDS_PER_FRAME;
        if (m_animation_time >= frames_per_second) {
            m_animation_time = 0.0f;
            m_animation_index++;
            if (m_animation_index >= m_animation_frames) {
                m_animation_lock = false;
                m_animation_index = 0; // Loop back to the first frame
            }
        }
    }

    // Update the model matrix with the new position and scale
    m_model_matrix = glm::mat4(1.0f);
    m_model_matrix = glm::translate(m_model_matrix, m_position);
    m_model_matrix = glm::scale(m_model_matrix, m_scale); // Apply scaling

    // stance rotation
    if (m_type == STANCE)
    {
        switch (m_entity->get_stance())
        {
        case N:
            m_model_matrix = glm::rotate(m_model_matrix, 0.0f, glm::vec3(0.0f, 0.0f, -1.0f));
            break;
        case E:
            m_model_matrix = glm::rotate(m_model_matrix, glm::pi<float>() / 2, glm::vec3(0.0f, 0.0f, -1.0f));
            break;
        case S:
            m_model_matrix = glm::rotate(m_model_matrix, glm::pi<float>(), glm::vec3(0.0f, 0.0f, -1.0f));
            break;
        case W:
            m_model_matrix = glm::rotate(m_model_matrix, 3 * glm::pi<float>() / 2, glm::vec3(0.0f, 0.0f, -1.0f));
            break;
        }
    }

    // weight indication, not very clean at all
    if (m_type == WEIGHT)
    {
        if (m_parent_elem->m_type == STANCE)
        {
            switch_animation("active", false);
        }
        else if (m_parent_elem->m_type == WEIGHT) {
            if (m_parent_elem->m_parent_elem->m_entity)
            {
				if (m_parent_elem->m_parent_elem->m_entity) {
					if (m_parent_elem->m_parent_elem->m_entity->get_weight() > 1) switch_animation("active", false);
					else switch_animation("idle", false);
				}
            }
            if (m_parent_elem->m_parent_elem->m_parent_elem)
            {
				if (m_parent_elem->m_parent_elem->m_parent_elem->m_entity) {
					if (m_parent_elem->m_parent_elem->m_parent_elem->m_entity->get_weight() > 2) switch_animation("active", false);
					else switch_animation("idle", false);
				}
            }
        }
    }

    // health indicator
    if (m_type == HEATLH)
    {
        switch (m_entity->get_health_state())
        {
        case ADV:
            switch_animation("adv", false);
            break;
        case HEALTH:
            switch_animation("health", false);
            break;
        }
    }
}

void UIElem::render(ShaderProgram* program) {
    if (!m_hidden) {
        program->set_model_matrix(m_model_matrix);
        draw_sprites_from_texture_atlas(program, m_texture_id, { m_animation_indices[m_animation_index] });
    }
}

void UIElem::set_scale(float x_scale, float y_scale) {
    m_scale.x = x_scale;
    m_scale.y = y_scale;
}

void UIElem::draw_sprites_from_texture_atlas(ShaderProgram* program, GLuint texture_id, const std::vector<int>& indices) {
    if (m_current_animation.empty() || m_animation_indices == nullptr) return;

    glBindTexture(GL_TEXTURE_2D, texture_id);

    for (int index : indices) {
        float u_coord = (float)(index % m_animation_cols) / (float)m_animation_cols;
        float v_coord = (float)(index / m_animation_cols) / (float)m_animation_rows;
        float width = 1.0f / (float)m_animation_cols;
        float height = 1.0f / (float)m_animation_rows;

        float tex_coords[] = {
            u_coord,         v_coord + height,
            u_coord + width, v_coord + height,
            u_coord + width, v_coord,
            u_coord,         v_coord + height,
            u_coord + width, v_coord,
            u_coord,         v_coord
        };

        float vertices[] = {
            -0.5, -0.5, 0.5, -0.5, 0.5, 0.5,
            -0.5, -0.5, 0.5, 0.5, -0.5, 0.5
        };

        glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
        glEnableVertexAttribArray(program->get_position_attribute());
        glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, tex_coords);
        glEnableVertexAttribArray(program->get_tex_coordinate_attribute());

        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    glDisableVertexAttribArray(program->get_position_attribute());
    glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
}
