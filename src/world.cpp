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
#include <algorithm>
#include <queue>
#include <unordered_map>
#include <SDL.h>
#include "game.h"
#include "world.h"
#include "object.h"
#include "character.h"

World::World(Game& game, int seed) :
    State(game),
    m_seed(seed)
{
    SDL_RenderGetLogicalSize(m_game.getRenderer(), &m_viewport.x, &m_viewport.y);

    m_sprites.resize(17);
    m_sprites[0 ].load(m_game, "tiles.png", vec2i(64, 128),  vec2i(32, 112), 0, 1);
    m_sprites[1 ].load(m_game, "tiles.png", vec2i(64, 128),  vec2i(32, 112), 1, 1);
    m_sprites[2 ].load(m_game, "tiles.png", vec2i(64, 128),  vec2i(32, 112), 2, 1);
    m_sprites[3 ].load(m_game, "tiles.png", vec2i(64, 128),  vec2i(32, 112), 3, 1);
    m_sprites[4 ].load(m_game, "tiles.png", vec2i(64, 128),  vec2i(32, 112), 4, 1);
    m_sprites[5 ].load(m_game, "tiles.png", vec2i(64, 128),  vec2i(32, 112), 5, 1);
    m_sprites[6 ].load(m_game, "tiles.png", vec2i(64, 128),  vec2i(32, 112), 6, 1);
    m_sprites[7 ].load(m_game, "tiles.png", vec2i(64, 128),  vec2i(32, 112), 7, 1);
    m_sprites[8 ].load(m_game, "tiles.png", vec2i(64, 128),  vec2i(32, 112), 8, 1);
    m_sprites[9 ].load(m_game, "tiles.png", vec2i(64, 128),  vec2i(32, 112), 9, 1);
    m_sprites[10].load(m_game, "tiles.png", vec2i(64, 128),  vec2i(32, 112), 10, 1);
    m_sprites[11].load(m_game, "tiles.png", vec2i(64, 128),  vec2i(32, 112), 11, 1);
    m_sprites[12].load(m_game, "tiles.png", vec2i(64, 128),  vec2i(32, 112), 12, 1);
    m_sprites[13].load(m_game, "tiles.png", vec2i(64, 128),  vec2i(32, 112), 13, 1);
    m_sprites[14].load(m_game, "tiles.png", vec2i(64, 128),  vec2i(32, 112), 14, 1);
    m_sprites[15].load(m_game, "tiles.png", vec2i(64, 128),  vec2i(32, 112), 15, 1);
    m_sprites[16].load(m_game, "trees.png", vec2i(128, 192), vec2i(64, 160), 0, 1);


    add(std::make_unique<Character>(*this, vec2f(0, 6), false));
    m_player = static_cast<Character*>(m_objects.back().get());

    add(std::make_unique<Character>(*this, vec2f(-3,-5), true));
    add(std::make_unique<Character>(*this, vec2f(-1,-6), true));
    add(std::make_unique<Character>(*this, vec2f( 0,-7), true));
    add(std::make_unique<Character>(*this, vec2f( 1,-6), true));
    add(std::make_unique<Character>(*this, vec2f( 3,-5), true));
}

/**
 * Get vertex height. Each vertex is shared by eight tiles.
 */
int World::getVertexZ(const vec2i& pos) {
    // flat central area
    if (pos.x >= -8 && pos.y >= -8 && pos.x <= 8 && pos.y <= 8) {
        return 0;
    }

    // smooth 3x3
    static const vec2i steps[] = {{0, 0}, {0, -1}, {-1, 0}, {+1, 0}, {0, +1}, {-1, -1}, {+1, -1}, {-1, +1}, {+1, +1}};
    int result = 1;
    for (auto step : steps) {
        result = result && std::hash<vec2i>()(pos + step, m_seed) % 6;
    }
    return result;
}

/**
 * get correct wall tile index
 */
int World::getWallSpriteId(const vec2i& pos) {
    return
        getVertexZ(pos + vec2i(1, 0))
      | getVertexZ(pos + vec2i(0, 0)) << 1
      | getVertexZ(pos + vec2i(1, 1)) << 2
      | getVertexZ(pos + vec2i(0, 1)) << 3;
}

/**
 * Procedural map generation
 */
void World::generate(Tile& tile, const vec2i& pos) {
    int a = getWallSpriteId(pos);

    // if not a getWallSpriteId place some obstacles
    if (a == 0) {
        a = std::hash<vec2i>()(pos, m_seed) % 40;

        // add a tree if there is enough space
        if (a < 1 && getWallSpriteId(pos + vec2i(-1, 1)) == 0 && getWallSpriteId(pos + vec2i(1, -1)) == 0) {
            a = 16 + a;
        }
        else {
            a = -1;
        }
    }

    tile.m_layers[0] = 0;
    tile.m_layers[1] = -1;
    tile.m_layers[2] = a;
    tile.m_passable = (a == -1);
}

