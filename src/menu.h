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
#include "state.h"
#include "sprite.h"

class Menu: public State {
public:
    Menu(Game&);
    void render(SDL_Renderer* renderer);
    void onEvent(SDL_Event& ev);
    void update(float dt);
private:
    struct Button {
        int    m_id;
        Sprite m_label;
        vec2i  m_pos;
    };
    int getButtonId(const vec2i& pos);
    void onSelect(int);

    int m_current;
    vec2i m_pos;
    vec2i m_size;

    Button m_caption;
    Sprite m_gradient_base;
    Sprite m_gradient_hover;
    std::vector<Button> m_buttons;
};

#endif
