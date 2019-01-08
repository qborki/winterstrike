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
#include <SDL.h>
#include <SDL_mixer.h>
#include "game.h"
#include "sprite.h"
#include "world.h"
#include "character.h"
#include "label.h"

static Sprite s_idle  ("character.png", vec2i(128, 128), vec2i(64, 94), 8, 1);
static Sprite s_walk  ("character.png", vec2i(128, 128), vec2i(64, 94), 0, 8);
static Sprite s_throw1("character.png", vec2i(128, 128), vec2i(64, 94), 8, 5);
static Sprite s_throw2("character.png", vec2i(128, 128), vec2i(64, 94), 13, 3);
static Sprite s_hit   ("character.png", vec2i(128, 128), vec2i(64, 94), 16, 8);
static Sprite s_die   ("character.png", vec2i(128, 128), vec2i(64, 94), 24, 8);
static Sprite s_dead  ("character.png", vec2i(128, 128), vec2i(64, 94), 31, 1);

Character::Character() : Object("Character"), m_sprite(&s_idle), m_frame(0), m_hp(100) {
}

void Character::render(SDL_Renderer* renderer, const vec2i& pos) {
    m_sprite->render(renderer, pos, m_facing, m_frame);
}

void Character::update(float dt) {
    // animate
    m_frame += 8 * dt;
    if (m_frame >= m_sprite->getFrames()) {

        if (m_sprite == &s_throw1) {
            Object* o = m_world->spawn("Snowball", m_pos);
            o->setDirection(m_dir);
            o->setOwnerId(m_object_id);
            setSprite(&s_throw2);
        }
        else if (m_sprite == &s_throw2) {
            setSprite(&s_idle);
        }
        else if (m_sprite == &s_hit) {
            setSprite(&s_idle);
        }
        else if (m_sprite == &s_die) {
            setSprite(&s_dead);
            m_z = 0;
        }
        else {
            m_frame = 0; // loop
        }
    }

    // update character position
    if (m_sprite == &s_walk) {
        m_pos += m_dir * 2 * dt;

        // check next waypoint
        if (!m_path.empty() && (*m_path.begin() - m_pos).length() < .1) {
            m_path.pop_front();

            if (m_path.empty()) {
                setSprite(&s_idle);
            }
            else {
                lookAt(*m_path.begin());
            }
        }
    }
}

void Character::setSprite(Sprite* sprite) {
    m_sprite = sprite;
    m_frame = 0;
}

void Character::lookAt(const vec2f& pos) {
    setDirection(pos - m_pos);
}

void Character::throwAt(const vec2f& pos) {
    lookAt(pos);
    setSprite(&s_throw1);
}

void Character::walkTo(const vec2f& pos) {
    buildPathTo(pos);
    lookAt(*m_path.begin());
    setSprite(&s_walk);
}

void Character::onHit(Object* other, int hp) {
    m_hp -= hp;
    if (m_hp > 0) {
        setSprite(&s_hit);
    }
    else {
        setSprite(&s_die);
        m_solid = false;
        m_collider = false;
    }

    if (Mix_PlayChannel(-1, Game::get().getSound("hit.ogg"), 0) < 0) {
        throw std::runtime_error(Mix_GetError());
    }
    Label* label = (Label*) m_world->spawn("Label", m_pos);
    label->setText("-" + std::to_string(hp));
} 

static bool init = Game::get().setFactory("Character", []() {
    return (Object*) new Character();
});
