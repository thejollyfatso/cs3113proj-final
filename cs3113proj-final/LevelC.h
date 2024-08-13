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
    GLuint m_font_texture_id;

    // PROGRESS //
    float post_player_start = 31.0f;
    bool goal_move_left = false;
    bool goal_move_right = false;
    AtkStance post_player_prev_stance = N;
    int goal_change_stance = 0;
    bool goal_max_weight = false;
    bool goal_min_weight = false;
    bool goal_attack = false;

    
    
    // ————— DESTRUCTOR ————— //
    ~LevelC();
    
    // ————— METHODS ————— //
    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram *program) override;
};
