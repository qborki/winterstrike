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
#include "world.h"
#include "label.h"

Label::Label(World& world, const vec2f& pos, const std::string& text, unsigned size, unsigned rgba, float ttl) :
    Object(world, "Label", pos),
    m_age(0),
    m_ttl(ttl),
    m_factor(0)
{
    m_solid = false;
    m_collider = false;
    m_sprite.text(m_world.getGame(), text, "BebasNeue.otf", size, rgba);
}


void Label::render(SDL_Renderer* renderer, const vec2i& pos) {
    m_sprite.render(renderer, pos + vec2i(0, -64 * (1 + m_factor)), 0, 0, vec2f(0.5, 0.5) * (1 + m_factor));
}

void Label::update(float dt) {
    m_age += dt;
    if (m_age >= m_ttl) {
        m_age = m_ttl;
        m_alive = 0;
    }

    // inverse cubic
    m_factor = m_age / m_ttl;
    m_factor = 1 - m_factor;
    m_factor = m_factor * m_factor * m_factor;
    m_factor = 1 - m_factor;
}
