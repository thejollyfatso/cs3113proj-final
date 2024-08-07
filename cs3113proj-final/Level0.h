#include "Scene.h"

class Level0 : public Scene {
public:
    // ————— STATIC ATTRIBUTES ————— //
    GLuint m_font_texture_id;
    
    // ————— DESTRUCTOR ————— //
    ~Level0();
    
    // ————— METHODS ————— //
    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram *program) override;
};
