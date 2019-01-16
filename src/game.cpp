/* Winter-Strike Game
 * Copyright (C) 2019 Boris Kumok
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#include <iostream>
#include <stdexcept>
#include <unistd.h>
#include <sys/stat.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include "vec.h"
#include "game.h"
#include "menu.h"
#include "world.h"
#include "object.h"
#include "camera.h"
#include "character.h"

/**
 * Make application a singleton, so that we can auto-register object factories
 */
Game& Game::get() {
    static Game game;
    return game;
}

Game::Game() :
    m_base_path("./"),
    m_event_type(-1),
    m_window(nullptr),
    m_renderer(nullptr),
    m_menu(nullptr),
    m_world(nullptr),
    m_player(nullptr),
    m_camera(nullptr),
    m_cursor(nullptr),
    m_fullScreen(true),
    m_musicEnabled(true),
    m_running(false) {
}

Game::~Game() {
    destroy();
}

/**
 * Free resources
 */
void Game::destroy() {
    delete m_menu;
    m_menu = nullptr;

    delete m_world;
    m_world = nullptr;
    m_player = nullptr;
    m_camera = nullptr;

    for (auto it : m_sounds) {
        if (it.second) Mix_FreeChunk(it.second);
    }
    m_sounds.clear();

    for (auto it : m_music) {
        if (it.second) Mix_FreeMusic(it.second);
    }
    m_music.clear();

    for (auto it : m_fonts) {
        if (it.second) TTF_CloseFont(it.second);
    }
    m_fonts.clear();

    for (auto it : m_textures) {
        if (it.second) SDL_DestroyTexture(it.second);
    }
    m_textures.clear();

    if (m_renderer) {
        SDL_DestroyRenderer(m_renderer);
        m_renderer = nullptr;
    }
    if (m_window) {
        SDL_DestroyWindow(m_window);
        m_window = nullptr;
    }
    Mix_Quit();
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

/**
 * Parse command line arguments, init SDL, create some objects
 */
Game& Game::init(int argc, char* argv[]) {
    // parse command line
    int opt;
    while ((opt = getopt(argc, argv, "mvw")) != -1) {
        switch (opt) {
            case 'm':
                m_musicEnabled = false;
                break;
            case 'v':
                std::cout << PROJECT_NAME << " (compiled " << BUILD_DATE << " " << BUILD_TIME << ")" << std::endl;
                std::cout << "Revision: " << PROJECT_VERSION << std::endl;
                exit(0);
            case 'w':
                m_fullScreen = false;
                break;
        }
    }

    char *base_path = SDL_GetBasePath();
    if (base_path) {
        m_base_path = base_path;
        SDL_free(base_path);
    }

    // init SDL subsystems
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        throw std::runtime_error(SDL_GetError());
    }

    m_event_type = SDL_RegisterEvents(1);
    if (m_event_type == ((Uint32)-1)) {
        throw std::runtime_error("SDL_RegisterEvents failed");
    }

    if (IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG == 0) {
        throw std::runtime_error(IMG_GetError());
    }

    if (TTF_Init() < 0) {
        throw std::runtime_error(TTF_GetError());
    }

    if (Mix_Init(MIX_INIT_OGG) & MIX_INIT_OGG == 0) {
        throw std::runtime_error(Mix_GetError());
    }

    if (Mix_OpenAudio( 22050, MIX_DEFAULT_FORMAT, 2, 4096 ) == -1) {
        throw std::runtime_error(Mix_GetError());
    }

    // create window and renderer
    int flags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;
    if (m_fullScreen) {
        flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
    }

    if ((m_window = SDL_CreateWindow(PROJECT_NAME.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, flags)) == nullptr) {
        throw std::runtime_error(SDL_GetError());
    }

    if ((m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC)) == nullptr) {
        throw std::runtime_error(SDL_GetError());
    }

    SDL_RenderSetLogicalSize(m_renderer, 800, 600);

    // start background music
    if (m_musicEnabled) {
        if (Mix_PlayMusic(getMusic("music.ogg"), -1) < 0) {
            throw std::runtime_error(Mix_GetError());
        }
    }

    m_menu = new Menu();

    m_running = true;
    return *this;
}

