#include "Scene.h"

class LevelC : public Scene {
public:
    // ————— STATIC ATTRIBUTES ————— //
    int ENEMY_COUNT = 0;
    int m_enemies_left = ENEMY_COUNT;
    int WIDGET_COUNT = 12;
    GLuint m_ui_texture_id;
    GLuint m_ui_texture_id2;
    GLuint m_ui_texture_id3;
    GLuint m_dialog_texture_id;
    GLuint m_font_texture_id;

    // PROGRESS //
    Dialog* dialogue;
    float post_player_start = 31.0f;
    bool goal_move_left = false;
    bool goal_move_right = false;
    AtkStance post_player_prev_stance = N;
    int goal_change_stance = 0;
    bool goal_max_weight = false;
    bool goal_min_weight = false;
    bool goal_attack = false;
    bool post_level_switch = false;

    // Progress audio
    AudioManager soundbox;
    bool goal_move_played = false;
    bool goal_stance_played = false;
    bool goal_weight_played = false;
    bool goal_attack_played = false;
    void goal_listener_audio()
    {
        if (!goal_move_played && goal_move_left && goal_move_right)
        {
            soundbox.play_sound("correct");
            goal_move_played = true;
        }
        if (!goal_stance_played && goal_change_stance >= 6)
        {
            soundbox.play_sound("correct");
            goal_stance_played = true;
        }
        if (!goal_weight_played && goal_max_weight && goal_min_weight) 
        {
            soundbox.play_sound("correct");
            goal_weight_played = true;
        }
        if (!goal_attack_played && goal_attack) 
        {
            soundbox.play_sound("correct");
            goal_attack_played = true;
        }
    }
    
    
    // ————— DESTRUCTOR ————— //
    ~LevelC();
    
    // ————— METHODS ————— //
    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram *program) override;
};
