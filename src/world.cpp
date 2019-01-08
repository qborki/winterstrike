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
#include <stdexcept>
#include <algorithm>
#include <SDL.h>
#include "game.h"
#include "sprite.h"
#include "world.h"
#include "object.h"
#include "camera.h"

static Sprite s_tiles("tiles.png", vec2i(128, 196), vec2i(64, 164), 0, 18);

World::World() {
}

World::~World() {
    for (auto it : m_objects) {
        delete it;
    }
    m_objects.clear();
}

/**
 * Consistent noise generator for coordinates
 */
static int hash (int a1, int a2, int a3) {
    int seed = (( a1 + a2) * (a1 + a2 + 1)) / 2 + a2;
    seed = (( seed + a3) * (seed + a3 + 1)) / 2 + a3;

    seed = ((seed >> 16) ^ seed) * 0x45d9f3b;
    seed = ((seed >> 16) ^ seed) * 0x45d9f3b;
    seed = (seed >> 16) ^ seed;
    return seed;
}

/**
 * Get vertex height. Each vertex is shared by eight tiles.
 */
static int heightmap(int x, int y) {
    x = abs(x);
    y = abs(y);

    // flat central area
    if (x < 4 && y < 4) {
        return 0;
    }

    // smooth
    return hash(x + 0, y + 0, 20) % 3
        && hash(x + 1, y + 0, 20) % 3
        && hash(x + 0, y + 1, 20) % 3
        && hash(x + 1, y + 1, 20) % 3;
}

/**
 * Procedural map generation
 */
void World::Tile::generate(int x, int y) {
    int a;
    // get correct wall tile
    a = heightmap(x + 1, y + 0)
      | heightmap(x + 0, y + 0) << 1
      | heightmap(x + 1, y + 1) << 2
      | heightmap(x + 0, y + 1) << 3;

    // if not a wall place some obstacles
    if (a == 0) {
        a = hash(abs(x), abs(y), 21) % 10;

        if (a < 1) {
            a = 16 + a;
        }
        else {
            a = -1;
        }
    }

    m_layers[0] = 0;
    m_layers[1] = a;
    m_passable = (a == -1);
}

World::Tile& World::getTile(const vec2i& pos) {
    vec2i local_pos = (pos % Chunk::SIZE + vec2i(Chunk::SIZE, Chunk::SIZE)) % Chunk::SIZE;
    vec2i chunk_pos = pos - local_pos;

    Chunk& chunk = m_chunks[chunk_pos];

    if (chunk.m_atime == 0) {
        SDL_LogDebug(SDL_LOG_CATEGORY_TEST, "Create tiles: [%d,%d]:[%d,%d]", chunk_pos.x, chunk_pos.y, chunk_pos.x + Chunk::SIZE, chunk_pos.y + Chunk::SIZE);

        for (int x = 0; x < Chunk::SIZE; ++x) {
            for (int y = 0; y < Chunk::SIZE; ++y) {
                chunk.m_tiles[x][y].generate(chunk_pos.x + x, chunk_pos.y + y);
            }
        }
    }
    chunk.m_atime = SDL_GetTicks();

    return chunk.m_tiles[local_pos.x][local_pos.y];
}

/**
 * Check if objects can move here
 */
bool World::isPassable(const vec2i& pos) {
    return getTile(pos).m_passable;
}

/**
 * Debug render tile
 */
void World::renderMarker(SDL_Renderer* renderer, Camera* camera, const vec2f& pos, int type) {
    vec2i v = camera->worldToScreen(pos);
    SDL_Point points[] = {{v.x, v.y + 32}, {v.x - 64, v.y}, {v.x + 64, v.y}, {v.x, v.y + 32}, {v.x, v.y - 32}, {v.x - 64, v.y}, {v.x + 64, v.y}, {v.x, v.y - 32}};
    if (type == 0) {
        SDL_SetRenderDrawColor(renderer, 255, 0, 255, SDL_ALPHA_OPAQUE);
    }
    else {
        SDL_SetRenderDrawColor(renderer, 128, 255, 128, SDL_ALPHA_OPAQUE);
    }
    SDL_RenderDrawLines(renderer, points, 8);
}

