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
#include "game.h"
#include "object.h"

class Cursor: public Object {
public:
    Cursor(): Object("Cursor"), m_rgba(0x80ff80ff) {
        m_collider = false;
        m_solid = false;
        m_z = 1;
    }

    void render(SDL_Renderer* renderer, const vec2i& pos) {
        SDL_Point points[] = {{pos.x, pos.y + 16}, {pos.x - 32, pos.y}, {pos.x, pos.y - 16}, {pos.x + 32, pos.y}, {pos.x, pos.y + 16}};
        SDL_SetRenderDrawColor(renderer, Uint8(m_rgba >> 24 & 0xff), Uint8((m_rgba >> 16) & 0xff), Uint8( (m_rgba >> 8) & 0xff), Uint8(m_rgba & 0xff));
        SDL_RenderDrawLines(renderer, points, sizeof(points) / sizeof(points[0]));
    }

private:
    int m_rgba;
};

static bool init = Game::get().setFactory("Cursor", []() {
    return (Object*) new Cursor();
});
