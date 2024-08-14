#include "LevelD.h"
#include "Utility.h"

#define LEVEL_WIDTH 60
#define LEVEL_HEIGHT 8

constexpr char SPRITESHEET_FILEPATH[] = "assets/spritesheet.png",
           PLATFORM_FILEPATH[]    = "assets/platformPack_tile027.png",
           ENEMY_FILEPATH[]       = "assets/spritesheet_prog1.png";

unsigned int LEVEL5_DATA[] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 67, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 70, 0, 0, 0, 0, 0,
    0, 0, 0, 127, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 128, 130, 0, 0, 0, 0, 0
};

LevelD::~LevelD()
{
    delete [] m_game_state.enemies;
    delete [] m_game_state.hitboxes;
    delete [] m_game_state.widgets;
    delete    m_game_state.player;
    delete    m_game_state.player2;
    delete    m_game_state.map;
    delete    m_game_state.meter;
    delete dialogue;
    Mix_FreeChunk(m_game_state.jump_sfx);
    Mix_FreeMusic(m_game_state.bgm);
}

void LevelD::initialise()
{
    m_game_state.meter = new Meter;
    m_font_texture_id = Utility::load_texture("assets/font1.png");
    GLuint map_texture_id = Utility::load_texture("assets/sunnyland/tileset.png");
    m_game_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVEL5_DATA, map_texture_id, 1.0f, 20, 12);
    
    GLuint player_texture_id = Utility::load_texture(SPRITESHEET_FILEPATH);
    GLuint player2_texture_id = Utility::load_texture(ENEMY_FILEPATH);

    glm::vec3 acceleration = glm::vec3(0.0f, -9.81f, 0.0f);

    m_game_state.player = new Entity(
        player_texture_id,         // texture id
        5.0f,                      // speed
        acceleration,              // acceleration
        6.0f,                      // jumping power
        0.0f,                      // animation time
        8,                         // animation frame amount
        0,                         // current animation index
        8,                         // animation column amount
        8,                         // animation row amount
        2.0f,                      // width
        2.0f,                       // height
        PLAYER
    );

    // enlarge
    glm::vec3 new_scale = { 4.0f, 4.0f, 0.0f };
    m_game_state.player->set_scale(new_scale);
    m_game_state.player->set_margin_y(glm::vec2(0.8f, 0.1f)); // trim sprite
    m_game_state.player->set_margin_x(glm::vec2(0.5f, 0.5f)); // trim sprite
    
    // set animations
    int run_animation[] = { 49, 50, 51 };
    int idle_animation[] = { 8, 8, 9, 9 };
    int counter_animation[] = { 3, 3, 1, 0 };
    int attack_animation[] = { 8, 9, 10, 11 };
    int attack2_animation[] = { 0, 1, 2, 3 };
    int death_animation[] = { 16, 17, 18, 19, 20, 21, 22 };
    int jump_animation[] = { 24, 25 };

    m_game_state.player->set_animation("run", run_animation, 3, 0, 0);
    m_game_state.player->set_animation("idle", idle_animation, 4, 0, 0);
    m_game_state.player->set_animation("counter", counter_animation, 4, 0, 0);
    m_game_state.player->set_animation("attack", attack_animation, 4, 3, 1);
    m_game_state.player->set_animation("attack2", attack2_animation, 4, 3, 1);
    m_game_state.player->set_animation("death", death_animation, 7, 0, 0);
    m_game_state.player->set_animation("jump", jump_animation, 2, 0, 0);
    m_game_state.player->switch_animation("idle", true); // start with idle

    m_game_state.player->set_position(glm::vec3(31.0f, 0.0f, 0.0f));

    // PLAYER 2
    AIDifficulty difficulty = MEDIUM;
    /*
	switch (selected_difficulty)
	{
	case 1:
		difficulty = EASY;
		break;
	case 2:
		difficulty = MEDIUM;
		break;
	case 3:
		difficulty = HARD;
		break;
	default:
		break;
	}
    */
    m_game_state.player2 = new Entity(
        player2_texture_id,         // texture id
        5.0f,                      // speed
        acceleration,              // acceleration
        6.0f,                      // jumping power
        0.0f,                      // animation time
        8,                         // animation frame amount
        0,                         // current animation index
        9,                         // animation column amount
        8,                         // animation row amount
        2.0f,                      // width
        2.0f,                       // height
        ENEMY,
        MIRROR,
        IDLE,
        difficulty
    );

    // enlarge
    new_scale = { 2.0f, 2.0f, 0.0f };
    m_game_state.player2->set_scale(new_scale);
    m_game_state.player2->set_margin_y(glm::vec2(-0.2f, 0.1f)); // trim sprite
    m_game_state.player2->set_margin_x(glm::vec2(0.5f, 0.5f)); // trim sprite
    
    int run_animation2[] = { 54, 56, 58 };
    int idle_animation2[] = { 36, 37, 38, 39, 40, 41, 42 };
    int counter_animation2[] = { 10, 12, 3, 3 };
    int attack_animation2[] = { 0, 1, 2, 3 };
    int attack2_animation2[] = { 9, 10, 11, 12 };
    int death_animation2[] = { 18, 19, 20, 21, 22, 23 };
    int jump_animation2[] = { 49, 50 };
    
    m_game_state.player2->set_animation("run", run_animation2, 3, 0, 0);
    m_game_state.player2->set_animation("idle", idle_animation2, 7, 0, 0);
    m_game_state.player2->set_animation("counter", counter_animation2, 4, 0, 0);
    m_game_state.player2->set_animation("attack", attack_animation2, 4, 3, 1);
    m_game_state.player2->set_animation("attack2", attack2_animation2, 4, 3, 1);
    m_game_state.player2->set_animation("death", death_animation2, 6, 0, 0);
    m_game_state.player2->set_animation("jump", jump_animation2, 2, 0, 0);
    m_game_state.player2->switch_animation("idle", true); // start with idle

    m_game_state.player2->set_position(glm::vec3(34.0f, 0.0f, 0.0f));
    m_game_state.player2->face_left();
    
    /* Set Meters */
    m_game_state.player->set_meter(m_game_state.meter);
    m_game_state.player2->set_meter(m_game_state.meter);
    /* Create Hitboxes and Hurtboxes */
    GLuint hitbox_texture_id = Utility::load_texture("assets/hitbox.png");

    // create array
    m_game_state.hitboxes = new Hitbox[2];

    // create hitboxes for each enemy then player
    m_game_state.hitboxes[0] = Hitbox(hitbox_texture_id, m_game_state.player);
    m_game_state.hitboxes[1] = Hitbox(hitbox_texture_id, m_game_state.player2);

    // set player hitbox and add hitdata
    m_game_state.player->set_hitbox(&m_game_state.hitboxes[0]);
    m_game_state.player2->set_hitbox(&m_game_state.hitboxes[1]);

    glm::vec3 hb_scale = { 1.8f, 1.4f, 1.0f };
    glm::vec3 hb_offset = { 0.8f, 0.2f, 0.0f };
    m_game_state.player->get_hitbox()->add_hitdata("attack", hb_scale, hb_offset);
    hb_scale = { 1.2f, 1.1f, 1.0f };
    hb_offset = { 0.4f, -0.44f, 0.0f };
    m_game_state.player2->get_hitbox()->add_hitdata("attack", hb_scale, hb_offset);
    hb_scale = { 1.7f, 1.2f, 1.0f };
    hb_offset = { 0.8f, 0.0f, 0.0f };
    m_game_state.player->get_hitbox()->add_hitdata("attack2", hb_scale, hb_offset);
    hb_scale = { 1.2f, 1.1f, 1.0f };
    hb_offset = { 0.4f, -0.44f, 0.0f };
    m_game_state.player2->get_hitbox()->add_hitdata("attack2", hb_scale, hb_offset);
    hb_scale = { 0.6f, 1.1f, 1.0f };
    hb_offset = { 0.16f, 0.0f, 0.0f };
    m_game_state.player->get_hitbox()->add_hitdata("idle", hb_scale, hb_offset);
    hb_scale = { 0.4f, 1.1f, 1.0f };
    hb_offset = { -0.2f, -0.44f, 0.0f };
    m_game_state.player2->get_hitbox()->add_hitdata("idle", hb_scale, hb_offset);
    hb_scale = { 0.6f, 1.1f, 1.0f };
    hb_offset = { 0.0f, 0.0f, 0.0f };
    m_game_state.player->get_hitbox()->add_hitdata("run", hb_scale, hb_offset);
    m_game_state.player->get_hitbox()->add_hitdata("jump", hb_scale, hb_offset);
    hb_scale = { 0.4f, 1.1f, 1.0f };
    hb_offset = { -0.2f, -0.44f, 0.0f };
    m_game_state.player2->get_hitbox()->add_hitdata("run", hb_scale, hb_offset);
    m_game_state.player2->get_hitbox()->add_hitdata("counter", hb_scale, hb_offset);
    hb_offset = { -0.3f, -0.44f, 0.0f };
    m_game_state.player2->get_hitbox()->add_hitdata("jump", hb_scale, hb_offset);

    // UI Elements
    m_game_state.widgets = new UIElem[WIDGET_COUNT];
    m_ui_texture_id = Utility::load_texture("assets/00.png");
    m_ui_texture_id2 = Utility::load_texture("assets/07.png");
    m_ui_texture_id3 = Utility::load_texture("assets/hearts.png");

    m_game_state.widgets[0] = UIElem(m_ui_texture_id, 20, 10, m_game_state.player, STANCE);
    int test_animation[] = { 55 };
    m_game_state.widgets[0].set_animation("idle", test_animation, 1);
    m_game_state.widgets[0].switch_animation("idle", false);
    m_game_state.widgets[0].m_offset = glm::vec3(-0.5f, 1.0f, 0.0f);

    m_game_state.widgets[1] = UIElem(m_ui_texture_id, 20, 10, &m_game_state.widgets[0], WEIGHT);
    int test_animation2[] = { 135 };
    int test_animation3[] = { 136 };
    m_game_state.widgets[1].set_animation("idle", test_animation2, 1);
    m_game_state.widgets[1].set_animation("active", test_animation3, 1);
    m_game_state.widgets[1].switch_animation("active", false);
    m_game_state.widgets[1].m_offset = glm::vec3(1.0f, 0.1f, 0.0f);

    m_game_state.widgets[2] = UIElem(m_ui_texture_id, 20, 10, &m_game_state.widgets[1], WEIGHT);
    m_game_state.widgets[2].set_animation("idle", test_animation2, 1);
    m_game_state.widgets[2].set_animation("active", test_animation3, 1);
    m_game_state.widgets[2].switch_animation("idle", false);
    m_game_state.widgets[2].m_offset = glm::vec3(0.19f, 0.0f, 0.0f);

    m_game_state.widgets[3] = UIElem(m_ui_texture_id, 20, 10, &m_game_state.widgets[2], WEIGHT);
    m_game_state.widgets[3].set_animation("idle", test_animation2, 1);
    m_game_state.widgets[3].set_animation("active", test_animation3, 1);
    m_game_state.widgets[3].switch_animation("idle", false);
    m_game_state.widgets[3].m_offset = glm::vec3(0.19f, 0.0f, 0.0f);

    m_game_state.widgets[4] = UIElem(m_ui_texture_id, 20, 10, m_game_state.player2, STANCE);
    m_game_state.widgets[4].set_animation("idle", test_animation, 1);
    m_game_state.widgets[4].switch_animation("idle", false);
    m_game_state.widgets[4].m_offset = glm::vec3(-0.5f, 0.5f, 0.0f);

    m_game_state.widgets[5] = UIElem(m_ui_texture_id, 20, 10, &m_game_state.widgets[4], WEIGHT);
    m_game_state.widgets[5].set_animation("idle", test_animation2, 1);
    m_game_state.widgets[5].set_animation("active", test_animation3, 1);
    m_game_state.widgets[5].switch_animation("active", false);
    m_game_state.widgets[5].m_offset = glm::vec3(1.0f, 0.1f, 0.0f);

    m_game_state.widgets[6] = UIElem(m_ui_texture_id, 20, 10, &m_game_state.widgets[5], WEIGHT);
    m_game_state.widgets[6].set_animation("idle", test_animation2, 1);
    m_game_state.widgets[6].set_animation("active", test_animation3, 1);
    m_game_state.widgets[6].switch_animation("idle", false);
    m_game_state.widgets[6].m_offset = glm::vec3(0.19f, 0.0f, 0.0f);

    m_game_state.widgets[7] = UIElem(m_ui_texture_id, 20, 10, &m_game_state.widgets[6], WEIGHT);
    m_game_state.widgets[7].set_animation("idle", test_animation2, 1);
    m_game_state.widgets[7].set_animation("active", test_animation3, 1);
    m_game_state.widgets[7].switch_animation("idle", false);
    m_game_state.widgets[7].m_offset = glm::vec3(0.19f, 0.0f, 0.0f);

    // Meter
    int test_animation4[] = { 18 };
    m_game_state.widgets[8] = UIElem(m_ui_texture_id2, 9, 18, m_game_state.player, METER);
    m_game_state.widgets[8].set_animation("idle", test_animation4, 1);
    m_game_state.widgets[8].switch_animation("idle", false);
    //m_game_state.widgets[8].m_offset = glm::vec3((-(m_game_state.player->get_position().x)/2)+((m_game_state.player2->get_position().x)/2), 1.6f, 0.0f);

    int test_animation5[] = { 25 };
    m_game_state.widgets[9] = UIElem(m_ui_texture_id2, 9, 18, m_game_state.player, METER);
    m_game_state.widgets[9].set_animation("idle", test_animation5, 1);
    m_game_state.widgets[9].switch_animation("idle", false);

    // Health indication
    int adv_state[] = { 143 };
    int luck3_state[] = { 70 };
    int luck2_state[] = { 68 };
    int luck1_state[] = { 67 };
    int health_state[] = { 130 };
    int wounded_state[] = { 52 };
    int dead_state[] = { 0 };
    m_game_state.widgets[10] = UIElem(m_ui_texture_id3, 13, 20, m_game_state.player, HEATLH);
    m_game_state.widgets[10].set_animation("adv", adv_state, 1);
    m_game_state.widgets[10].set_animation("luck3", luck3_state, 1);
    m_game_state.widgets[10].set_animation("luck2", luck2_state, 1);
    m_game_state.widgets[10].set_animation("luck1", luck1_state, 1);
    m_game_state.widgets[10].set_animation("health", health_state, 1);
    m_game_state.widgets[10].set_animation("wounded", wounded_state, 1);
    m_game_state.widgets[10].set_animation("dead", dead_state, 1);
    m_game_state.widgets[10].switch_animation("adv", false);
    m_game_state.widgets[10].m_offset = glm::vec3(1.1f, 0.95f, 0.0f);

    m_game_state.widgets[11] = UIElem(m_ui_texture_id3, 13, 20, m_game_state.player2, HEATLH);
    m_game_state.widgets[11].set_animation("adv", adv_state, 1);
    m_game_state.widgets[11].set_animation("luck3", luck3_state, 1);
    m_game_state.widgets[11].set_animation("luck2", luck2_state, 1);
    m_game_state.widgets[11].set_animation("luck1", luck1_state, 1);
    m_game_state.widgets[11].set_animation("health", health_state, 1);
    m_game_state.widgets[11].set_animation("wounded", wounded_state, 1);
    m_game_state.widgets[11].set_animation("dead", dead_state, 1);
    m_game_state.widgets[11].switch_animation("adv", false);
    m_game_state.widgets[11].m_offset = glm::vec3(1.1f, 0.45f, 0.0f);

    /**
     BGM and SFX
     */
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    
    m_game_state.bgm = Mix_LoadMUS("assets/sound/lightning-tiger-god-loop.mp3");
    Mix_PlayMusic(m_game_state.bgm, -1);
    //Mix_VolumeMusic(MIX_MAX_VOLUME / 2);
    
    m_game_state.jump_sfx = Mix_LoadWAV("assets/bounce.wav");

    // Dialog
    GLuint m_dialog_texture_id = Utility::load_texture("assets/dialogue/D/d_spritesheet.png");
    dialogue = new Dialog(m_dialog_texture_id, 5, 1);
    dialogue->m_animation_index = 0;
    dialogue->m_entity = m_game_state.player2;
    dialogue->m_scale = glm::vec3(4.8f);
    dialogue->m_offset = glm::vec3(-0.4f, 3.0f, 0.0f);
}

