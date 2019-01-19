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
#ifndef WORLD_H
#define WORLD_H

#include <memory>
#include <vector>
#include <unordered_map>
#include "state.h"
#include "object.h"
#include "vec.h"
#include "sprite.h"

class Character;
class Object;

class World: public State {
public:
    World(Game&, int seed);

    void render(SDL_Renderer*);
    void update(float dt);
    void onEvent(SDL_Event& ev);

    void add(std::unique_ptr<Object> object);

    bool isPassable(const vec2i& pos);

    std::vector<Object*> getObjectsInRadius(const vec2f& pos, float radius);
    std::vector<vec2f> buildPath(const vec2f& from, const vec2f& goal);
    bool checkVisible(const vec2f& origin, const vec2f& target);

    inline Game& getGame() {
        return m_game;
    }
private:
    struct Tile {
        enum  { LAYERS = 3 };
        int m_layers[LAYERS];
        int m_passable;
    };
    struct Chunk {
        enum { SIZE = 64 };
        Tile  m_tiles[SIZE][SIZE];
        int   m_atime;

        inline Chunk(): m_atime(0) {}
    };
    const vec2i worldToScreen(const vec2f& pos) const;
    const vec2f screenToWorld(const vec2i& pos) const;
    void renderMarker(SDL_Renderer*, const vec2f& pos, unsigned rgba);

    // procedural map generation
    Tile& getTile(const vec2i&);
    void  generate(Tile&, const vec2i&);
    int   getVertexZ(const vec2i&);
    int   getWallSpriteId(const vec2i&);

    int        m_seed;     
    vec2i      m_cursor;
    vec2i      m_viewport;
    vec2f      m_camera;
    Character* m_player;

    std::vector<Sprite> m_sprites;
    std::vector<std::unique_ptr<Object>> m_objects;
    std::unordered_map<vec2i, Chunk> m_chunks;

};

#endif
