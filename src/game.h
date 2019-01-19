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
#ifndef GAME_H
#define GAME_H

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include "state.h"

struct SDL_Window;
struct SDL_Renderer;
struct SDL_Texture;
struct Mix_Chunk;
typedef struct _Mix_Music Mix_Music;
typedef struct _TTF_Font TTF_Font;

class Game {
public:
    enum {STATE_MENU, STATE_WORLD};

    Game();
    ~Game();

    void init(int argc, char* argv[]);
    void destroy();
    void run();

    void pushState(int stateid);
    void popState();

    // Resource manager
    SDL_Texture* getTexture(const std::string& fileName);
    TTF_Font*    getFont(const std::string& fileName, int ptsize);
    Mix_Chunk*   getSound(const std::string& fileName);
    Mix_Music*   getMusic(const std::string& fileName);
    inline SDL_Renderer* getRenderer() {
        return m_renderer;
    }

private:
    const std::string getDataFile(const std::string&) const;

    std::string m_base_path;

    SDL_Window*   m_window;
    SDL_Renderer* m_renderer;

    // assets cache
    std::unordered_map<std::string, SDL_Texture*> m_textures;
    std::unordered_map<std::string, TTF_Font*>    m_fonts;
    std::unordered_map<std::string, Mix_Chunk*>   m_sounds;
    std::unordered_map<std::string, Mix_Music*>   m_music;

    // active states stack (all are rendered, but only top is updated and gets input)
    std::vector<std::unique_ptr<State>> m_states;
    std::vector<std::unique_ptr<State>> m_purgatory;

    bool m_fullScreen;
    bool m_musicEnabled;

    // version and executable link time (set by build scripts)
    static const std::string PROJECT_NAME;
    static const std::string PROJECT_VERSION;
    static const std::string PROJECT_COMMIT;
    static const std::string BUILD_DATE;
    static const std::string BUILD_TIME;
    static const std::string BUILD_TYPE;
    static const std::string BUILD_PLATFORM;
    static const std::string INSTALL_PREFIX;
};
#endif
