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
#ifndef CHARACTER_H
#define CHARACTER_H

#include <vector>
#include "object.h"
#include "sprite.h"

class Character: public Object {
public:
    enum {IDLE, WALK, THROW1, THROW2, HIT, DIE, DEAD};

    Character(World&, const vec2f& pos, bool ai);

    void render(SDL_Renderer* renderer, const vec2i& pos);
    void update(float dt);

    void walkTo(const vec2f& pos);
    void lookAt(const vec2f& pos);
    void throwAt(const vec2f& pos);

    void onCollision(Object* other);
    void onHit(Object* other, int hp);
private:
    void setState(int state);
    int  getFacing(const vec2f& dir);

    vec2f  m_dir;
    int    m_facing;
    int    m_state;
    float  m_frame;
    int    m_hp;
    bool   m_ai;

    std::vector<vec2f> m_path;
    std::vector<Sprite> m_sprites;
};
#endif
