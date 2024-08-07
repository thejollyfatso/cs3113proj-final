#include "Level0.h"
#include "Utility.h"

Level0::~Level0()
{
    delete [] m_game_state.enemies;
    delete [] m_game_state.hitboxes;
    delete [] m_game_state.widgets;
    delete    m_game_state.player;
    delete    m_game_state.player2;
    delete    m_game_state.map;
    delete    m_game_state.meter;
    Mix_FreeChunk(m_game_state.jump_sfx);
    Mix_FreeMusic(m_game_state.bgm);
}

void Level0::initialise()
{
	m_game_state.next_scene_id = -1;
    m_font_texture_id = Utility::load_texture("assets/font1.png");

    m_game_state.enemies = new Entity;
    m_game_state.hitboxes = new Hitbox;
    m_game_state.widgets = new UIElem;
    m_game_state.player = new Entity;
    m_game_state.player2 = new Entity;
    m_game_state.map = new Map;
    m_game_state.meter = new Meter;
}

void Level0::update(float delta_time)
{
}


void Level0::render(ShaderProgram *g_shader_program)
{
	Utility::draw_text(g_shader_program, m_font_texture_id, "lorem ipsum", 0.5f, 0.05f,
		glm::vec3(-0.5f, -1.0f, 0.0f)); // position according to player
}
