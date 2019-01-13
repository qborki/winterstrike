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
#include "sprite.h"
#include "object.h"

static Sprite s_snowball("snowball.png", vec2i(64, 64), vec2i(32, 12), 0, 2);
static Sprite s_explode("snowball.png",  vec2i(64, 64), vec2i(32, 12), 1, 8);

class Snowball: public Object {
public:
    Snowball(): Object("Snowball"), m_sprite(&s_snowball), m_frame(0), m_speed(16), m_height(64), m_ttl(1) {
        m_solid = false;
    }

    void render(SDL_Renderer* renderer, const vec2i& pos) {
        if (m_sprite == &s_snowball) {
            // shadow
            m_sprite->render(renderer, vec2i(pos.x, floor(pos.y)), 0, 0);
            m_sprite->render(renderer, vec2i(pos.x, floor(pos.y - m_height)), 0, 1);
        }
        else {
            m_sprite->render(renderer, vec2i(pos.x, floor(pos.y - m_height)), 0, (int)m_frame);
        }
    }

    void update(float dt) {
        if (m_sprite == &s_snowball) {
            m_pos += m_dir * m_speed * dt;
            m_ttl -= dt;
            if (m_ttl < 0) {
                m_sprite = &s_explode;
            }
        }
        else if (m_sprite == &s_explode) {
            m_frame += 8 * dt;
            if (m_frame >= m_sprite->getFrames()) {
                m_frame = 0;
                m_alive = false;
            }
        }
    }

    void onCollision(Object* other) {
        // check owner so we don't get hit by own projectiles
        if (m_sprite == &s_snowball && (other == nullptr || other->getObjectId() != m_owner_id)) {
            m_sprite = &s_explode;
            m_solid = false;
            m_collider = false;

            if (Mix_PlayChannel(-1, Game::get().getSound("hit.ogg"), 0) < 0) {
                throw std::runtime_error(Mix_GetError());
            }
            if (other) {
                other->onHit(this, 25);
            }
        }
    } 

private:
    Sprite* m_sprite;
    float m_frame;
    float m_speed;
    float m_height;
    float m_ttl;
};

static bool init = Game::get().setFactory("Snowball", []() {
    return (Object*) new Snowball();
});
