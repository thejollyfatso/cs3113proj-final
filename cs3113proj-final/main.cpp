/**
* Author: Rafael de Leon
* Assignment: Rise of the AI
* Date due: 2024-07-27, 11:59pm
* Extension: 2024-07-30, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#define GL_SILENCE_DEPRECATION
#define GL_GLEXT_PROTOTYPES 1
#define FIXED_TIMESTEP 0.0166666f
#define LEVEL1_WIDTH 14
#define LEVEL1_HEIGHT 8
#define LEVEL1_LEFT_EDGE 5.0f

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL_mixer.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "cmath"
#include <ctime>
#include <vector>
#include "Entity.h"
#include "Map.h"
#include "Utility.h"
#include "Scene.h"
#include "Level0.h"
#include "LevelA.h"
#include "LevelB.h"
#include "LevelC.h"
#include "LevelC1.h"
#include "LevelD.h"
#include "LevelD1.h"

// ————— CONSTANTS ————— //
constexpr int WINDOW_WIDTH  = 640,
          WINDOW_HEIGHT = 480;

constexpr float BG_RED     = 0.1922f,
            BG_BLUE    = 0.549f,
            BG_GREEN   = 0.9059f,
            BG_OPACITY = 1.0f;

constexpr int VIEWPORT_X = 0,
          VIEWPORT_Y = 0,
          VIEWPORT_WIDTH  = WINDOW_WIDTH,
          VIEWPORT_HEIGHT = WINDOW_HEIGHT;

constexpr char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
           F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

constexpr float MILLISECONDS_IN_SECOND = 1000.0;

enum AppStatus { RUNNING, TERMINATED };

// ————— GLOBAL VARIABLES ————— //
Scene *g_current_scene;
Level0 *g_level_0;
LevelA *g_level_a;
LevelB *g_level_b;
LevelC *g_level_c;
LevelC1 *g_level_c1;
LevelD* g_level_d;
LevelD1* g_level_d1;

Scene *g_levels[7];

SDL_Window* g_display_window;

AppStatus g_app_status = RUNNING;
ShaderProgram g_shader_program;
glm::mat4 g_view_matrix, g_projection_matrix;

float g_previous_ticks = 0.0f;
float g_accumulator = 0.0f;

void switch_to_scene(Scene *scene)
{
    g_current_scene = scene;
    g_current_scene->initialise();
}

void initialise();
void process_input();
void update();
void render();
void shutdown();


void initialise()
{
    // ————— VIDEO ————— //
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    g_display_window = SDL_CreateWindow("WimWim",
                                      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                      WINDOW_WIDTH, WINDOW_HEIGHT,
                                      SDL_WINDOW_OPENGL);
    
    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);
    if (context == nullptr)
    {
        shutdown();
    }
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    // ————— GENERAL ————— //
    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
    
    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);
    
    g_view_matrix = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);

    glUseProgram(g_shader_program.get_program_id());
    
    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);
    
    
    // ————— LEVEL SETUP ————— //
    g_level_0 = new Level0();
    g_level_a = new LevelA();
    g_level_b = new LevelB();
    g_level_c = new LevelC();
    g_level_c1 = new LevelC1();
    g_level_d = new LevelD();
    g_level_d1 = new LevelD1();
    g_levels[0] = g_level_0;
    g_levels[1] = g_level_a;
    g_levels[2] = g_level_b;
    g_levels[3] = g_level_c;
    g_levels[4] = g_level_c1;
    g_levels[5] = g_level_d;
    g_levels[6] = g_level_d1;
    switch_to_scene(g_level_0);
    
    // ————— BLENDING ————— //
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void process_input()
{
    g_current_scene->get_state().player->set_movement(glm::vec3(0.0f));
    
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        // ————— KEYSTROKES ————— //
        switch (event.type) {
            // ————— END GAME ————— //
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                g_app_status = TERMINATED;
                break;
                
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                case SDLK_x:
                    // If in the difficulty selection state, revert back to the main menu state
                    if (g_current_scene == g_levels[0]) {
                        if (g_level_0->m_current_menu_state == DIFFICULTY_SELECTION) {
                            g_level_0->m_current_menu_state = MAIN_MENU;  // Revert to the main menu state
                        }
                        else {
                            g_app_status = TERMINATED;  // Exit the game if in the main menu state
                        }
                    }
                    else {
                        switch_to_scene(g_levels[0]);
                    }
                    break;
                    case SDLK_m:
                        // toggle music
                        if (Mix_VolumeMusic(-1)) Mix_VolumeMusic(0);
                        else Mix_VolumeMusic(MIX_MAX_VOLUME / 2);
                        break;

                    case SDLK_1:
                        if (g_current_scene == g_levels[0]) {
                            if (g_level_0->m_current_menu_state == MAIN_MENU) {
                                switch_to_scene(g_levels[1]);  // Multiplayer
                            }
                            else if (g_level_0->m_current_menu_state == DIFFICULTY_SELECTION) {
                                g_level_b->selected_difficulty = 1;
                                switch_to_scene(g_levels[2]);  // Duel with Easy difficulty
                            }
                        }
                        break;

                    case SDLK_2:
                        if (g_current_scene == g_levels[0]) {
                            if (g_level_0->m_current_menu_state == MAIN_MENU) {
                                g_level_0->m_current_menu_state = DIFFICULTY_SELECTION;  // Switch to difficulty selection
                            }
                            else if (g_level_0->m_current_menu_state == DIFFICULTY_SELECTION) {
                                g_level_b->selected_difficulty = 2;
                                switch_to_scene(g_levels[2]);  // Duel with Medium difficulty
                            }
                        }
                        break;

                    case SDLK_3:
                        if (g_current_scene == g_levels[0]) {
                            if (g_level_0->m_current_menu_state == MAIN_MENU) {
                                switch_to_scene(g_levels[3]);
                            }
                            else if (g_level_0->m_current_menu_state == DIFFICULTY_SELECTION) {
                                g_level_b->selected_difficulty = 3;
                                switch_to_scene(g_levels[2]);  // Duel with Hard difficulty
                            }
                        }
                        break;

                    case SDLK_4:
                        if (g_current_scene == g_levels[0]) {
                            if (g_level_0->m_current_menu_state == MAIN_MENU) {
                                //switch_to_scene(g_levels[5]);
                                switch_to_scene(g_levels[6]);
                            }
                        }
                        break;
                        
                    case SDLK_d:
                        g_current_scene->get_state().player->move_right();
                        break;
                    case SDLK_a:
                        g_current_scene->get_state().player->move_left();
                        break;

                    case SDLK_f:
                        // attack
						g_current_scene->get_state().player->attack();
                        break;

                    case SDLK_w:
						g_current_scene->get_state().player->inc_weight();
                        break;
                        
                    case SDLK_s:
						g_current_scene->get_state().player->dec_weight();
                        break;
                        
                    case SDLK_q:
						g_current_scene->get_state().player->dec_stance();
                        break;
                        
                    case SDLK_e:
						g_current_scene->get_state().player->inc_stance();
                        break;
                        
                    case SDLK_l:
                        if (g_current_scene == g_levels[1]) g_current_scene->get_state().player2->move_right();
                        break;
                    case SDLK_j:
                        if (g_current_scene == g_levels[1]) g_current_scene->get_state().player2->move_left();
                        break;
                    case SDLK_h:
                        // attack
						if (g_current_scene == g_levels[1]) g_current_scene->get_state().player2->attack();
                        break;

                    case SDLK_i:
						if (g_current_scene == g_levels[1]) g_current_scene->get_state().player2->inc_weight();
                        break;
                        
                    case SDLK_k:
						if (g_current_scene == g_levels[1]) g_current_scene->get_state().player2->dec_weight();
                        break;
                        
                    case SDLK_u:
						if (g_current_scene == g_levels[1]) g_current_scene->get_state().player2->dec_stance();
                        break;
                        
                    case SDLK_o:
						if (g_current_scene == g_levels[1]) g_current_scene->get_state().player2->inc_stance();
                        break;

                    
                    case SDLK_BACKQUOTE: // Toggling debug mode with backtick key
                        static_cast<LevelB*>(g_levels[2])->m_debug_mode = !static_cast<LevelB*>(g_levels[2])->m_debug_mode;
                        break;
					default:
                        break;
                }
                
            default:
                break;
        }
    }
    
    // ————— KEY HOLD ————— //
    const Uint8 *key_state = SDL_GetKeyboardState(NULL);

    /*  Removing key hold for discrete movements
    if (key_state[SDL_SCANCODE_LEFT])        g_current_scene->get_state().player->move_left();
    else if (key_state[SDL_SCANCODE_RIGHT])  g_current_scene->get_state().player->move_right();
    */

    if (glm::length( g_current_scene->get_state().player->get_movement()) > 1.0f)
        g_current_scene->get_state().player->normalise_movement();
 
}

