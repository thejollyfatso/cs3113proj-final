/**
* Author: Rafael de Leon
* Assignment: [Your game's name here]
* Date due: 2024-08-15, 1:00pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include <GL/glew.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "Entity.h"

AtkStance& operator++(AtkStance& dir) {
    dir = static_cast<AtkStance>((static_cast<int>(dir) + 1) % 4);
    return dir;
}

AtkStance& operator--(AtkStance& dir) {
    dir = static_cast<AtkStance>((static_cast<int>(dir) + 3) % 4);
    return dir;
}

void Entity::set_ai_difficulty(AIDifficulty difficulty) {
	m_ai_difficulty = difficulty;
	switch (m_ai_difficulty) {
	case EASY:
		m_ai_action_delay = 750;
		break;
	case MEDIUM:
		m_ai_action_delay = 500;
		break;
	case HARD:
		m_ai_action_delay = 250;
		break;
	}
}

void Entity::ai_start_new_attack_sequence() {
    m_ai_attack_limit = rand() % 3 + 1; // Set attack limit to a random number between 1 and 3
    m_ai_attack_count = 0;              // Reset the attack count
}

void Entity::ai_attempt_attack() {
    auto now = std::chrono::steady_clock::now();
    auto time_since_last_attack = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_last_attack_time).count();

    // Check if enough time has passed and if we haven't reached the attack limit
    if (time_since_last_attack >= 250 && m_ai_attack_count < m_ai_attack_limit) {
        attack();
        m_last_attack_time = now; // Update the time of the last attack
        m_ai_attack_count++;      // Increment the attack count

        // Check if the attack sequence is complete
        if (m_ai_attack_count >= m_ai_attack_limit) {
            m_ai_state = DISTANCE; // Return to the DISTANCE state after completing the sequence
        }
    }
}

void Entity::ai_activate(Entity* player) {
    if (m_is_active)
    {
        switch (m_ai_type) {
        case TRAP:
            ai_trap(player);
            break;
        case DEBUG_RANGER:
            ai_range(player);
            break;
        case CRASHER:
            ai_crash(player);
            break;
        case MIRROR:
            ai_mirror(player);
            break;
        case COOLER:
            ai_cooler(player);
            break;
        case MASTER:
            if (m_ai_action_count % 3 == 0) ai_crash(player);
            if (m_ai_action_count % 3 == 1) ai_mirror(player);
            if (m_ai_action_count % 3 == 2) ai_cooler(player);
            break;
        default:
            break;
        }
    }
}

void Entity::ai_action_inc() {
    // Generate a random number between 0 and 99
    int rand_num = rand() % 100;

    // 50% chance to increment m_ai_action_count
    if (rand_num < 50) m_ai_action_count = rand() % 3; // Randomize between 0, 1, and 2

    m_ai_state = IDLE;
}

void Entity::ai_trap(Entity *player) {
    switch (m_ai_state) {
    case ATTACKING:
        attack();
        if (m_position.x > player->get_position().x + 1.2) {
            face_left();
            m_ai_state = IDLE;
        }
        else if (m_position.x < player->get_position().x - 1.2) {
            face_right();
            m_ai_state = IDLE;
        }
        break;
    case IDLE:
        if (!(m_position.x > player->get_position().x + 1.2 || m_position.x < player->get_position().x - 1.2)) {
            m_ai_state = ATTACKING;
            break;
        }
    }
}

void Entity::ai_range(Entity* player) {
    switch (m_ai_state) {
    case APPROACH:
        if (m_position.x > player->get_position().x + 2.4) {
            move_left();
        }
        else if (m_position.x < player->get_position().x - 2.4) {
            move_right();
        }
        if (glm::distance(m_position, player->get_position()) < 1.6f) {
            m_ai_state = DISTANCE;
        }
        break;
    case DISTANCE:
		if (glm::distance(m_position, player->get_position()) < 2.4f) {
            if (m_position.x < player->get_position().x) move_left();
            if (m_position.x > player->get_position().x) move_right();
        }
        else if (glm::distance(m_position, player->get_position()) > 3.2f) {
            m_ai_state = APPROACH;
        }
        break;
    }
}

void Entity::ai_crash(Entity* player) {
    auto now = std::chrono::steady_clock::now();
    auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_last_action_time).count();
    auto defense_elapsed_time = std::chrono::duration_cast<std::chrono::seconds>(now - m_defense_start_time).count();

    int opposite_rating = m_oracle_aggression_rating; // Use the aggression rating

    // Adjusted switch time formula for a range of 1 to 5 seconds
    float switch_time = 1 + 4 * pow(10, (1 - (opposite_rating / 10.0f)) - 1);

    switch (m_ai_state) {
    case CRASH_DEF:
        m_ai_attack_count = 0;

        if (defense_elapsed_time >= switch_time) {
            m_ai_state = CRASH_OFF;
            m_last_action_time = now;
            break;
        }

        if (elapsed_time >= m_ai_action_delay) {
            if (m_atk_stance != player->get_stance()) {
                if ((m_atk_stance + 1) % 4 == player->get_stance()) {
                    inc_stance();
                }
                else {
                    dec_stance();
                }
            }
            if (m_atk_weight != player->get_weight()) {
                if (m_atk_weight < player->get_weight()) {
                    inc_weight();
                }
                else {
                    dec_weight();
                }
            }
            m_last_action_time = now;
        }

        if (glm::distance(m_position, player->get_position()) < 2.0f) {
            if (m_position.x < player->get_position().x) move_left();
            if (m_position.x > player->get_position().x) move_right();
        }

        // Switch to CRASH_OFF if player is too far
        if (glm::distance(m_position, player->get_position()) > m_max_defensive_distance) {
            m_ai_state = CRASH_OFF;
        }

        break;

    case CRASH_OFF:
        if (elapsed_time >= m_ai_action_delay) {
            float distance = glm::distance(m_position, player->get_position());

            if (distance < m_attack_range) {
                if (m_ai_attack_count == 0) {
                    ai_start_new_attack_sequence();
                }
                ai_attempt_attack();
            }
            else {
                if (m_position.x < player->get_position().x) move_right();
                if (m_position.x > player->get_position().x) move_left();
            }
            m_last_action_time = now;
        }
        break;

    case IDLE:
        m_ai_state = CRASH_DEF;
        m_defense_start_time = std::chrono::steady_clock::now();
        break;
    }
}

void Entity::ai_mirror(Entity* player) {
    auto now = std::chrono::steady_clock::now();
    auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_last_action_time).count();
    auto defense_elapsed_time = std::chrono::duration_cast<std::chrono::seconds>(now - m_defense_start_time).count();

    int opposite_rating = m_oracle_aggression_rating; // Use the aggression rating

    // Adjusted switch time formula for a range of 1 to 5 seconds
    float switch_time = 1 + 4 * pow(10, (1 - (opposite_rating / 10.0f)) - 1);

    switch (m_ai_state) {
    case MIRROR_DEF:
        m_ai_attack_count = 0;

        if (defense_elapsed_time >= switch_time) {
            m_ai_state = MIRROR_OFF;
            m_last_action_time = now;
            break;
        }

        if (elapsed_time >= m_ai_action_delay) {
            if (m_atk_stance != static_cast<AtkStance>((player->get_stance() + 2) % 4)) {
                if ((m_atk_stance + 1) % 4 == static_cast<AtkStance>((player->get_stance() + 2) % 4)) {
                    inc_stance();
                }
                else {
                    dec_stance();
                }
            }
            if (m_atk_weight != player->get_weight()) {
                if (m_atk_weight > player->get_weight()) {
                    dec_weight();
                }
            }
            m_last_action_time = now;
        }

        if (glm::distance(m_position, player->get_position()) < 2.0f) {
            if (m_position.x < player->get_position().x) move_left();
            if (m_position.x > player->get_position().x) move_right();
        }

        // Switch to MIRROR_OFF if player is too far
        if (glm::distance(m_position, player->get_position()) > m_max_defensive_distance) {
            m_ai_state = MIRROR_OFF;
        }

        break;

    case MIRROR_OFF:
        if (elapsed_time >= m_ai_action_delay) {
            float distance = glm::distance(m_position, player->get_position());

            if (distance < m_attack_range) {
                if (m_ai_attack_count == 0) {
                    ai_start_new_attack_sequence();
                }
                ai_attempt_attack();
            }
            else {
                if (m_position.x < player->get_position().x) move_right();
                if (m_position.x > player->get_position().x) move_left();
            }
            m_last_action_time = now;
        }
        break;

    case IDLE:
        m_ai_state = MIRROR_DEF;
        m_defense_start_time = std::chrono::steady_clock::now();
        break;
    }
}

void Entity::ai_cooler(Entity* player) {
    auto now = std::chrono::steady_clock::now();
    auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_last_action_time).count();
    auto defense_elapsed_time = std::chrono::duration_cast<std::chrono::seconds>(now - m_defense_start_time).count();

    int opposite_rating = m_oracle_aggression_rating; // Use the aggression rating

    // Adjusted switch time formula for a range of 1 to 5 seconds
    float switch_time = 1 + 4 * pow(10, (1 - (opposite_rating / 10.0f)) - 1);

    switch (m_ai_state) {
    case COOLER_DEF:
        m_ai_attack_count = 0;

        if (defense_elapsed_time >= switch_time) {
            m_ai_state = COOLER_OFF;
            m_last_action_time = now;
            break;
        }

        if (elapsed_time >= m_ai_action_delay) {
            if (m_atk_stance != player->get_stance()) {
                if ((m_atk_stance + 1) % 4 == player->get_stance()) {
                    inc_stance();
                }
                else {
                    dec_stance();
                }
            }
            if (m_atk_weight != player->get_weight()) {
                if (m_atk_weight < player->get_weight()) {
                    inc_weight();
                }
                else {
                    dec_weight();
                }
            }
            m_last_action_time = now;
        }

        if (glm::distance(m_position, player->get_position()) < 2.0f) {
            if (m_position.x < player->get_position().x) move_left();
            if (m_position.x > player->get_position().x) move_right();
        }

        // Switch to COOLER_OFF if player is too far
        if (glm::distance(m_position, player->get_position()) > m_max_defensive_distance) {
            m_ai_state = COOLER_OFF;
        }

        break;

    case COOLER_OFF:
        if (elapsed_time >= m_ai_action_delay) {
            float distance = glm::distance(m_position, player->get_position());

            if (distance < m_attack_range) {
                if (m_ai_attack_count == 0) {
                    ai_start_new_attack_sequence();
                }
                ai_attempt_attack();
            }
            else {
                if (m_position.x < player->get_position().x) move_right();
                if (m_position.x > player->get_position().x) move_left();
            }
            m_last_action_time = now;
        }
        break;

    case IDLE:
        m_ai_state = COOLER_DEF;
        m_defense_start_time = std::chrono::steady_clock::now();
        break;
    }
}

void Entity::oracle_calculate_aggression_rating() {
    // Calculate the total time elapsed in the match in seconds
    auto now = std::chrono::steady_clock::now();
    float match_time = std::chrono::duration_cast<std::chrono::seconds>(now - m_defense_start_time).count();

    // Avoid division by zero if the match time is too short
    if (match_time < 1.0f) {
        match_time = 1.0f;
    }

    // Calculate the average time between attacks
    float avg_attack_interval = match_time / (static_cast<float>(m_attack_count)/6); // manual adjust

    // Map the average attack interval to an aggression rating on a scale of 1 to 10
    if (avg_attack_interval <= 1.0f) {
        // Map first second to aggression ratings 7-10
        m_oracle_aggression_rating = static_cast<int>(10 - (avg_attack_interval * 3));
    }
    else if (avg_attack_interval >= 5.0f) {
        // Map 5 seconds or more to aggression rating 1
        m_oracle_aggression_rating = 1;
    }
    else {
        // Map the rest of the range to aggression ratings 2-6
        m_oracle_aggression_rating = static_cast<int>(6 - ((avg_attack_interval - 1) / 4) * 5);
    }
}

void Entity::oracle_calculate_style_preference() {
    if (m_crash_count > m_mirror_count && m_crash_count > m_cool_count) {
        m_oracle_style_preference = StylePreference::CRASHING;
    }
    else if (m_mirror_count > m_crash_count && m_mirror_count > m_cool_count) {
        m_oracle_style_preference = StylePreference::MIRRORING;
    }
    else if (m_cool_count > m_crash_count && m_cool_count > m_mirror_count) {
        m_oracle_style_preference = StylePreference::COOLING;
    }
    else {
        m_oracle_style_preference = StylePreference::UNDEFINED;  // In case of a tie
    }
}

void Entity::oracle_calculate_bind_parry_ratio() {
    if (m_parry_count > 0) {
        m_oracle_bind_parry_ratio = static_cast<float>(m_bind_count) / m_parry_count;
    }
    else {
        m_oracle_bind_parry_ratio = (m_bind_count > 0) ? static_cast<float>(m_bind_count) : 0.0f;
    }
}

void Entity::oracle_calculate_close_retreat_ratio() {
    if (m_retreat_count > 0) {
        m_oracle_close_retreat_ratio = static_cast<float>(m_close_count) / m_retreat_count;
    }
    else {
        m_oracle_close_retreat_ratio = (m_close_count > 0) ? static_cast<float>(m_close_count) : 0.0f;
    }
}

void Entity::ai_oracle(Entity* player) {
    auto now = std::chrono::steady_clock::now();

    // 1. Track aggression (attack frequency)
    if (player->m_is_attacking) {  // Use actual variables and logic from your codebase
        auto time_since_last_attack = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_last_player_attack_time).count();
        if (m_attack_count > 0) {
            m_player_attack_interval_avg = ((m_player_attack_interval_avg * m_attack_count) + time_since_last_attack) / (m_attack_count + 1);
        }
        else {
            m_player_attack_interval_avg = time_since_last_attack;
        }
        m_last_player_attack_time = now;
        m_attack_count++;
    }

    // 2. Track engagement style (crashing, mirroring, cooling)
    // tracked in bind/parry

    // 3. Track binding vs. parrying preference
    // tracked by hitbox updates

	// 4. Track close vs. retreat behavior
    if (player->get_movement().x < 0) {  // Player is moving left
        if (player->get_position().x > m_position.x) {
            m_close_count++;  // Moving towards AI
        }
        else {
            m_retreat_count++;  // Moving away from AI
        }
    }
    else if (player->get_movement().x > 0) {  // Player is moving right
        if (player->get_position().x < m_position.x) {
            m_close_count++;  // Moving towards AI
        }
        else {
            m_retreat_count++;  // Moving away from AI
        }
    }

    // Calculate final values
    oracle_calculate_aggression_rating();
    oracle_calculate_style_preference();
    oracle_calculate_bind_parry_ratio();
    oracle_calculate_close_retreat_ratio();
}



Entity::Entity()
    : m_position(0.0f), m_movement(0.0f), m_scale(1.0f, 1.0f, 0.0f), m_model_matrix(1.0f),
    m_speed(0.0f), m_animation_cols(0), m_animation_frames(0), m_animation_index(0),
    m_animation_rows(0), m_animation_indices(nullptr), m_animation_time(0.0f),
    m_texture_id(0), m_velocity(0.0f), m_acceleration(0.0f), m_width(0.0f), m_height(0.0f) {}

Entity::Entity(GLuint texture_id, float speed, glm::vec3 acceleration, float jump_power, float animation_time,
    int animation_frames, int animation_index, int animation_cols,
    int animation_rows, float width, float height, EntityType entity_type)
    : m_position(0.0f), m_movement(0.0f), m_scale(1.0f, 1.0f, 0.0f), m_model_matrix(1.0f),
    m_speed(speed), m_acceleration(acceleration), m_jumping_power(jump_power), m_animation_cols(animation_cols),
    m_animation_frames(animation_frames), m_animation_index(animation_index),
    m_animation_rows(animation_rows), m_animation_indices(nullptr),
    m_animation_time(animation_time), m_texture_id(texture_id), m_velocity(0.0f),
    m_width(width), m_height(height), m_entity_type(entity_type) {
    face_right();
}

Entity::Entity(GLuint texture_id, float speed, glm::vec3 acceleration, float jump_power, float animation_time,
    int animation_frames, int animation_index, int animation_cols,
    int animation_rows, float width, float height, EntityType entity_type, AIType ai_type, AIState ai_state, AIDifficulty ai_difficulty)
    : m_position(0.0f), m_movement(0.0f), m_scale(1.0f, 1.0f, 0.0f), m_model_matrix(1.0f),
    m_speed(speed), m_acceleration(acceleration), m_jumping_power(jump_power), m_animation_cols(animation_cols),
    m_animation_frames(animation_frames), m_animation_index(animation_index),
    m_animation_rows(animation_rows), m_animation_indices(nullptr),
    m_animation_time(animation_time), m_texture_id(texture_id), m_velocity(0.0f),
    m_width(width), m_height(height), m_entity_type(entity_type), m_ai_type(ai_type), m_ai_state(ai_state) {
    set_ai_difficulty(ai_difficulty);
    face_right();
}

Entity::Entity(GLuint texture_id, float speed, float width, float height, EntityType entity_type)
    : m_position(0.0f), m_movement(0.0f), m_scale(1.0f, 1.0f, 0.0f), m_model_matrix(1.0f),
    m_speed(speed), m_animation_cols(0), m_animation_frames(0), m_animation_index(0),
    m_animation_rows(0), m_animation_indices(nullptr), m_animation_time(0.0f),
    m_texture_id(texture_id), m_velocity(0.0f), m_acceleration(0.0f), m_width(width), m_height(height), m_entity_type(entity_type) {}

Entity::Entity(GLuint texture_id, float speed, float width, float height, EntityType entity_type, AIType ai_type, AIState ai_state, AIDifficulty ai_difficulty)
    : m_position(0.0f), m_movement(0.0f), m_scale(1.0f, 1.0f, 0.0f), m_model_matrix(1.0f),
    m_speed(speed), m_animation_cols(0), m_animation_frames(0), m_animation_index(0),
    m_animation_rows(0), m_animation_indices(nullptr), m_animation_time(0.0f),
    m_texture_id(texture_id), m_velocity(0.0f), m_acceleration(0.0f), m_width(width), m_height(height), m_entity_type(entity_type), m_ai_type(ai_type), m_ai_state(ai_state), m_ai_difficulty(ai_difficulty) {
    set_ai_difficulty(m_ai_difficulty);
}

Entity::~Entity() {}

void Entity::move_right()
{
    if (!m_is_moving && m_recovery == RECOVERY_FRAMES)
    {
        if (!m_is_attacking)
        {
			if (m_scale.x >= 0) switch_animation("run", true);
			else switch_animation("jump", true);
        }
        m_recovery = 0;
        m_face_forward = true;
        m_target_position = m_position + glm::vec3(0.4f, 0.0f, 0.0f) * glm::vec3(MAX_ATK_WEIGHT + 2 - m_atk_weight, 0.0f, 0.0f);
        m_is_moving = true;
        soundbox.play_sound("dash");
    }
}

void Entity::move_left()
{
    if (!m_is_moving && m_recovery == RECOVERY_FRAMES)
    {
        if (!m_is_attacking)
        {
            if (m_scale.x < 0) switch_animation("run", true);
            else switch_animation("jump", true);
        }
        m_recovery = 0;
        m_face_forward = false;
        m_target_position = m_position - glm::vec3(0.4f, 0.0f, 0.0f) * glm::vec3(MAX_ATK_WEIGHT + 2 - m_atk_weight, 0.0f, 0.0f);
        m_is_moving = true;
        soundbox.play_sound("dash");
    }
}

void Entity::face_right() 
{ 
    if (!m_face_forward)
    {
		m_scale.x *= -1; 
    }
	m_face_forward = true;
}

void Entity::face_left() 
{ 
    if (m_face_forward)
    {
        m_scale.x *= -1;
    }
	m_face_forward = false;
}

std::string const Entity::get_stance_str() const
{
    switch (m_atk_stance) {
		case N: return "N";
		case E: return "E";
		case S: return "S";
		case W: return "W";
		default: return "Unknown";
    }
}

void const Entity::set_hitbox(Hitbox* hitbox) { m_hitbox = hitbox; }
void const Entity::set_meter(Meter* meter) { m_meter = meter; }
// helpers
void const Entity::set_hitdata_by_animation() { m_hitbox->set_hitdata(m_current_animation); }
void const Entity::hitbox_activate() { m_hitbox->m_active = true; }
void const Entity::hitbox_deactivate() { m_hitbox->m_active = false; }

void const Entity::attack() 
{ 
	soundbox.play_unique_sound("decision");
    if (!m_is_moving) m_is_attacking = true;
    if (m_input_queue.size() < 3 && !m_is_moving) m_input_queue.push_back({ m_atk_stance, m_atk_weight });
}

bool const Entity::bind(AtkStance o_atk_stance, int o_atk_weight, bool o_adv)
{
	soundbox.play_unique_sound("bind");
    if (o_atk_stance == m_atk_stance)
    {
        m_crash_count++;
        if (m_atk_weight < o_atk_weight)
        {
            if (o_adv) take_hit();
            return true;
        }
    }
    else if (o_atk_stance % 2 == m_atk_stance % 2) // if not matching, opposite?
    {
		m_mirror_count++;
        if (m_atk_weight > o_atk_weight) 
        {
            if (o_adv) take_hit();
            return true;
        }
        if (m_atk_weight == o_atk_weight)
        {
            knockback();
            return true;
        }
    }
    else // non matching
    {
        m_cool_count++;
        return false;  // non matching bind always ends neutral
    }
    return false; // catch any condition that falls through
}

bool const Entity::parry(AtkStance o_atk_stance, int o_atk_weight, bool o_adv)
{
	soundbox.play_unique_sound("parry");
    switch_animation("counter", true);  
    if (o_atk_stance == m_atk_stance)
    {
        m_crash_count++;
        if (m_atk_weight == o_atk_weight)
        {
            knockback();
            return true;
        }
        else 
        {
            if (o_adv) take_hit();
            return true;
        }
    }
    else if (o_atk_stance % 2 == m_atk_stance % 2) // if not matching, opposite?
    {
        m_mirror_count++;
		if (o_adv) take_hit();
		return true;
    }
    else // non matching
    {
        m_cool_count++;
        if (m_atk_weight == o_atk_weight) 
        {
            if (o_adv) take_hit();
            return true;
        }
        else if (m_atk_weight < o_atk_weight)
        {
            knockback();
            return true;
        }
    }
    return false; // catch any condition that falls through
}

void const Entity::knockback()
{
	if (m_scale.x > 0) m_target_position.x = m_position.x - 0.3f;
	else m_target_position.x = m_position.x + 0.3f;
	m_is_moving = true;
	soundbox.play_sound("knockback");
    ai_action_inc(); // change master ai DEBUG
}

void const Entity::take_hit()
{
    if (!m_taking_hit)
    {
        if (!m_h_advantage)
        {
            //death(); //DEBUG
            soundbox.play_sound("hit");
            if (m_h_luck > 0) m_h_luck--;
            else if (!m_h_wounded) m_h_wounded = true;
            else death();
        }
        else 
        {
            soundbox.play_sound("adv");
            m_h_advantage--;
        }
    }
    ai_action_inc(); // change master ai DEBUG

	// Start the defense timer
	m_defense_start_time = std::chrono::steady_clock::now();
    m_taking_hit = true;
}

void const Entity::death() 
{ 
    soundbox.play_sound("death");
    m_alive = false;
    switch_animation("death", true);  
}

void Entity::set_animation(std::string animation_name, int* indices, int frames, int active_start, int active_frames) {
    m_animations[animation_name] = { std::vector<int>(indices, indices + frames), active_frames, active_start };
}

void Entity::switch_animation(std::string animation_name, bool locked) {
    if (!m_animation_lock || (animation_name == "death" && m_current_animation != "death")) {
        if (locked) m_animation_index = 0;
        m_animation_lock = locked;
        auto it = m_animations.find(animation_name);
        if (it != m_animations.end()) {
            m_current_animation = animation_name;

            // Access the indices, active_frames, and active_start from the AnimationData
            AnimationData& animData = it->second;
            m_animation_indices = animData.indices.data();
            m_animation_frames = animData.indices.size();
        }
    }
}

void Entity::draw_sprite_from_texture_atlas(ShaderProgram* program, GLuint texture_id, int index) {
    if (m_current_animation.empty() || m_animation_indices == nullptr) return;

    // Get the frame index from the current animation
    int frame_index = m_animation_indices[m_animation_index];


    float u_coord = (float)(frame_index % m_animation_cols) / (float)m_animation_cols;
    float v_coord = (float)(frame_index / m_animation_cols) / (float)m_animation_rows;

    float width = 1.0f / (float)m_animation_cols;
    float height = 1.0f / (float)m_animation_rows;

    // antiquated code
    float margin_u = 0.0f;
    float margin_v = 0.0f;

    // Adjusted UV coordinates with margin
    float tex_coords[] = {
        u_coord + margin_u,         v_coord + height - margin_v,
        u_coord + width - margin_u, v_coord + height - margin_v,

        u_coord + width - margin_u, v_coord + margin_v,
        u_coord + margin_u,         v_coord + height - margin_v,
        u_coord + width - margin_u, v_coord + margin_v,
        u_coord + margin_u,         v_coord + margin_v
    };

    float vertices[] = {
        -0.5, -0.5, 0.5, -0.5, 0.5, 0.5,
        -0.5, -0.5, 0.5, 0.5, -0.5, 0.5
    };

    glBindTexture(GL_TEXTURE_2D, texture_id);

    glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->get_position_attribute());

    glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, tex_coords);

    glEnableVertexAttribArray(program->get_tex_coordinate_attribute());

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(program->get_position_attribute());
    glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
}

bool const Entity::check_collision(Entity* other) const {
    float left_margin_adjustment = m_margin[0][0];
    float right_margin_adjustment = m_margin[0][1];
    float up_margin_adjustment = m_margin[1][0];
    float down_margin_adjustment = m_margin[1][1];

    float adjusted_width = m_width - left_margin_adjustment - right_margin_adjustment;
    float adjusted_height = m_height - up_margin_adjustment - down_margin_adjustment;

    float other_left_margin_adjustment = other->m_margin[0][0];
    float other_right_margin_adjustment = other->m_margin[0][1];
    float other_up_margin_adjustment = other->m_margin[1][0];
    float other_down_margin_adjustment = other->m_margin[1][1];

    float other_adjusted_width = other->m_width - other_left_margin_adjustment - other_right_margin_adjustment;
    float other_adjusted_height = other->m_height - other_up_margin_adjustment - other_down_margin_adjustment;

    float x_distance = fabs((m_position.x + left_margin_adjustment - right_margin_adjustment) -
        (other->m_position.x + other_left_margin_adjustment - other_right_margin_adjustment))
        - ((adjusted_width + other_adjusted_width) / 2.0f);
    float y_distance = fabs((m_position.y + up_margin_adjustment - down_margin_adjustment) -
        (other->m_position.y + other_up_margin_adjustment - other_down_margin_adjustment))
        - ((adjusted_height + other_adjusted_height) / 2.0f);

    return x_distance < 0.0f && y_distance < 0.0f;
}

void const Entity::check_collision_x(Entity* collidable_entities, int collidable_entity_count) {
    float left_margin_adjustment = m_margin[0][0];
    float right_margin_adjustment = m_margin[0][1];
    float adjusted_width = m_width - left_margin_adjustment - right_margin_adjustment;

    for (int i = 0; i < collidable_entity_count; i++) {
        Entity* collidable_entity = &collidable_entities[i];
        if (!collidable_entity->m_is_active) return;
        if (collidable_entity == this) continue;

        float other_left_margin_adjustment = collidable_entity->m_margin[0][0];
        float other_right_margin_adjustment = collidable_entity->m_margin[0][1];
        float other_adjusted_width = collidable_entity->m_width - other_left_margin_adjustment - other_right_margin_adjustment;

        if (check_collision(collidable_entity)) {
            float x_distance = fabs((m_position.x + left_margin_adjustment - right_margin_adjustment) -
                (collidable_entity->m_position.x + other_left_margin_adjustment - other_right_margin_adjustment));
            float x_overlap = fabs(x_distance - (adjusted_width / 2.0f) - (other_adjusted_width / 2.0f));
            if (m_velocity.x > 0) {
                m_position.x -= x_overlap;
                m_velocity.x = 0;
                m_collided_right = true;
            }
            else if (m_velocity.x < 0) {
                m_position.x += x_overlap;
                m_velocity.x = 0;
                m_collided_left = true;
            }
        }
    }
}

void const Entity::check_collision_y(Entity* collidable_entities, int collidable_entity_count) {
    float up_margin_adjustment = m_margin[1][0];
    float down_margin_adjustment = m_margin[1][1];
    float adjusted_height = m_height - up_margin_adjustment - down_margin_adjustment;

    for (int i = 0; i < collidable_entity_count; i++) {
        Entity* collidable_entity = &collidable_entities[i];
        if (!collidable_entity->m_is_active) return;
        if (collidable_entity == this) continue;

        float other_up_margin_adjustment = collidable_entity->m_margin[1][0];
        float other_down_margin_adjustment = collidable_entity->m_margin[1][1];
        float other_adjusted_height = collidable_entity->m_height - other_up_margin_adjustment - other_down_margin_adjustment;

        if (check_collision(collidable_entity)) {
            float y_distance = fabs((m_position.y + up_margin_adjustment - down_margin_adjustment) -
                (collidable_entity->m_position.y + other_up_margin_adjustment - other_down_margin_adjustment));
            float y_overlap = fabs(y_distance - (adjusted_height / 2.0f) - (other_adjusted_height / 2.0f));

            if (m_velocity.y > 0) {
                m_position.y -= y_overlap;
                m_velocity.y = 0;
                m_collided_top = true;
            }
            else if (m_velocity.y < 0) {
                m_position.y += y_overlap;
                m_velocity.y = 0;
                m_collided_bottom = true;
            }
        }
    }
}

void const Entity::check_collision_y(Map* map) {
    float up_margin_adjustment = m_margin[1][0];
    float down_margin_adjustment = m_margin[1][1];
    float adjusted_height = m_height - up_margin_adjustment - down_margin_adjustment;
    float adjusted_width = m_width - (m_margin[0][0] + m_margin[0][1]);

    glm::vec3 top = glm::vec3(m_position.x, m_position.y + (adjusted_height / 2) - up_margin_adjustment, m_position.z);
    glm::vec3 top_left = glm::vec3(m_position.x - (adjusted_width / 2), m_position.y + (adjusted_height / 2) - up_margin_adjustment, m_position.z);
    glm::vec3 top_right = glm::vec3(m_position.x + (adjusted_width / 2), m_position.y + (adjusted_height / 2) - up_margin_adjustment, m_position.z);

    glm::vec3 bottom = glm::vec3(m_position.x, m_position.y - (adjusted_height / 2) + down_margin_adjustment, m_position.z);
    glm::vec3 bottom_left = glm::vec3(m_position.x - (adjusted_width / 2), m_position.y - (adjusted_height / 2) + down_margin_adjustment, m_position.z);
    glm::vec3 bottom_right = glm::vec3(m_position.x + (adjusted_width / 2), m_position.y - (adjusted_height / 2) + down_margin_adjustment, m_position.z);

    float penetration_x = 0;
    float penetration_y = 0;

    if (map->is_solid(top, &penetration_x, &penetration_y) && m_velocity.y > 0) {
        m_position.y -= penetration_y;
        m_velocity.y = 0;
        m_collided_top = true;
    }
    else if (map->is_solid(top_left, &penetration_x, &penetration_y) && m_velocity.y > 0) {
        m_position.y -= penetration_y;
        m_velocity.y = 0;
        m_collided_top = true;
    }
    else if (map->is_solid(top_right, &penetration_x, &penetration_y) && m_velocity.y > 0) {
        m_position.y -= penetration_y;
        m_velocity.y = 0;
        m_collided_top = true;
    }

    if (map->is_solid(bottom, &penetration_x, &penetration_y) && m_velocity.y < 0) {
        m_position.y += penetration_y;
        m_velocity.y = 0;
        m_collided_bottom = true;
    }
    else if (map->is_solid(bottom_left, &penetration_x, &penetration_y) && m_velocity.y < 0) {
        m_position.y += penetration_y;
        m_velocity.y = 0;
        m_collided_bottom = true;
    }
    else if (map->is_solid(bottom_right, &penetration_x, &penetration_y) && m_velocity.y < 0) {
        m_position.y += penetration_y;
        m_velocity.y = 0;
        m_collided_bottom = true;
    }
}

void const Entity::check_collision_x(Map* map) {
    float left_margin_adjustment = m_margin[0][0];
    float right_margin_adjustment = m_margin[0][1];
    float adjusted_height = m_height - (m_margin[1][0] + m_margin[1][1]);
    float adjusted_width = m_width - left_margin_adjustment - right_margin_adjustment;

    glm::vec3 left = glm::vec3(m_position.x - (adjusted_width / 2) + left_margin_adjustment, m_position.y, m_position.z);
    glm::vec3 right = glm::vec3(m_position.x + (adjusted_width / 2) - right_margin_adjustment, m_position.y, m_position.z);

    float penetration_x = 0;
    float penetration_y = 0;

    if (map->is_solid(left, &penetration_x, &penetration_y) && m_velocity.x < 0) {
        m_position.x += penetration_x;
        m_velocity.x = 0;
        m_collided_left = true;
    }
    if (map->is_solid(right, &penetration_x, &penetration_y) && m_velocity.x > 0) {
        m_position.x -= penetration_x;
        m_velocity.x = 0;
        m_collided_right = true;
    }
}

void Entity::update(float delta_time, Entity* player, Entity* collidable_entities, int collidable_entity_count, Map* map) {
    if (!m_is_active) return;
    switch_animation("idle", false);
    //if (m_is_attacking && m_meter->m_frame == 0)
    if (m_is_attacking && m_meter->m_frame == 4)  // hardcode change for visual timing
    {
		soundbox.play_sound("swing");
		if (m_atk_stance < 2) switch_animation("attack", true);  
		else switch_animation("attack2", true);  
        if (m_input_queue.size() < 2) m_is_attacking = false;
    }
    // clear input queue if input didn't go through
    if (!m_is_attacking && m_input_queue.size() < 0)  m_input_queue.clear();
    set_hitdata_by_animation(); 

    m_collided_top = false;
    m_collided_bottom = false;
    m_collided_left = false;
    m_collided_right = false;

    if (m_entity_type == ENEMY) ai_activate(player);

    if (m_animation_indices != nullptr && !m_current_animation.empty()) {
        m_animation_time += delta_time;
        float frames_per_second = (float)1 / SECONDS_PER_FRAME;

        if (m_animation_time >= frames_per_second) {
            m_animation_time = 0.0f;
            m_animation_index++;
            if (m_recovery < RECOVERY_FRAMES && !m_is_moving) m_recovery++;
			if (m_entity_type == ENEMY) ai_oracle(player); // run oracle only by frame

            // Check if we are within the active frames
            auto it = m_animations.find(m_current_animation);
            if (it != m_animations.end()) {
                int active_start = it->second.active_start;
                int active_end = active_start + it->second.active_frames;

                if (m_animation_index >= active_start && m_animation_index < active_end) {
                    // The frame is active
                    m_hitbox->set_active(true);
                }
                else {
                    m_hitbox->set_active(false);
                    player->set_hit_flag(false); //when known safe?
                }
            }

            if (m_animation_index >= m_animation_frames) {
                if (m_current_animation == "attack" || m_current_animation == "attack2") m_input_queue.pop_front();
                m_animation_lock = false;
                switch_animation("idle", false);
                m_animation_index = 0; // Loop back to the first frame
                m_hitbox->set_active(false);
                if (!m_alive) deactivate();  // if animation has finished and no longer alive
            }
        }
    }

    //m_velocity.x = m_movement.x * m_speed;
    m_velocity += m_acceleration * delta_time;

    if (m_is_jumping) {
        m_is_jumping = false;
        m_velocity.y += m_jumping_power;
    }

    //m_position.x += m_velocity.x * delta_time;
    check_collision_x(collidable_entities, collidable_entity_count);
    check_collision_x(map);

    m_position.y += m_velocity.y * delta_time;
    check_collision_y(collidable_entities, collidable_entity_count);
    check_collision_y(map);

    if (m_position.x != m_target_position.x && m_is_moving)
    {
        if (m_position.x < m_target_position.x)
        {
            m_movement.x = 1.0f;
            if (m_face_forward) m_position.x += m_movement.x * m_speed * delta_time;
            else m_position.x = m_target_position.x;
        }
        else {
            m_movement.x = -1.0f;
            if (!m_face_forward) m_position.x += m_movement.x * m_speed * delta_time;
            else m_position.x = m_target_position.x;
        }
    }
    if (m_position.x == m_target_position.x) 
    {
        m_is_moving = false;
        m_movement.x = 0.0f;
    }


    m_model_matrix = glm::mat4(1.0f);
    m_model_matrix = glm::translate(m_model_matrix, m_position);
    m_model_matrix = glm::scale(m_model_matrix, m_scale); // DEBUG
}

void Entity::render(ShaderProgram* program) {
    if (!m_is_active) return;
    program->set_model_matrix(m_model_matrix);

    if (m_animation_indices != NULL) {
        draw_sprite_from_texture_atlas(program, m_texture_id, m_animation_indices[m_animation_index]);
        return;
    }

    float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float tex_coords[] = { 0.0,  1.0, 1.0,  1.0, 1.0, 0.0,  0.0,  1.0, 1.0, 0.0,  0.0, 0.0 };

    glBindTexture(GL_TEXTURE_2D, m_texture_id);

    glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->get_position_attribute());
    glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, tex_coords);
    glEnableVertexAttribArray(program->get_tex_coordinate_attribute());

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(program->get_position_attribute());
    glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
}

