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
#ifndef SPRITE_H
#define SPRITE_H

#include <string>
#include "vec.h"

class  Game;
struct SDL_Texture;
struct SDL_Renderer;

class Sprite {
public:
    Sprite();
    ~Sprite();

    void destroy();

    void text(Game&, const std::string& text,     const std::string& fontname, int ptsize, int rgba);
    void load(Game&, const std::string& filename, const vec2i& size, const vec2i& offset = vec2i(), int start = 0, int count = 1);
    void grad(Game&, const vec2i& size, int rgba0, int rgba1);

    inline const bool exists() const {
        return m_texture != nullptr;
    }

    inline const vec2i& getSize() const {
        return m_size;
    }

    inline const vec2i& getOffset() const {
        return m_offset;
    }

    inline const int getFrames() const {
        return m_count;
    }

    inline const int getSides() const {
        return m_rows;
    }

    void render(SDL_Renderer*, const vec2i& pos, int side = 0, int frame = 0, const vec2f& scale = vec2f(1.0, 1.0));
private:
    SDL_Texture* m_texture;
    bool m_must_destroy;
    int m_rows;
    int m_cols;
    int m_start;
    int m_count;
    vec2i m_size;
    vec2i m_offset;
};

#endif