void LevelD::update(float delta_time)
{
    // players face each other
    if ((m_game_state.player->get_position().x < m_game_state.player2->get_position().x &&
        m_game_state.player->get_scale().x < 0) ||
        (m_game_state.player->get_position().x > m_game_state.player2->get_position().x &&
        m_game_state.player->get_scale().x > 0))
    {
        m_game_state.player->horizontal_mirror();
        m_game_state.player2->horizontal_mirror();
    }

    m_game_state.meter->update(delta_time);

    m_game_state.player->update(delta_time, m_game_state.player2, m_game_state.enemies, ENEMY_COUNT, m_game_state.map);
    //m_game_state.player2->update(delta_time, m_game_state.player2, m_game_state.enemies, ENEMY_COUNT, m_game_state.map);
    m_game_state.player2->update(delta_time, m_game_state.player, m_game_state.enemies, ENEMY_COUNT, m_game_state.map); // ai relates to player
    
	m_game_state.hitboxes[0].update(delta_time, m_game_state.player2->get_hitbox());
	m_game_state.hitboxes[1].update(delta_time, m_game_state.player->get_hitbox());

    for (int i = 0; i < WIDGET_COUNT; i++)
		m_game_state.widgets[i].update(delta_time);

    // center meter
    m_game_state.widgets[8].m_offset = glm::vec3((-(m_game_state.player->get_position().x)/2)+((m_game_state.player2->get_position().x)/2), 1.6f, 0.0f);
    m_game_state.widgets[9].m_offset = glm::vec3((-(m_game_state.player->get_position().x)/2)+((m_game_state.player2->get_position().x)/2), 
        3.0f - (3.0 * ((m_game_state.meter->m_frame+1.0f) / (m_game_state.meter->m_FRAMES*1.0f))) + 1.6f,
        0.0f);

    dialogue->update(delta_time);
    switch (m_game_state.player2->get_state()) {
    case CRASH_DEF:
        dialogue->m_animation_index = 2;
        break;
    case MIRROR_DEF:
        dialogue->m_animation_index = 1;
        break;
    case COOLER_DEF:
        dialogue->m_animation_index = 4;
        break;
    case CRASH_OFF:
        dialogue->m_animation_index = 0;
        break;
    case MIRROR_OFF:
        dialogue->m_animation_index = 3;
        break;
    default:
        break;
    }

    if (!m_game_state.player2->is_alive()) post_level_switch = true;
}


