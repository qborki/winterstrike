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
#ifndef _CHARACTER_H
#define _CHARACTER_H

#include "object.h"

class Sprite;

class Character: public Object {
public:
    Character();

    void render(SDL_Renderer* renderer, const vec2i& pos);
    void update(float dt);

    void walkTo(const vec2f& pos);
    void lookAt(const vec2f& pos);
    void throwAt(const vec2f& pos);

    void onHit(Object* other, int hp);
private:
    void setSprite(Sprite* );

    Sprite* m_sprite;
    float m_frame;
    int m_hp;
};
#endif
