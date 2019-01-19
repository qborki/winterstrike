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
#include <algorithm> // random_shuffle
#include "sprite.h"
#include "world.h"
#include "character.h"
#include "snowball.h"
#include "label.h"

Character::Character(World& world, const vec2f& pos, bool ai) :
    Object(world, ai ? "CharacterAI" : "Character", pos),
    m_dir(1, 0),
    m_facing(getFacing(m_dir)),
    m_state(IDLE),
    m_frame(0),
    m_hp(100),
    m_ai(ai)
{
    std::string file = std::string("character-") + (m_ai ? "blue" : "red") + ".png";

    m_sprites.resize(7);
    m_sprites[IDLE  ].load(m_world.getGame(), file, vec2i(128, 128), vec2i(64, 94), 8, 1);
    m_sprites[WALK  ].load(m_world.getGame(), file, vec2i(128, 128), vec2i(64, 94), 0, 8);
    m_sprites[THROW1].load(m_world.getGame(), file, vec2i(128, 128), vec2i(64, 94), 8, 5);
    m_sprites[THROW2].load(m_world.getGame(), file, vec2i(128, 128), vec2i(64, 94), 13, 3);
    m_sprites[HIT   ].load(m_world.getGame(), file, vec2i(128, 128), vec2i(64, 94), 16, 8);
    m_sprites[DIE   ].load(m_world.getGame(), file, vec2i(128, 128), vec2i(64, 94), 24, 8);
    m_sprites[DEAD  ].load(m_world.getGame(), file, vec2i(128, 128), vec2i(64, 94), 31, 1);
}

void Character::render(SDL_Renderer* renderer, const vec2i& pos) {
    m_sprites[m_state].render(renderer, pos, m_facing, m_frame);
}

void Character::update(float dt) {
    // animate
    m_frame += 8 * dt;
    if (m_frame >= m_sprites[m_state].getFrames()) {
        if (m_state == THROW1) {
            m_world.add(std::make_unique<Snowball>(m_world, m_pos, m_dir, m_object_id));
            setState(THROW2);
        }
        else if (m_state == THROW2) {
            setState(IDLE);
        }
        else if (m_state == HIT) {
            setState(IDLE);
        }
        else if (m_state == DIE) {
            setState(DEAD);
            m_z = 1;
            m_classname = "Corpse";

            if (!m_ai) {
                m_world.add(std::make_unique<Label>(m_world, m_pos, std::string("Game over"), 96, 0x804040ff, 5));
            }
        }
        else {
            m_frame = 0; // loop
        }
    }

    // update character position
    if (m_state == WALK) {
        m_pos += m_dir * 4 * dt;

        // check next waypoint
        if (!m_path.empty() && (m_path.back() - m_pos).length() < .1) {
            m_path.pop_back();

            if (m_path.empty()) {
                setState(IDLE);
            }
            else {
                lookAt(m_path.back());
            }
        }
    }

   // idle AIs might do something
   if (m_ai && m_state == IDLE && std::rand() < RAND_MAX / 64) {
       bool attack = false;
   
       // attack someone
       if (std::rand() % 2) {
           std::vector<Object*> objects(m_world.getObjectsInRadius(m_pos, 16));
           std::random_shuffle(objects.begin(), objects.end());
   
           // find someone we can attack
           for (auto object : objects) {
               if (object != this && object->getClassname() == "Character") {
                   // check if target is reachable
                   if (m_world.checkVisible(m_pos, object->getPosition())) {
                       throwAt(object->getPosition());
                       attack = true;
                       break;
                   }
               }
           }
       }
   
       // or try to move closer
       if (!attack) {
           std::vector<Object*> objects(m_world.getObjectsInRadius(m_pos, 64));
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
               if (m_world.isPassable((vec2i)dst)) {
                   walkTo(dst);
                   break;
               }
           }
       }
   }
}

void Character::setState(int state) {
    if (m_state != state) {
        m_state = state;
        m_frame = 0;
    }
}

/**
 * Calculate facing direction (as in sprites) from a vector
 */
int Character::getFacing(const vec2f& dir) {
    float a = dir.x != 0 ? (dir.y / dir.x) : (dir.y > 0 ? 999 : -999);

    if (a > 2.4142135623731) { // tn(67.5)
        return dir.x < 0 ? 4 : 0;
    }
    else if (a > 0.4142135623731) { // tn(22.5)
        return dir.x < 0 ? 5 : 1;
    }
    else if (a > -0.4142135623731) { // tn(-22.5)
        return dir.x < 0 ? 6 : 2;
    }
    else if (a > -2.4142135623731) { // tn(-67.5)
        return dir.x < 0 ? 7 : 3;
    }
    else {
        return dir.x < 0 ? 0 : 4;
    }
}

void Character::lookAt(const vec2f& pos) {
    m_dir = pos - m_pos;
    m_dir.normalize();
    m_facing = getFacing(m_dir);
}

void Character::throwAt(const vec2f& pos) {
    if (m_state == IDLE || m_state == WALK || m_state == THROW1) {
        lookAt(pos);
        setState(THROW1);
    }
}

void Character::walkTo(const vec2f& pos) {
    if (m_state == IDLE || m_state == WALK || m_state == THROW1) {
        m_path = m_world.buildPath(m_pos, pos);

        if (!m_path.empty()) {
            lookAt(m_path.back());
            setState(WALK);
        }
    }
}

void Character::onCollision(Object* other) {
    if (m_state == WALK) {
        setState(IDLE);
    }
}

void Character::onHit(Object* other, int hp) {
    m_hp -= hp;
    if (m_hp > 0) {
        setState(HIT);
    }
    else {
        setState(DIE);
        m_solid = false;
        m_collider = false;
    }
    m_world.add(std::make_unique<Label>(m_world, m_pos, "-" + std::to_string(hp), 16, 0x408040ff));
} 
