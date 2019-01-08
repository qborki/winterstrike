/* Winter-Strike Game
 * Copyright (C) 2019 Kumok Boris
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
#ifndef _WORLD_H
#define _WORLD_H

#include <vector>
#include <map>
#include "vec.h"

struct SDL_Renderer;
class Camera;
class Object;

class World {
public:
    World();
    ~World();

    void render(SDL_Renderer*, Camera* camera);
    void update(float dt);

    Object* spawn(const std::string& classname, const vec2f& pos);

    bool isPassable(const vec2i& pos);

    void renderMarker(SDL_Renderer*, Camera*, const vec2f& pos, int type);
private:
    std::vector<Object*> m_objects;

    // Procedural tile generation and caching
    struct Tile {
        int m_layers[2];
        int m_passable;
        void generate(int x, int y);
    };
    struct Chunk {
        enum { SIZE = 32 };
        Tile  m_tiles[SIZE][SIZE];
        int   m_atime; // access timestamp for garbage collector

        inline Chunk(): m_atime(0) {}
    };
    std::map<vec2i, Chunk> m_chunks;

    Tile& getTile(const vec2i&);
};

#endif