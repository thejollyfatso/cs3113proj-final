#include "Scene.h"
enum MenuState { MAIN_MENU, DIFFICULTY_SELECTION };

class Level0 : public Scene {
public:
    // ————— STATIC ATTRIBUTES ————— //
    GLuint m_font_texture_id;

    MenuState m_current_menu_state;
    
    // ————— DESTRUCTOR ————— //
    ~Level0();
    
    // ————— METHODS ————— //
    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram *program) override;
};
