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

Character::Character(bool ai):
    Object(ai ? "CharacterAI" : "Character"),
    m_sprite(&m_idle),
    m_frame(0),
    m_hp(100),
    m_ai(ai),
    m_spritesheet(std::string("character-") + (m_ai ? "blue" : "red") + ".png"),
    m_idle  (m_spritesheet, vec2i(128, 128), vec2i(64, 94), 8, 1),
    m_walk  (m_spritesheet, vec2i(128, 128), vec2i(64, 94), 0, 8),
    m_throw1(m_spritesheet, vec2i(128, 128), vec2i(64, 94), 8, 5),
    m_throw2(m_spritesheet, vec2i(128, 128), vec2i(64, 94), 13, 3),
    m_hit   (m_spritesheet, vec2i(128, 128), vec2i(64, 94), 16, 8),
    m_die   (m_spritesheet, vec2i(128, 128), vec2i(64, 94), 24, 8),
    m_dead  (m_spritesheet, vec2i(128, 128), vec2i(64, 94), 31, 1)
{
}

void Character::render(SDL_Renderer* renderer, const vec2i& pos) {
    m_sprite->render(renderer, pos, m_facing, m_frame);
}

void Character::update(float dt) {
    // animate
    m_frame += 8 * dt;
    if (m_frame >= m_sprite->getFrames()) {

        if (m_sprite == &m_throw1) {
            setSprite(&m_throw2);

            Object* o = m_world->spawn("Snowball", m_pos);
            o->setDirection(m_dir);
            o->setOwnerId(m_object_id);
        }
        else if (m_sprite == &m_throw2) {
            setSprite(&m_idle);
        }
        else if (m_sprite == &m_hit) {
            setSprite(&m_idle);
        }
        else if (m_sprite == &m_die) {
            setSprite(&m_dead);
            m_z = 1;
            m_classname = "Corpse";

            if (!m_ai) {
                Label* label = (Label*) m_world->spawn("Label", m_pos);
                label->setSize(96);
                label->setColor(0x804040ff);
                label->setText("Game over");
            }
        }
        else {
            m_frame = 0; // loop
        }
    }

    // update character position
    if (m_sprite == &m_walk) {
        m_pos += m_dir * 4 * dt;

        // check next waypoint
        if (!m_path.empty() && (m_path.back() - m_pos).length() < .1) {
            m_path.pop_back();

            if (m_path.empty()) {
                setSprite(&m_idle);
            }
            else {
                lookAt(m_path.back());
            }
        }
    }

    // idle AIs might do something
    if (m_ai && m_sprite == &m_idle && std::rand() < RAND_MAX / 64) {
        bool attack = false;

        // attack someone
        if (std::rand() % 2) {
            std::vector<Object*> objects = m_world->getObjectsInRadius(m_pos, 16);
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

        // or try to move closer
        if (!attack) {
            std::vector<Object*> objects = m_world->getObjectsInRadius(m_pos, 64);
            std::random_shuffle(objects.begin(), objects.end());
            Object* object = this;

            for (auto other : objects) {
                if (other != this && other->getClassname() == "Character") {
                    object = other;
                    break;
                }
            }

            // find open spot
            for (int i = 0; i < 10; i++) {
                vec2f dst = object->getPosition() + vec2f(std::rand() % 6 - 3, std::rand() % 6 - 3);
                if (m_world->isPassable((vec2i)dst)) {
                    walkTo(dst);
                    break;
                }
            }
        }
    }
}

void Character::setSprite(Sprite* sprite) {
    if (m_sprite != sprite) {
        m_sprite = sprite;
        m_frame = 0;
    }
}

void Character::lookAt(const vec2f& pos) {
    setDirection(pos - m_pos);
}

void Character::throwAt(const vec2f& pos) {
    if (m_sprite == &m_idle || m_sprite == &m_walk || m_sprite == &m_throw1) {
        lookAt(pos);
        setSprite(&m_throw1);
    }
}

void Character::walkTo(const vec2f& pos) {
    if (m_sprite == &m_idle || m_sprite == &m_walk || m_sprite == &m_throw1) {
        m_path = m_world->buildPath(m_pos, pos);

        if (!m_path.empty()) {
            lookAt(m_path.back());
            setSprite(&m_walk);
        }
    }
}

void Character::onCollision(Object* other) {
    if (m_sprite == &m_walk) {
        setSprite(&m_idle);
    }
}

void Character::onHit(Object* other, int hp) {
    m_hp -= hp;
    if (m_hp > 0) {
        setSprite(&m_hit);
    }
    else {
        setSprite(&m_die);
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
