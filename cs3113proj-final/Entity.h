#ifndef ENTITY_H
#define ENTITY_H

#include <map>
#include <vector>
#include <deque>
#include "Map.h"
#include "Hitbox.h"
#include "Meter.h"
#include "glm/glm.hpp"
#include "ShaderProgram.h"
enum EntityType { PLATFORM, PLAYER, ENEMY  };
enum AIType     { WALKER, GUARD, TRAP      };
enum AIState    { WALKING, IDLE, ATTACKING };
enum AtkStance  { N, E, S, W               };
struct AtkInput
{
    AtkStance stance;
    int weight;
};


enum AnimationDirection { LEFT, RIGHT, UP, DOWN };

AtkStance& operator++(AtkStance& dir);
AtkStance& operator--(AtkStance& dir);

class Hitbox;

class Entity
{
    friend class Hitbox;

    struct AnimationData {
        std::vector<int> indices;
        int active_frames;
        int active_start;
    };

private:
    bool m_is_active = true;
    bool m_alive = true;
    
    EntityType m_entity_type;
    AIType     m_ai_type;
    AIState    m_ai_state;
    // ————— TRANSFORMATIONS ————— //
    glm::vec3 m_movement;
    glm::vec3 m_position;
    glm::vec3 m_target_position;
    glm::vec3 m_scale;
    glm::vec3 m_velocity;
    glm::vec3 m_acceleration;

    glm::mat4 m_model_matrix;

    float     m_speed,
              m_jumping_power;
    
    bool m_is_moving = false;
    bool m_is_attacking = false;
    bool m_is_jumping;
    bool m_face_forward = true; // right

    // ————— TEXTURES ————— //
    GLuint    m_texture_id;

    // ————— ANIMATION ————— //
    int m_animation_cols;
    int m_animation_frames,
        m_animation_index,
        m_animation_rows;


    int* m_animation_indices = nullptr;
    float m_animation_time = 0.0f;

    std::map<std::string, AnimationData> m_animations; // Map of animations
    std::string m_current_animation; // Current animation name

    bool m_animation_lock = false; // Will be used to prevent animation cancelling
    int m_recovery = RECOVERY_FRAMES;

    glm::mat2 m_margin = { 0.0f, 0.0f, 0.0f, 0.0f }; // The margin to trim for visual clarity in collisions

    float m_width = 1.0f,
          m_height = 1.0f;
    // ————— COLLISIONS ————— //
    bool m_collided_top    = false;
    bool m_collided_bottom = false;
    bool m_collided_left   = false;
    bool m_collided_right  = false;


    // ----- Combat ----- //
    AtkStance m_atk_stance = N;
    int m_atk_weight = 1;

    int m_h_advantage = 1;
    int m_h_luck = 3;
    bool m_h_wounded = false;

    Hitbox* m_hitbox = nullptr;
    Meter* m_meter = nullptr;

    std::deque<AtkInput> m_input_queue;

public:
    // ————— STATIC VARIABLES ————— //
    static constexpr int SECONDS_PER_FRAME = 9;
    static constexpr int MIN_ATK_WEIGHT = 1;
    static constexpr int MAX_ATK_WEIGHT = 3;
    static constexpr int RECOVERY_FRAMES = 2;

    // ————— METHODS ————— //
    Entity();
    Entity(GLuint texture_id, float speed, glm::vec3 acceleration, float jump_power, float animation_time,
        int animation_frames, int animation_index, int animation_cols,
           int animation_rows, float width, float height, EntityType EntityType);
    Entity(GLuint texture_id, float speed, glm::vec3 acceleration, float jump_power, float animation_time,
        int animation_frames, int animation_index, int animation_cols,
        int animation_rows, float width, float height, EntityType entity_type, AIType ai_type, AIState ai_state);
    Entity(GLuint texture_id, float speed, float width, float height, EntityType EntityType); // Simpler constructor
    Entity(GLuint texture_id, float speed, float width, float height, EntityType EntityType, AIType AIType, AIState AIState); // AI constructor
    ~Entity();

    void draw_sprite_from_texture_atlas(ShaderProgram* program, GLuint texture_id, int index);
    bool const check_collision(Entity* other) const;
    
    void const check_collision_y(Entity* collidable_entities, int collidable_entity_count);
    void const check_collision_x(Entity* collidable_entities, int collidable_entity_count);
    
    // Overloading our methods to check for only the map
    void const check_collision_y(Map *map);
    void const check_collision_x(Map *map);
    
    void update(float delta_time, Entity *player, Entity *collidable_entities, int collidable_entity_count, Map *map);
    void render(ShaderProgram* program);

