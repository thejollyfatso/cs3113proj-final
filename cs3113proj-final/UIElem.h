#ifndef UIELEM_H
#define UIELEM_H

#include <GL/glew.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "Entity.h"
enum ElemType { STANCE, WEIGHT, ADV, LUCK, HEATLH };

class UIElem {
    struct AnimationData {
        std::vector<int> indices;
        int active_frames;
        int active_start;
    };
public:
    ElemType m_type;
    glm::vec3 m_position;
    glm::vec3 m_scale;
    glm::vec3 m_offset;
    GLuint m_texture_id;
    glm::mat4 m_model_matrix;
    bool m_hidden = false;
    Entity* m_entity;

    static constexpr int SECONDS_PER_FRAME = 9;
    int m_animation_cols;
    int m_animation_rows;
    int m_animation_frames;
    int m_animation_index;
    std::string m_current_animation;
    int* m_animation_indices;
    std::map<std::string, AnimationData> m_animations;
    float m_animation_time;
    bool m_animation_lock;

    UIElem(GLuint texture_id, int animation_cols, int animation_rows, Entity* entity = nullptr, ElemType m_type);
    void set_animation(std::string animation_name, int* indices, int frames);
    void switch_animation(std::string animation_name, bool locked);
    void update(float delta_time);
    void update(float delta_time, Entity* entity);
    void render(ShaderProgram* program);
    void set_scale(float x_scale, float y_scale); // New method for setting scale
    void draw_sprites_from_texture_atlas(ShaderProgram* program, GLuint texture_id, const std::vector<int>& indices);
};

#endif // UIELEM_H

