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
#include <unordered_map>
#include <SDL.h>
#include "object.h"

int Object::max_object_id = 0;

Object::Object(World& world, const std::string& classname, const vec2f& pos) :
    m_world(world),
    m_classname(classname),
    m_object_id(max_object_id++),
    m_pos(pos),
    m_z(2),
    m_alive(true),
    m_solid(true),
    m_collider(true)
{
    SDL_LogDebug(SDL_LOG_CATEGORY_TEST, "Create %s  (object #%d)", m_classname.c_str(), m_object_id);
}

Object::~Object() {
    SDL_LogDebug(SDL_LOG_CATEGORY_TEST, "Delete %s  (object #%d)", m_classname.c_str(), m_object_id);
}

void Object::render(SDL_Renderer* renderer, const vec2i& screenCoords) {
}

void Object::update(float dt) {
}

void Object::onCollision(Object* other) {
}

void Object::onHit(Object* other, int hp) {
}