void update()
{
    // ————— DELTA TIME / FIXED TIME STEP CALCULATION ————— //
    float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    float delta_time = ticks - g_previous_ticks;
    g_previous_ticks = ticks;
    
    delta_time += g_accumulator;
    
    if (delta_time < FIXED_TIMESTEP)
    {
        g_accumulator = delta_time;
        return;
    }
    
    while (delta_time >= FIXED_TIMESTEP) {
        // ————— UPDATING THE SCENE (i.e. map, character, enemies...) ————— //
        g_current_scene->update(FIXED_TIMESTEP);
        
        delta_time -= FIXED_TIMESTEP;
    }
    
    g_accumulator = delta_time;
    
    
    // ————— PLAYER CAMERA ————— //
    g_view_matrix = glm::mat4(1.0f);
    
    float midpoint_x = (g_current_scene->get_state().player->get_position().x + g_current_scene->get_state().player2->get_position().x) / 2.0f;

    g_view_matrix = glm::translate(g_view_matrix, glm::vec3(
        -(midpoint_x > LEVEL1_LEFT_EDGE ? midpoint_x : LEVEL1_LEFT_EDGE),
        3.75,
        0
    ));
    if (g_current_scene == g_level_c && g_level_c->post_level_switch) { switch_to_scene(g_levels[4]); }
}

void render()
{
    g_shader_program.set_view_matrix(g_view_matrix);
    
    glClear(GL_COLOR_BUFFER_BIT);
    
    // ————— RENDERING THE SCENE (i.e. map, character, enemies...) ————— //
    g_current_scene->render(&g_shader_program);
    
    SDL_GL_SwapWindow(g_display_window);
}

void shutdown()
{    
    SDL_Quit();
    
    // ————— DELETING LEVEL DATA (i.e. map, character, enemies...) ————— //
    //delete g_level_0;
    delete g_level_a;
    delete g_level_b;
    delete g_level_c;
    delete g_level_c1;
    delete g_level_d;
    delete g_level_d1;
}

// ————— GAME LOOP ————— //
int main(int argc, char* argv[])
{
    initialise();
    
    while (g_app_status == RUNNING)
    {
        process_input();
        update();
        render();
    }
    
    shutdown();
    return 0;
}