/**
 * Get or generate a tile at specified map coordinates
 */
World::Tile& World::getTile(const vec2i& pos) {
    vec2i local_pos = (pos % Chunk::SIZE + vec2i(Chunk::SIZE, Chunk::SIZE)) % Chunk::SIZE;
    vec2i chunk_pos = pos - local_pos;

    Chunk& chunk = m_chunks[chunk_pos];

    if (chunk.m_atime == 0) {
        SDL_LogDebug(SDL_LOG_CATEGORY_TEST, "Create tiles: [%d,%d]:[%d,%d]", chunk_pos.x, chunk_pos.y, chunk_pos.x + Chunk::SIZE, chunk_pos.y + Chunk::SIZE);

        for (int x = 0; x < Chunk::SIZE; ++x) {
            for (int y = 0; y < Chunk::SIZE; ++y) {
                generate(chunk.m_tiles[x][y], chunk_pos + vec2i(x, y));
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
 * Find objects near position.
 */
std::vector<Object*> World::getObjectsInRadius(const vec2f& pos, float radius) {
    std::vector<Object*> result;

    for (auto& object : m_objects) {
        if ((pos - object->getPosition()).length() < radius) {
            result.push_back(object.get());
        }
    }
    return result;
}

/**
 * Debug render tile
 */
void World::renderMarker(SDL_Renderer* renderer, const vec2f& pos, unsigned rgba) {
    vec2i v = worldToScreen(pos);
    SDL_Point points[] = {{v.x, v.y + 16}, {v.x - 32, v.y}, {v.x, v.y - 16}, {v.x + 32, v.y}, {v.x, v.y + 16}};

    SDL_SetRenderDrawColor(renderer, Uint8(rgba >> 24 & 0xff), Uint8((rgba >> 16) & 0xff), Uint8( (rgba >> 8) & 0xff), Uint8(rgba & 0xff));
    SDL_RenderDrawLines(renderer, points, sizeof(points) / sizeof(points[0]));
}

/**
 * Convert world grid coordinates to screen pixel coordinates
 */
const vec2i World::worldToScreen(const vec2f& pos) const {
    vec2f v = (pos - m_camera) * 64;
    return vec2i(std::round((v.x - v.y) / 2), std::round((v.x + v.y) / 4)) + m_viewport / 2;
}

/**
 * Convert screen pixel coordinates to world grid coordinates
 */
const vec2f World::screenToWorld(const vec2i& pos) const {
    vec2i v = pos - m_viewport / 2;
    return vec2f(2.0 * v.y + v.x, 2.0 * v.y - v.x) / 64 + m_camera;
}

/**
 * Main rendering function. Draw world and its objects.
 */
void World::render(SDL_Renderer* renderer) {
    vec2f lt = screenToWorld(vec2i() - m_sprites[16].getOffset()),
          rb = screenToWorld(m_viewport + m_sprites[16].getOffset());

    lt.x = floor(lt.x);
    lt.y = floor(lt.y);
    rb.x = ceil(rb.x);
    rb.y = ceil(rb.y);

    int cx = ceil((rb.x - rb.y - lt.x + lt.y + 1) / 2 + 1);
    int cy = ceil(rb.x + rb.y - lt.x - lt.y + 1);

    for (int z = 0; z < Tile::LAYERS; ++z) {
        vec2f pos = lt;

        for (int a = 0; a < cy; ++a) {
            for (int b = 0; b < cx; ++b) {
                Tile& tile = getTile(vec2i(pos));

                if (tile.m_layers[z] >= 0) {
                    m_sprites[tile.m_layers[z]].render(renderer, worldToScreen(pos), 0, 0);
                }

                pos += vec2f(1, -1);
            }

            for (auto& it : m_objects) {
                vec2f objpos = it->getPosition();

                if (it->getZ() == z && (int)std::round(objpos.x + objpos.y) == (int)(pos.x + pos.y)) {
                    it->render(renderer, worldToScreen(objpos));
                }
            }

            pos += vec2f(-cx, cx);
            pos += a % 2 ? vec2f(1, 0) : vec2f(0, 1);
        }

        if (z == 1) {
            renderMarker(renderer, screenToWorld(m_cursor).round<float>(), 0x80ff80ff);
        }
    }

    for (auto& it : m_objects) {
        if (it->getZ() >= Tile::LAYERS) {
            it->render(renderer, worldToScreen(it->getPosition()));
        }
    }
}

/**
 * Add object to the world
 */
void World::add(std::unique_ptr<Object> object) {
    m_objects.push_back(std::move(object));
}

/**
 * Move and update objects
 */
void World::update(float dt) {
    m_camera = m_player->getPosition();

    // movement and collision detection
    for (size_t i = 0; i < m_objects.size(); ++i) {
        Object& object = *m_objects[i];

        // backup position
        vec2f backup_pos = object.getPosition();

        // update and move object
        object.update(dt);

        // check for collisions
        if (object.isSolid() || object.isCollider()) {
            vec2i ipos = object.getPosition().round<int>();
            if (!isPassable(ipos)) {
                // restore position
                if (object.isSolid()) {
                    object.setPosition(backup_pos);
                }
                // notify collision with world
                if (object.isCollider()) {
                    object.onCollision(nullptr);
                }
            }

            for (size_t j = 0; j < m_objects.size(); ++j) {
                if (i == j) {
                    continue;
                }
                Object& other = *m_objects[j];
                bool solid = object.isSolid() && other.isSolid();
                bool collider = object.isCollider() && other.isCollider();

                if (!solid && !collider) {
                    continue;
                }

                if ((object.getPosition() - other.getPosition()).squareLength() <= 0.5) {
                    // restore position
                    if (solid) {
                        object.setPosition(backup_pos);
                    }
                    // notify collision
                    if (collider) {
                        object.onCollision(&other);
                        other.onCollision(&object);
                    }
                }
            }
        }
    }

    // remove dead
    m_objects.erase(std::remove_if(m_objects.begin(), m_objects.end(), [](const auto& o) { return !o->isAlive(); }), m_objects.end());

    // do z-sorting (FIXME: move to render?)
    std::sort(m_objects.begin(), m_objects.end(), [](const auto& a, const auto& b) {
        vec2f posa = a->getPosition(), posb = b->getPosition();
        int  za = a->getZ(), zb = b->getZ();

        return (za < zb) || ((za == zb) && (posa.x + posa.y) < (posb.x + posb.y));
    });
}

/**
 * 8-direction move cost heuristic
 */
static inline int octile_heuristic(const vec2i& v) {
    int dx = std::abs(v.x), dy = std::abs(v.y);
    return (dx > dy) ? (1000 * dx + 414 * dy) : (1000 * dy + 414 * dx);
}

/**
 * A-star pathfinding
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

    std::unordered_map<vec2i, Node> m_nodes;
    Node::PrioQ m_queue;

    vec2i start = fstart.round<int>();
    vec2i goal = fgoal.round<int>();

    Node* cur = &m_nodes[start];
    cur->idx = start;
    cur->parent = nullptr;
    cur->actual = 0;
    cur->heuristic = octile_heuristic(start - goal);

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

            // allow diagonal movement only if adjacent tiles are passable
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
                    next->heuristic = octile_heuristic(idx - goal);

                    m_queue.push(next);
                }
            }
        }
    }

    // if we couldn't reach the goal, build path to the closest node instead
    if (cur->idx != goal) {
        for (auto& it : m_nodes) {
            if ((it.second.heuristic < cur->heuristic) || ((it.second.heuristic == cur->heuristic) && (it.second.actual < cur->actual))) {
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
 */
bool World::checkVisible(const vec2f& forigin, const vec2f& ftarget) {
    vec2i cursor = forigin.round<int>();
    vec2i target = ftarget.round<int>();
    vec2f ray = ftarget - forigin;
    ray.normalize();

    int steps = std::abs(target.x - cursor.x) + std::abs(target.y - cursor.y);
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

/**
 * Handle user input for playing state
 */
void World::onEvent(SDL_Event& ev) {
    if (ev.type == SDL_MOUSEMOTION) {
        m_cursor = vec2i(ev.motion.x, ev.motion.y);
    }
    else if (ev.type == SDL_MOUSEBUTTONUP && ev.button.button == SDL_BUTTON_LEFT) {
        m_player->walkTo(screenToWorld(vec2i(ev.button.x, ev.button.y)));
    }
    else if (ev.type == SDL_MOUSEBUTTONUP && ev.button.button == SDL_BUTTON_RIGHT) {
        m_player->throwAt(screenToWorld(vec2i(ev.button.x, ev.button.y)));
    }
    else if (ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_ESCAPE) {
        m_game.pushState(Game::STATE_MENU);
    }
    else if (ev.type == SDL_WINDOWEVENT && ev.window.event == SDL_WINDOWEVENT_RESIZED) {
        SDL_RenderGetLogicalSize(m_game.getRenderer(), &m_viewport.x, &m_viewport.y);
    }
}