void LevelD::render(ShaderProgram *g_shader_program)
{
    m_game_state.map->render(g_shader_program);
    for (int i = 0; i < 2 + 1; i++)
    {
        m_game_state.hitboxes[i].render(g_shader_program);
    }
    m_game_state.player->render(g_shader_program);
    m_game_state.player2->render(g_shader_program);
    for (int i = 0; i < WIDGET_COUNT; i++)
		m_game_state.widgets[i].render(g_shader_program);

    if (post_level_switch)
		Utility::draw_text(g_shader_program, m_font_texture_id, "Space to continue", 0.4f, 0.01f,
			m_game_state.player->get_position() + glm::vec3(-3.0f, 2.0f, 0.0f)); // position according to player
    /*
	Utility::draw_text(g_shader_program, m_font_texture_id, std::to_string(m_game_state.meter->m_frame), 0.5f, 0.05f,
	//Utility::draw_text(g_shader_program, m_font_texture_id, std::to_string(m_game_state.meter->m_frame/(m_game_state.meter->m_FRAMES * 1.0f)), 0.5f, 0.05f,
		glm::vec3(((m_game_state.player->get_position().x + m_game_state.player2->get_position().x)/2), -2.4f, 0.0f));
        */
    // DEBUG prototype to ui elements
    /*
	Utility::draw_text(g_shader_program, m_font_texture_id, m_game_state.player->get_stance_str(), 0.5f, 0.05f,
		m_game_state.player->get_position() + glm::vec3(-0.5f, 1.0f, 0.0f)); // position according to player
	Utility::draw_text(g_shader_program, m_font_texture_id, std::to_string(m_game_state.player->get_weight()), 0.5f, 0.05f,
		m_game_state.player->get_position() + glm::vec3(0.4f, 1.0f, 0.0f)); // position according to player
	Utility::draw_text(g_shader_program, m_font_texture_id, m_game_state.player2->get_stance_str(), 0.5f, 0.05f,
		m_game_state.player2->get_position() + glm::vec3(-0.5f, 1.0f, 0.0f)); // position according to player
	Utility::draw_text(g_shader_program, m_font_texture_id, std::to_string(m_game_state.player2->get_weight()), 0.5f, 0.05f,
		m_game_state.player2->get_position() + glm::vec3(0.4f, 1.0f, 0.0f)); // position according to player

    if (!m_game_state.player2->is_alive())
		Utility::draw_text(g_shader_program, m_font_texture_id, "You Win!", 0.5f, 0.05f,
			m_game_state.player->get_position() + glm::vec3(-2.0f, 2.0f, 0.0f)); // position according to player
    if (!m_game_state.player->is_alive())
		Utility::draw_text(g_shader_program, m_font_texture_id, "You Lose.", 0.5f, 0.05f,
			m_game_state.player->get_position() + glm::vec3(-2.0f, 2.0f, 0.0f)); // position according to player
            */


    //dialogue->render(g_shader_program, 0);
}
