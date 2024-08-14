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
    m_current_menu_state = MAIN_MENU;
	m_game_state.next_scene_id = -1;
    m_font_texture_id = Utility::load_texture("assets/font1.png");

    m_game_state.enemies = new Entity;
    m_game_state.hitboxes = new Hitbox;
    m_game_state.widgets = new UIElem;
    m_game_state.player = new Entity;
    m_game_state.player2 = new Entity;
    m_game_state.map = new Map;
    m_game_state.meter = new Meter;

    m_game_state.bgm = Mix_LoadMUS("assets/sound/japan-origami.mp3");
    Mix_PlayMusic(m_game_state.bgm, -1);
    Mix_VolumeMusic(MIX_MAX_VOLUME / 2);
}

void Level0::update(float delta_time)
{
}


void Level0::render(ShaderProgram *g_shader_program)
{
    if (m_current_menu_state == MAIN_MENU) {
        Utility::draw_text(g_shader_program, m_font_texture_id, "MAIN MENU", 0.5f, 0.05f,
            glm::vec3(2.8f, -1.8f, 0.0f));
        Utility::draw_text(g_shader_program, m_font_texture_id, "1: Multiplayer", 0.5f, 0.01f,
            glm::vec3(1.6f, -2.8f, 0.0f));
        Utility::draw_text(g_shader_program, m_font_texture_id, "2: Duel vs CPU", 0.5f, 0.01f,
            glm::vec3(1.6f, -3.6f, 0.0f));
        Utility::draw_text(g_shader_program, m_font_texture_id, "3: Tutorial", 0.5f, 0.01f,
            glm::vec3(1.6f, -4.4f, 0.0f));
        Utility::draw_text(g_shader_program, m_font_texture_id, "4: Progression", 0.5f, 0.01f,
            glm::vec3(1.6f, -5.2f, 0.0f));
        Utility::draw_text(g_shader_program, m_font_texture_id, "X: Exit", 0.5f, 0.01f,
            glm::vec3(1.6f, -6.0f, 0.0f));
    }
    else if (m_current_menu_state == DIFFICULTY_SELECTION) {
        Utility::draw_text(g_shader_program, m_font_texture_id, "DIFFICULTY", 0.5f, 0.05f,
            glm::vec3(2.4f, -2.0f, 0.0f));
        Utility::draw_text(g_shader_program, m_font_texture_id, "1: Easy", 0.5f, 0.01f,
            glm::vec3(1.6f, -3.0f, 0.0f));
        Utility::draw_text(g_shader_program, m_font_texture_id, "2: Medium", 0.5f, 0.01f,
            glm::vec3(1.6f, -3.8f, 0.0f));
        Utility::draw_text(g_shader_program, m_font_texture_id, "3: Hard", 0.5f, 0.01f,
            glm::vec3(1.6f, -4.6f, 0.0f));
        Utility::draw_text(g_shader_program, m_font_texture_id, "X: Back", 0.5f, 0.01f,
            glm::vec3(1.6f, -5.4f, 0.0f));
    }
}
