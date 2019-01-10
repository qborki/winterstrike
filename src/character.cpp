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
#include <algorithm>
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

Character::Character(bool ai) : Object("Character"), m_sprite(&s_idle), m_frame(0), m_hp(100), m_ai(ai) {
}

void Character::render(SDL_Renderer* renderer, const vec2i& pos) {
    m_sprite->render(renderer, pos, m_facing, m_frame);
}

void Character::update(float dt) {
    // animate
    m_frame += 8 * dt;
    if (m_frame >= m_sprite->getFrames()) {

        if (m_sprite == &s_throw1) {
            setSprite(&s_throw2);

            Object* o = m_world->spawn("Snowball", m_pos);
            o->setDirection(m_dir);
            o->setOwnerId(m_object_id);
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
            m_classname = "Corpse";
        }
        else {
            m_frame = 0; // loop
        }
    }

    // update character position
    if (m_sprite == &s_walk) {
        m_pos += m_dir * 2 * dt;

        // check next waypoint
        if (!m_path.empty() && (m_path.back() - m_pos).length() < .1) {
            m_path.pop_back();

            if (m_path.empty()) {
                setSprite(&s_idle);
            }
            else {
                lookAt(m_path.back());
            }
        }
    }

    // idle AIs might do something
    if (m_ai && m_sprite == &s_idle && std::rand() < RAND_MAX / 128) {
        bool attack = false;

        // attack someone
        if (std::rand() % 2) {
            std::vector<Object*> objects = m_world->getObjectsInRadius(m_pos, 5);
            std::random_shuffle(objects.begin(), objects.end());

            // find someone we can attack
            for (auto object : objects) {
                if (object != this && object->getClassname() == "Character") {
                    // check if target is reachable
                    if (m_world->checkVisible(m_pos, object->getPosition())) {
                        throwAt(object->getPosition());
                        attack = true;
                        break;
                    }
                }
            }
        }
        // walk
        if (!attack) {
            for (int i = 0; i < 10; i++) {
                vec2f dst = m_pos + vec2f(std::rand() % 6 - 3, std::rand() % 6 - 3);
                if (m_world->isPassable((vec2i)dst)) {
                    walkTo(dst);
                    break;
                }
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
    m_path = m_world->buildPath(m_pos, pos);
    lookAt(m_path.back());
    setSprite(&s_walk);
}

void Character::onCollision(Object* other) {
    if (m_sprite == &s_walk) {
        setSprite(&s_idle);
    }
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
    Label* label = (Label*) m_world->spawn("Label", m_pos);
    label->setText("-" + std::to_string(hp));
} 

static bool init[] = {
    Game::get().setFactory("Character", []() {
        return (Object*) new Character(false);
    }),
    Game::get().setFactory("CharacterAi", []() {
        return (Object*) new Character(true);
    })
};
