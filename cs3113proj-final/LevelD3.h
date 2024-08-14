#include "Scene.h"

class LevelD3 : public Scene {
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

    Dialog* dialogue;
    bool post_level_switch = false;
    
    
    // ————— DESTRUCTOR ————— //
    ~LevelD3();
    
    // ————— METHODS ————— //
    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram *program) override;
};
