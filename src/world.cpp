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
#include <list>
#include <queue>
#include <map>
#include <SDL.h>
#include "game.h"
#include "sprite.h"
#include "world.h"
#include "object.h"
#include "camera.h"

static Sprite s_sprites[] = {
    Sprite("tiles.png", vec2i(64, 128),  vec2i(32, 112), 0, 1),
    Sprite("tiles.png", vec2i(64, 128),  vec2i(32, 112), 1, 1),
    Sprite("tiles.png", vec2i(64, 128),  vec2i(32, 112), 2, 1),
    Sprite("tiles.png", vec2i(64, 128),  vec2i(32, 112), 3, 1),
    Sprite("tiles.png", vec2i(64, 128),  vec2i(32, 112), 4, 1),
    Sprite("tiles.png", vec2i(64, 128),  vec2i(32, 112), 5, 1),
    Sprite("tiles.png", vec2i(64, 128),  vec2i(32, 112), 6, 1),
    Sprite("tiles.png", vec2i(64, 128),  vec2i(32, 112), 7, 1),
    Sprite("tiles.png", vec2i(64, 128),  vec2i(32, 112), 8, 1),
    Sprite("tiles.png", vec2i(64, 128),  vec2i(32, 112), 9, 1),
    Sprite("tiles.png", vec2i(64, 128),  vec2i(32, 112), 10, 1),
    Sprite("tiles.png", vec2i(64, 128),  vec2i(32, 112), 11, 1),
    Sprite("tiles.png", vec2i(64, 128),  vec2i(32, 112), 12, 1),
    Sprite("tiles.png", vec2i(64, 128),  vec2i(32, 112), 13, 1),
    Sprite("tiles.png", vec2i(64, 128),  vec2i(32, 112), 14, 1),
    Sprite("tiles.png", vec2i(64, 128),  vec2i(32, 112), 15, 1),
    Sprite("trees.png", vec2i(128, 192), vec2i(64, 160), 0, 1)
};


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
    if (x < 8 && y < 8) {
        return 0;
    }

    // smooth 3x3
    static const vec2i steps[] = {{0, 0}, {0, -1}, {-1, 0}, {+1, 0}, {0, +1}, {-1, -1}, {+1, -1}, {-1, +1}, {+1, +1}};
    int result = 1;
    for (auto step : steps) {
        result = result && hash(x + step.x, y + step.y, 20) % 6;
    }
    return result;
}

/**
 * get correct wall tile
 */
static int wall(int x, int y) {
    return heightmap(x + 1, y + 0)
      | heightmap(x + 0, y + 0) << 1
      | heightmap(x + 1, y + 1) << 2
      | heightmap(x + 0, y + 1) << 3;
}

/**
 * Procedural map generation
 */
