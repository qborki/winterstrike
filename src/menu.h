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
#ifndef MENU_H
#define MENU_H

#include <vector>
#include "sprite.h"

struct SDL_Renderer;
struct SDL_Texture;
typedef union SDL_Event SDL_Event;

class Menu {
public:
    Menu();
    ~Menu();

    void render(SDL_Renderer* renderer);
    void onEvent(SDL_Event& ev);
    void update(float dt);

private:
    std::vector<std::string> m_items;
    std::vector<Sprite> m_sprites;
    Sprite m_gradient;

    int m_current;
};

#endif
