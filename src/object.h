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
#ifndef OBJECT_H
#define OBJECT_H

#include <string>
#include "vec.h"

class World;
struct SDL_Renderer;

class Object {
public:
    Object(World& world, const std::string& className, const vec2f& pos = vec2f());
    virtual ~Object();

    virtual void render(SDL_Renderer* renderer, const vec2i& screenCoords);
    virtual void update(float dt);

    inline const std::string& getClassname() const {
        return m_classname;
    }

    inline const vec2f& getPosition() const {
        return m_pos;
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

    inline void setOwnerId(int object_id) {
        m_owner_id = object_id;
    }

    inline void setPosition(const vec2f& pos) {
        m_pos = pos;
    }

    inline void setZ(int z) {
        m_z = z;
    }

    // events
    virtual void onCollision(Object* other);
    virtual void onHit(Object* other, int hp);
protected:
    static int max_object_id;

    World&      m_world;
    std::string m_classname;
    int         m_object_id;
    vec2f       m_pos;
    int         m_z;
    bool        m_alive;
    bool        m_solid;    // object blocks movement
    bool        m_collider; // object may collide with other objects
    int         m_owner_id;
};
#endif
