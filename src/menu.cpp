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
#include "menu.h"

Menu::Menu(): m_current(0), m_items({"New Game", "Options", "Exit"}) {
    SDL_ShowCursor(SDL_ENABLE);
}

Menu::~Menu() {
    SDL_ShowCursor(SDL_DISABLE);
}

void Menu::render(SDL_Renderer* renderer) {
    if (m_sprites.empty()) {
        m_sprites.resize(m_items.size());

        for (size_t i = 0; i < m_items.size(); ++i) {
            m_sprites[i].createFromText(renderer, m_items[i], "LinBiolinum_Rah.ttf", 32, 0x804040ff);
        }
        m_gradient.createFromVGradient(renderer, 200, 60, 0x404080ff, 0x202040ff);
    }

    if (!m_sprites.empty()) {
        SDL_Rect rect = { 300, 100, 200, int(60 * m_sprites.size())};
        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 255);
        SDL_RenderFillRect(renderer, &rect);

        m_gradient.render(renderer, vec2i(400, 130 + 60 * m_current));
        for (size_t i = 0; i < m_sprites.size(); ++i) {
            m_sprites[i].render(renderer, vec2i(400, 130 + 60 * i));
        }
    }
}

void Menu::onEvent(SDL_Event& ev) {
    if (ev.type == SDL_KEYDOWN) {
        if (ev.key.keysym.sym == SDLK_DOWN) {
            m_current = (m_current + 1) % m_items.size();
        }
        else if (ev.key.keysym.sym == SDLK_UP) {
            m_current = (m_current - 1 + m_items.size()) % m_items.size();
        }
        else if (ev.key.keysym.sym == SDLK_ESCAPE) {
            Game::get().pushEvent(Game::EV_MENU_SELECT, this, (void*)(uintptr_t)-1);
        }
        else if (ev.key.keysym.sym == SDLK_RETURN) {
            Game::get().pushEvent(Game::EV_MENU_SELECT, this, (void*)(uintptr_t)m_current);
        }
    }
}

void Menu::update(float dt) {
}