    void ai_activate(Entity *player);
    void ai_walk();
    void ai_walk(float l_bound, float r_bound);
    void ai_guard(Entity *player);
    void ai_trap(Entity *player);
    
    void normalise_movement() { m_movement = glm::normalize(m_movement); }

    void face_right();
    void face_left();

    void move_left();
    void move_right();
    
    void const jump() { switch_animation("jump", true);  m_is_jumping = true; }
    void const attack();
    void const bind(AtkStance o_atk_stance, int o_atk_weight);
    void const parry(AtkStance o_atk_stance, int o_atk_weight);
    void const knockback();
    void const take_hit();
    void const death();

    void set_animation(std::string animation_name, int* indices, int frames, int active_start, int active_frames );
    void switch_animation(std::string animation_name, bool locked);

    // ————— GETTERS ————— //
    EntityType const get_entity_type()    const { return m_entity_type;   };
    AIType     const get_ai_type()        const { return m_ai_type;       };
    AIState    const get_ai_state()       const { return m_ai_state;      };
    float const get_jumping_power() const { return m_jumping_power; }
    glm::vec3 const get_position()     const { return m_position; }
    glm::vec3 const get_velocity()     const { return m_velocity; }
    glm::vec3 const get_acceleration() const { return m_acceleration; }
    glm::vec3 const get_movement()     const { return m_movement; }
    glm::vec3 const get_scale()        const { return m_scale; }
    GLuint    const get_texture_id()   const { return m_texture_id; }
    float     const get_speed()        const { return m_speed; }
    bool      const get_collided_top() const { return m_collided_top; }
    bool      const get_collided_bottom() const { return m_collided_bottom; }
    bool      const get_collided_right() const { return m_collided_right; }
    bool      const get_collided_left() const { return m_collided_left; }

    void const horizontal_mirror() { m_scale.x *= -1; }

    // Combat get/set //
    int const get_weight() const { return m_atk_weight;  }
    void inc_weight() { if ( m_atk_weight < MAX_ATK_WEIGHT ) ++m_atk_weight; }
    void dec_weight() { if ( m_atk_weight > MIN_ATK_WEIGHT ) --m_atk_weight; }
    AtkStance const get_stance() const { return m_atk_stance;  }
    std::string const get_stance_str() const;
    void inc_stance() { ++m_atk_stance;  }
    void dec_stance() { --m_atk_stance;  }
    bool      const is_alive() const { return m_alive; }

    Hitbox* get_hitbox() const { return m_hitbox; }
    
    void activate() { m_is_active = true; m_alive = true; };
    void deactivate() { m_is_active = false; };
    // ————— SETTERS ————— //
    void const set_entity_type(EntityType new_entity_type)  { m_entity_type = new_entity_type;};
    void const set_ai_type(AIType new_ai_type){ m_ai_type = new_ai_type;};
    void const set_ai_state(AIState new_state){ m_ai_state = new_state;};
    void const set_position(glm::vec3 new_position) { m_position = new_position; }
    void const set_velocity(glm::vec3 new_velocity) { m_velocity = new_velocity; }
    void const set_acceleration(glm::vec3 new_acceleration) { m_acceleration = new_acceleration; }
    void const set_movement(glm::vec3 new_movement) { m_movement = new_movement; }
    void const set_scale(glm::vec3 new_scale) { m_scale = new_scale; }
    void const set_texture_id(GLuint new_texture_id) { m_texture_id = new_texture_id; }
    void const set_speed(float new_speed) { m_speed = new_speed; }
    void const set_animation_cols(int new_cols) { m_animation_cols = new_cols; }
    void const set_animation_rows(int new_rows) { m_animation_rows = new_rows; }
    void const set_animation_frames(int new_frames) { m_animation_frames = new_frames; }
    void const set_animation_index(int new_index) { m_animation_index = new_index; }
    void const set_animation_time(float new_time) { m_animation_time = new_time; }
    void const set_margin_x(glm::vec2 new_margin) { m_margin[0][0] = new_margin.x; m_margin[0][1] = new_margin.y; }
    void const set_margin_y(glm::vec2 new_margin) { m_margin[1][0] = new_margin.x; m_margin[1][1] = new_margin.y; }
    void const set_jumping_power(float new_jumping_power) { m_jumping_power = new_jumping_power;}
    void const set_width(float new_width) {m_width = new_width; }
    void const set_height(float new_height) {m_height = new_height; }

    void const set_hitbox(Hitbox* hitbox);
    void const set_hitdata_by_animation();
    void const set_meter(Meter* meter);
    void const hitbox_activate();
	void const hitbox_deactivate();

};

#endif // ENTITY_H
