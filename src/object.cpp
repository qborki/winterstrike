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
#define _USE_MATH_DEFINES
#include <cmath>
#include <limits>
#include <stdexcept>
#include <SDL.h>
#include "object.h"
#include "world.h"

int Object::max_object_id = 0;

Object::Object(const std::string& classname) :
    m_classname(classname),
    m_object_id(max_object_id++),
    m_world(nullptr),
    m_dir(1, 0),
    m_z(2),
    m_facing(0),
    m_alive(true),
    m_solid(true),
    m_collider(true) {

    SDL_LogDebug(SDL_LOG_CATEGORY_TEST, "Create %s  (object #%d)", m_classname.c_str(), m_object_id);
}

Object::~Object() {
    SDL_LogDebug(SDL_LOG_CATEGORY_TEST, "Delete %s  (object #%d)", m_classname.c_str(), m_object_id);
}

void Object::render(SDL_Renderer* renderer, const vec2i& screenCoords) {
}

void Object::update(float dt) {
}

/**
 * Calculate facing direction (as in sprites) from vector
 */
void Object::setDirection(const vec2f& dir) {
    float a = dir.x != 0 ? (dir.y / dir.x) : (dir.y > 0 ? 999 : -999);

    if (a > 2.4142135623731) { // tn(67.5)
        m_facing = dir.x < 0 ? 4 : 0;
    }
    else if (a > 0.4142135623731) { // tn(22.5)
        m_facing = dir.x < 0 ? 5 : 1;
    }
    else if (a > -0.4142135623731) { // tn(-22.5)
        m_facing = dir.x < 0 ? 6 : 2;
    }
    else if (a > -2.4142135623731) { // tn(-67.5)
        m_facing = dir.x < 0 ? 7 : 3;
    }
    else {
        m_facing = dir.x < 0 ? 0 : 4;
    }

    m_dir = dir;
    m_dir.normalize();
}

void Object::onCollision(Object* other) {
}

void Object::onHit(Object* other, int hp) {
}