void World::Tile::generate(int x, int y) {
    int a = wall(x, y);;

    // if not a wall place some obstacles
    if (a == 0) {
        a = hash(abs(x), abs(y), 21) % 40;

        if (a < 1 && wall(x - 1, y + 1) == 0 && wall(x + 1, y - 1) == 0) {
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
 * Find objects near position. (All hail C++11 vector move semantics!)
 */
std::vector<Object*> World::getObjectsInRadius(const vec2f& pos, float radius) {
    std::vector<Object*> result;
    std::copy_if(m_objects.begin(), m_objects.end(), std::back_inserter(result), [pos, radius](Object* object){
        return (pos - object->getPosition()).length() < radius;
    });
    return result;
}

/**
 * Debug render tile
 */
void World::renderMarker(SDL_Renderer* renderer, Camera* camera, const vec2f& pos, unsigned rgba) {
    vec2i v = camera->worldToScreen(pos);
    SDL_Point points[] = {{v.x, v.y + 16}, {v.x - 32, v.y}, {v.x, v.y - 16}, {v.x + 32, v.y}, {v.x, v.y + 16}};

    SDL_SetRenderDrawColor(renderer, Uint8(rgba >> 24 & 0xff), Uint8((rgba >> 16) & 0xff), Uint8( (rgba >> 8) & 0xff), Uint8(rgba & 0xff));
    SDL_RenderDrawLines(renderer, points, sizeof(points) / sizeof(points[0]));
}

/**
 * Main rendering function. Draw world and its objects.
 */
void World::render(SDL_Renderer* renderer, Camera* camera) {
    vec2f lt = camera->screenToWorld(vec2i() - s_sprites[16].getOffset()),
          rb = camera->screenToWorld(camera->getSize() + s_sprites[16].getOffset());

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
                    s_sprites[tile.m_layers[z]].render(renderer, camera->worldToScreen(pos), 0, 0);
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
        if (z == 0) {
            // for (auto it : m_objects) {
            //     for (auto p : it->m_path) {
            //         renderMarker(renderer, camera, p, 0x996666ff);
            //     }
            // }
            renderMarker(renderer, camera, m_cursor, 0x80ff80ff);
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
                    object->setPosition(backup_pos);
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

                vec2f other_pos = other->getPosition();
                vec2i other_ipos = vec2i(round(other_pos.x), round(other_pos.y));
                // vec2f delta = object->getPosition() - other->getPosition();

                if (ipos == other_ipos) {
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


/**
 * A-star pathfinding with diagonal heuristic
 */
std::vector<vec2f> World::buildPath(const vec2f& fstart, const vec2f& fgoal) {
    static const vec2i steps[] = {{0, -1}, {-1, 0}, {+1, 0}, {0, +1}, {-1, -1}, {+1, -1}, {-1, +1}, {+1, +1}};
    static const int weights[] = {1000, 1000, 1000, 1000, 1414, 1414, 1414, 1414}; // M_SQRT2

    struct Node {
        struct Compare {
            inline const bool operator()(const Node* n1, const Node* n2) const {
                return n1->actual + n1->heuristic > n2->actual + n2->heuristic;
            }
        };
        using PrioQ = std::priority_queue<Node*, std::vector<Node*>, Compare>;

        Node(): actual(std::numeric_limits<int>::max()) {}
        Node* parent;
        vec2i idx;
        int   actual;
        int   heuristic;
    };

    std::map<vec2i, Node> m_nodes;
    Node::PrioQ m_queue;

    vec2i start(round(fstart.x), round(fstart.y));
    vec2i goal(round(fgoal.x), round(fgoal.y));

    Node* cur = &m_nodes[start];
    cur->idx = start;
    cur->parent = nullptr;
    cur->actual = 0;
    cur->heuristic = std::numeric_limits<int>::max();

    m_queue.push(cur);

    while (!m_queue.empty() && m_queue.size() < 50) {
        cur = m_queue.top();
        if (cur->idx == goal) {
            break;
        }
        m_queue.pop();

        for (int i = 0; i < 8; ++i) {
            vec2i idx = cur->idx + steps[i];
            bool passable = isPassable(idx);
            if (i > 3) {
                passable = passable && isPassable(cur->idx + vec2i(steps[i].x, 0));
                passable = passable && isPassable(cur->idx + vec2i(0, steps[i].y));
            }

            if (passable) {
                Node* next = &m_nodes[idx];
                int actual = cur->actual + weights[i];

                if (actual < next->actual) {
                    next->idx = idx;
                    next->parent = cur;
                    next->actual = actual;
                    next->heuristic = 1000 * std::max(std::abs(idx.x - goal.x), std::abs(idx.y - goal.y));

                    m_queue.push(next);
                }
            }
        }
    }

    // if we couldn't reach the goal, build path to the closest node instead
    if (cur->idx != goal) {
        for (auto& it : m_nodes) {
            if (it.second.heuristic < cur->heuristic) {
                cur = &it.second;
            }
        }
    }

    std::vector<vec2f> path;
    for (; cur && cur->idx != start; cur = cur->parent) {
        path.push_back((vec2f)(cur->idx));
    }
    return path;
}

/**
 * Check if line from origin to target is blocked.
 *
 * "A Fast Voxel Traversal Algorithm for Ray Tracing" (Amanatides and Woo)
 */
bool World::checkVisible(const vec2f& forigin, const vec2f& ftarget) {
    vec2i cursor(round(forigin.x), round(forigin.y));
    vec2i target(round(ftarget.x), round(ftarget.y));
    vec2f ray = ftarget - forigin;
    ray.normalize();

    int steps = abs(target.x - cursor.x) + abs(target.y - cursor.y);
    int stepX = (ray.x >= 0) ? 1 : -1;
    int stepY = (ray.y >= 0) ? 1 : -1;
    float tMaxX, tMaxY, tDeltaX, tDeltaY;

    tMaxX = tMaxY = tDeltaX = tDeltaY = std::numeric_limits<float>::infinity();

    if (ray.x != 0) {
        tDeltaX = float(stepX) / ray.x;
        tMaxX = (std::round(forigin.x) - (forigin.x + 0.5f) + (ray.x > 0 ? 1.0f : 0.0f)) / ray.x;
    }
    if (ray.y != 0) {
        tDeltaY = float(stepY) / ray.y;
        tMaxY = (std::round(forigin.y) - (forigin.y + 0.5f) + (ray.y > 0 ? 1.0f : 0.0f)) / ray.y;
    }

    for (int i = 0; i < steps; i++) {
        if (tMaxX < tMaxY) {
            cursor.x += stepX;
            tMaxX += tDeltaX;
        }
        else {
            cursor.y += stepY;
            tMaxY += tDeltaY;
        }

        if (!isPassable(cursor)) {
            return false;
        }
    }

    return true;
}
