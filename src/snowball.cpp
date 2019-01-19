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
#include <SDL.h>
#include <SDL_mixer.h>
#include "game.h"
#include "world.h"
#include "snowball.h"

Snowball::Snowball(World& world, const vec2f& pos, const vec2f& dir, int owner_id) :
    Object(world, "Snowball", pos),
    m_dir(dir),
    m_state(SNOWBALL),
    m_frame(0),
    m_speed(16),
    m_height(64),
    m_ttl(1)
{
    m_sprites.resize(3);
    m_sprites[SHADOW  ].load(m_world.getGame(), "snowball.png", vec2i(64, 64), vec2i(32, 12), 0, 1);
    m_sprites[SNOWBALL].load(m_world.getGame(), "snowball.png", vec2i(64, 64), vec2i(32, 12), 1, 1);
    m_sprites[EXPLODE ].load(m_world.getGame(), "snowball.png", vec2i(64, 64), vec2i(32, 12), 1, 8);

    m_solid = false;
    m_owner_id = owner_id;
}

void Snowball::render(SDL_Renderer* renderer, const vec2i& pos) {
    if (m_state == SNOWBALL) {
        m_sprites[SHADOW].render(renderer, vec2i(pos.x, floor(pos.y)), 0, 0);
    }
    m_sprites[m_state].render(renderer, vec2i(pos.x, floor(pos.y - m_height)), 0, (int)m_frame);
}

void Snowball::update(float dt) {
    if (m_state == SNOWBALL) {
        m_pos += m_dir * m_speed * dt;
        m_ttl -= dt;
        if (m_ttl < 0) {
            m_state = EXPLODE;
        }
    }
    else if (m_state == EXPLODE) {
        m_frame += 8 * dt;
        if (m_frame >= m_sprites[m_state].getFrames()) {
            m_frame = 0;
            m_alive = false;
        }
    }
}

void Snowball::onCollision(Object* other) {
    // check owner so we don't get hit by own projectiles
    if (m_state == SNOWBALL && (other == nullptr || other->getObjectId() != m_owner_id)) {
        m_state = EXPLODE;
        m_solid = false;
        m_collider = false;

        if (Mix_PlayChannel(-1, m_world.getGame().getSound("hit.ogg"), 0) < 0) {
            throw std::runtime_error(Mix_GetError());
        }
        if (other) {
            other->onHit(this, 25);
        }
    }
} 