/**
 * Main rendering function. Draw world and its objects.
 */
void World::render(SDL_Renderer* renderer, Camera* camera) {
    vec2f lt = camera->screenToWorld(vec2i()),
          rb = camera->screenToWorld(camera->getSize() + s_tiles.getOffset());

    lt.x = floor(lt.x);
    lt.y = floor(lt.y);
    rb.x = ceil(rb.x);
    rb.y = ceil(rb.y);

    int cx = ceil((rb.x - rb.y - lt.x + lt.y + 1) / 2 + 1);
    int cy = ceil(rb.x + rb.y - lt.x - lt.y + 1);

    for (int z = 0; z < 2; ++z) {
        vec2f pos = lt;

        for (int a = 0; a < cy; ++a) {
            for (int b = 0; b < cx; ++b) {
                Tile& tile = getTile(vec2i(pos));

                if (tile.m_layers[z] >= 0) {
                    s_tiles.render(renderer, camera->worldToScreen(pos), 0, tile.m_layers[z]);
                }

                pos += vec2f(1, -1);
            }

            for (auto it : m_objects) {
                vec2f objpos = it->getPosition();

                if (it->getZ() == z && (int)round(objpos.x + objpos.y) == (int)(pos.x + pos.y)) {
                    it->render(renderer, camera->worldToScreen(objpos));
                }
            }

            pos += vec2f(-cx, cx);
            pos += a % 2 ? vec2f(1, 0) : vec2f(0, 1);
        }
    }

    for (auto it : m_objects) {
        if (it->getZ() > 1) {
            it->render(renderer, camera->worldToScreen(it->getPosition()));
        }
    }

    // renderMarker(renderer, camera, lt, 1);
    // renderMarker(renderer, camera, rb + vec2f(0, 0), 2);
}

/**
 * Create object and link it to this world
 */
Object* World::spawn(const std::string& className, const vec2f& pos) {
    Object* o = Game::get().getFactory(className)();

    m_objects.push_back(o);

    o->setWorld(this);
    o->setPosition(pos);

    return o;
}

/**
 * Move and update objects
 */
void World::update(float dt) {
    // movement and collision detection
    for (auto object : m_objects) {
        // backup position
        vec2f backup_pos = object->getPosition();

        // update and move object
        object->update(dt);

        // check for collisions
        if (object->isSolid() || object->isCollider()) {
            vec2f fpos = object->getPosition();
            vec2i ipos(round(fpos.x), round(fpos.y));
            if (!isPassable(ipos)) {
                // restore position
                if (object->isSolid()) {
                    // object->setPosition(backup_pos);
                }
                // notify collision with world
                if (object->isCollider()) {
                    object->onCollision(nullptr);
                }
            }

            for (auto other : m_objects) {
                if (object == other) {
                    continue;
                }

                bool solid = object->isSolid() && other->isSolid();
                bool collider = object->isCollider() && other->isCollider();

                if (!solid && !collider) {
                    continue;
                }

                vec2f delta = object->getPosition() - other->getPosition();

                if (delta.length() < 0.2) {
                    // restore position
                    if (solid) {
                        object->setPosition(backup_pos);
                    }
                    // notify collision
                    if (collider) {
                        object->onCollision(other);
                        other->onCollision(object);
                    }
                }
            }
        }
    }

    // remove dead
    auto it = m_objects.begin();
    while (it != m_objects.end()) {
        if ((*it)->isAlive()) {
            ++it;
        }
        else {
            delete *it;
            it = m_objects.erase(it);
        }
    }

    // do z-sorting (FIXME: move to render?)
    std::sort(m_objects.begin(), m_objects.end(), [] (const Object* a, const Object* b) {
        vec2f posa = a->getPosition(), posb = b->getPosition();
        int  za = a->getZ(), zb = b->getZ();

        return za < zb || za == zb && posa.x + posa.y < posb.x + posb.y;
    });
}