/**
 * Add custom user event to SDL event queue
 */
void Game::pushEvent(int code, void* data1, void* data2) {
    SDL_Event ev;
    SDL_zero(ev);
    ev.type = m_event_type;
    ev.user.code = code;
    ev.user.data1 = data1;
    ev.user.data2 = data2;
    SDL_PushEvent(&ev);
}

/**
 * Show or hide a menu
 */
void Game::toggleMenu(bool show) {
    delete m_menu;
    m_menu = nullptr;

    if (show) {
        m_menu = new Menu();
    }
}

/**
 * Create a world and populate it
 */
void Game::createWorld() {
    delete m_world;
    m_world = nullptr;
    m_cursor = nullptr;
    m_camera = nullptr;
    m_player = nullptr;

    m_world  = new World();
    m_cursor = m_world->spawn("Cursor", vec2f(0,0));
    m_camera = (Camera*) m_world->spawn("Camera", vec2f(0, 0));
    m_camera->setSize(vec2i(800, 600));
    m_player = (Character*) m_world->spawn("Character", vec2f(0, 7));
    m_world->spawn("CharacterAi", vec2f(-3,-5));
    m_world->spawn("CharacterAi", vec2f(-1,-6));
    m_world->spawn("CharacterAi", vec2f(0,-7));
    m_world->spawn("CharacterAi", vec2f(1,-6));
    m_world->spawn("CharacterAi", vec2f(3,-5));
}

/**
 * Main loop
 */
