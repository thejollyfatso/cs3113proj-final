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
	Utility::draw_text(g_shader_program, m_font_texture_id, "MAIN MENU", 0.5f, 0.05f,
		glm::vec3(2.8f, -2.0f, 0.0f));
	Utility::draw_text(g_shader_program, m_font_texture_id, "1: Multiplayer", 0.5f, 0.01f,
		glm::vec3(2.0f, -3.0f, 0.0f));
	Utility::draw_text(g_shader_program, m_font_texture_id, "2: Duel vs CPU", 0.5f, 0.01f,
		glm::vec3(2.0f, -3.8f, 0.0f));
	Utility::draw_text(g_shader_program, m_font_texture_id, "3: Tutorial", 0.5f, 0.01f,
		glm::vec3(2.0f, -4.6f, 0.0f));
}
