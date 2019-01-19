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
#ifndef SNOWBALL_H
#define SNOWBALL_H

#include <vector>
#include "object.h"
#include "sprite.h"

class Snowball: public Object {
public:
    enum { SHADOW, SNOWBALL, EXPLODE };

    Snowball(World& world, const vec2f& pos, const vec2f& dir, int owner);

    void render(SDL_Renderer* renderer, const vec2i& pos);
    void update(float dt);

    void onCollision(Object* other);
private:
    vec2f m_dir;
    int   m_state;
    float m_frame;
    float m_speed;
    float m_height;
    float m_ttl;

    std::vector<Sprite> m_sprites;
};
#endif
