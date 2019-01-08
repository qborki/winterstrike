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
#include <cmath>
#include "game.h"
#include "camera.h"

Camera::Camera(): Object("Camera") {
    m_solid = false;
    m_collider = false;
}

void Camera::setSize(const vec2i& size) {
    m_size = size;
}

const vec2i Camera::getSize() const {
    return m_size;
}

const vec2i Camera::worldToScreen(const vec2f& pos) const {
    vec2f v = (pos - m_pos) * 128;
    
    return vec2i(round((v.x - v.y) / 2), round((v.x + v.y) / 4)) + m_size / 2;
}

const vec2f Camera::screenToWorld(const vec2i& pos) const {
    vec2i v = pos - m_size / 2;

    return vec2f(2.0 * v.y + v.x, 2.0 * v.y - v.x) / 128 + m_pos;
}

static bool init = Game::get().setFactory("Camera", []() {
    return (Object*) new Camera();
});
