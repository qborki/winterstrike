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
#ifndef _OBJECT_H
#define _OBJECT_H

#include <string>
#include <list>
#include <queue>
#include <map>
#include "vec.h"

class World;
struct SDL_Renderer;

class Object {
public:
    Object(const std::string& className);
    virtual ~Object();

    virtual void render(SDL_Renderer* renderer, const vec2i& screenCoords);
    virtual void update(float dt);

    inline const std::string& getClassname() const {
        return m_classname;
    }

    inline const vec2f& getPosition() const {
        return m_pos;
    }

    inline const vec2f& getDirection() const {
        return m_dir;
    }

    inline const int getZ() const {
        return m_z;
    }

    inline const bool isAlive() const {
        return m_alive;
    }

    inline const bool isSolid() const {
        return m_solid;
    }

    inline const bool isCollider() const {
        return m_collider;
    }

    inline const int getObjectId() const {
        return m_object_id;
    }

    inline const int getOwnerId() const {
        return m_owner_id;
    }

    inline void setWorld(World* world) {
        m_world = world;
    }

    inline void setOwnerId(int object_id) {
        m_owner_id = object_id;
    }

    inline void setPosition(const vec2f& pos) {
        m_pos = pos;
    }

    void setDirection(const vec2f& dir);

    inline void setZ(int z) {
        m_z = z;
    }

    void buildPathTo(const vec2f& pos);

    // events
    virtual void onCollision(Object* other);
    virtual void onHit(Object* other, int hp);

protected:
    static int max_object_id;

    std::string m_classname;

    World* m_world;
    vec2f  m_pos;
    vec2f  m_dir;
    int    m_z;
    int    m_facing;
    bool   m_alive;
    bool   m_solid;    // object blocks movement
    bool   m_collider; // object may collide with other objects
    int    m_object_id;
    int    m_owner_id;

    // AStar pathfinding
    struct Node {
        struct Compare {
            inline const bool operator()(const Node* n1, const Node* n2) const {
                return n1->actual + n1->heuristic > n2->actual + n2->heuristic;
            }
        };
        using PrioQ = std::priority_queue<Node*, std::vector<Node*>, Compare>;

        Node(): actual(99999) {}
        vec2i idx;
        Node* parent;
        float actual;
        float heuristic;
    };

    std::map<vec2i, Node> m_nodes;
    Node::PrioQ m_queue;
    std::list<vec2f> m_path;
};
#endif