void Game::run() {
    Uint32 currentTime = SDL_GetTicks();
    SDL_Event ev;

    while (m_running)  {
        // input
        while (SDL_PollEvent(&ev) != 0) {
            if (ev.type == SDL_QUIT) {
                m_running = false;
            }
            else if (ev.type == SDL_WINDOWEVENT) {
                switch (ev.window.event) {
                    case SDL_WINDOWEVENT_RESIZED:
                        float k = fmin(1.0, fmin(800.0 / ev.window.data1, 600.0 / ev.window.data2));
                        int w = int(ev.window.data1 * k);
                        int h = int(ev.window.data2 * k);
                        SDL_RenderSetLogicalSize(m_renderer, w, h);
                        if (m_camera) {
                            m_camera->setSize(vec2i(w, h));
                        }
                    break;
                }
            }
            else if (ev.type == SDL_KEYDOWN) {
                switch (ev.key.keysym.sym) {
                    case SDLK_f:
                        m_fullScreen = !m_fullScreen;
                        SDL_SetWindowFullscreen(m_window, m_fullScreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
                    break;
                }
            }
            else if (ev.type == m_event_type) {
                switch (ev.user.code) {
                    case EV_MENU_SELECT:
                        int item = (int)(uintptr_t)ev.user.data2;
                        if (item == -1) {
                            toggleMenu(false);
                        }
                        else if (item == 0) {
                            toggleMenu(false);
                            createWorld();
                        }
                        else if (item == 2) {
                            m_running = false;
                        }
                    break;
                }
            }

            if (m_menu) {
                m_menu->onEvent(ev);
            }
            else if (m_world) {
                if (ev.type == SDL_MOUSEMOTION) {
                    vec2f cursor = m_camera->screenToWorld(vec2i(ev.motion.x, ev.motion.y));
                    m_cursor->setPosition(vec2f(round(cursor.x), round(cursor.y)));
                }
                else if (ev.type == SDL_MOUSEBUTTONUP) {
                    if (ev.button.button == SDL_BUTTON_LEFT) {
                        m_player->walkTo(m_camera->screenToWorld(vec2i(ev.button.x, ev.button.y)));
                    }
                    else if (ev.button.button == SDL_BUTTON_RIGHT) {
                        m_player->throwAt(m_camera->screenToWorld(vec2i(ev.button.x, ev.button.y)));
                    }
                }
                else if (ev.type == SDL_KEYDOWN) {
                    if (ev.key.keysym.sym == SDLK_ESCAPE) {
                        toggleMenu(true);
                    }
                }
            }
        }

        Uint32 time = SDL_GetTicks();
        float dt = (time - currentTime) / 1000.0;
        currentTime = time;

        // update
        if (m_menu) {
            m_menu->update(dt);
        }
        else if (m_world) {
            m_world->update(dt);
            m_camera->setPosition(m_player->getPosition());
        }
        else {
            m_running = false;
        }

        // render
        SDL_SetRenderDrawColor(m_renderer, 0x00, 0x00, 0x00, 0xFF);
        SDL_RenderClear(m_renderer);

        if (m_world) {
            m_world->render(m_renderer, m_camera);
        }
        if (m_menu) {
            m_menu->render(m_renderer);
        }

        SDL_RenderPresent(m_renderer);
        SDL_Delay(20);
    }
}

/**
 * Get registered constructor
 */
Game::ObjectCtor Game::getFactory(const std::string& className) const {
    return m_factories.at(className);
}

/**
 * Register object factory
 */
bool Game::setFactory(const std::string& className, ObjectCtor constructor) {
    SDL_LogDebug(SDL_LOG_CATEGORY_TEST, "Register: %s", className.c_str());
    m_factories[className] = constructor;
    return true;;
}

/**
 * Load and cache textures
 */
SDL_Texture* Game::getTexture(const std::string& fileName) {
    SDL_Texture* texture = m_textures[fileName];

    if (texture == nullptr) {
        SDL_LogDebug(SDL_LOG_CATEGORY_TEST, "Load: %s", fileName.c_str());
        if ((texture = IMG_LoadTexture(m_renderer, getDataFile("gfx/" + fileName).c_str())) == nullptr) {
            throw std::runtime_error(IMG_GetError());
        }
        m_textures[fileName] = texture;
    }
    return texture;
}

/**
 * Load and cache fonts
 */
TTF_Font* Game::getFont(const std::string& fileName, int ptsize) {
    std::string key = fileName + ":" + std::to_string(ptsize);
    TTF_Font* font = m_fonts[key];

    if (font == nullptr) {
        SDL_LogDebug(SDL_LOG_CATEGORY_TEST, "Load: %s", key.c_str());
        if ((font = TTF_OpenFont(getDataFile("gfx/" + fileName).c_str(), ptsize)) == nullptr) {
            throw std::runtime_error(TTF_GetError());
        }
        m_fonts[key] = font;
    }
    return font;
}

/**
 * Load and cache audiofiles
 */
Mix_Chunk* Game::getSound(const std::string& fileName) {
    Mix_Chunk* chunk = m_sounds[fileName];

    if (chunk == nullptr) {
        SDL_LogDebug(SDL_LOG_CATEGORY_TEST, "Load: %s", fileName.c_str());
        if ((chunk = Mix_LoadWAV(getDataFile("sfx/" + fileName).c_str())) == nullptr) {
            throw std::runtime_error(Mix_GetError());
        }
        m_sounds[fileName] = chunk;
    }
    return chunk;
}

/**
 * Load and cache music
 */
Mix_Music* Game::getMusic(const std::string& fileName) {
    Mix_Music* music = m_music[fileName];

    if (music == nullptr) {
        SDL_LogDebug(SDL_LOG_CATEGORY_TEST, "Load: %s", fileName.c_str());
        if ((music = Mix_LoadMUS(getDataFile("sfx/" + fileName).c_str())) == nullptr) {
            throw std::runtime_error(Mix_GetError());
        }
        m_music[fileName] = music;
    }
    return music;
}

/**
 * Resolve resource filename
 */
const std::string Game::getDataFile(const std::string& fileName) const {
    // prefer local data (e.g. in build tree)
    std::string path(m_base_path + fileName);

    // then check system installation directory
    struct stat info;
    if (stat(path.c_str(), &info) != 0) {
        path = INSTALL_PREFIX + "/share/" + PROJECT_NAME + "/" + fileName;
    }
    return path;
}
