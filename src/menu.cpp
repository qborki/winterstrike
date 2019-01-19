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
#include <SDL_mixer.h>
#include "game.h"
#include "menu.h"

Menu::Menu(Game& game) :
    State(game),
    m_current(0),
    m_pos(400, 150),
    m_size(256, 64)
{
    std::vector<std::string> options = {"New Game", "Score", "Exit"};

    m_buttons.resize(options.size());
    for (size_t i = 0; i < options.size(); ++i) {
        m_buttons[i].m_id = i;
        m_buttons[i].m_pos = m_pos + vec2i(0, (m_size.y + 16) * i);
        m_buttons[i].m_label.text(m_game, options[i], "LinBiolinum_Rah.ttf", 32, 0x804040ff);
    }

    m_gradient_base.grad(m_game, m_size, 0x404080ff, 0x202040ff);
    m_gradient_hover.grad(m_game, m_size, 0x6060f0ff, 0x404080ff);
    m_caption.m_label.text(m_game, "Winter-Strike", "LinBiolinum_Rah.ttf", 96, 0x404080ff);
    m_caption.m_pos = m_pos - vec2i(0, 96);
}

void Menu::render(SDL_Renderer* renderer) {
    SDL_Rect rect = { 0, 0, 800, 600 };
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 194);
    SDL_RenderFillRect(renderer, &rect);

    m_caption.m_label.render(renderer, m_caption.m_pos);

    for (auto& button : m_buttons) {
        if (button.m_id == m_current) {
            m_gradient_hover.render(renderer, button.m_pos);
            button.m_label.render(renderer, button.m_pos);
        }
        else {
            m_gradient_base.render(renderer, button.m_pos);
            button.m_label.render(renderer, button.m_pos);
        }
    }
}

int Menu::getButtonId(const vec2i& pos) {
    for (auto& button : m_buttons) {
        vec2i local = pos - button.m_pos + m_size / 2;

        if (local.x > 0 && local.y > 0 && local.x < m_size.x && local.y < m_size.y) {
            return button.m_id;
        }
    }
    return -1;
}

void Menu::onEvent(SDL_Event& ev) {
    if (ev.type == SDL_KEYDOWN) {
        if (ev.key.keysym.sym == SDLK_DOWN) {
            m_current = (m_current + 1) % m_buttons.size();
        }
        else if (ev.key.keysym.sym == SDLK_UP) {
            m_current = (m_current - 1 + m_buttons.size()) % m_buttons.size();
        }
        else if (ev.key.keysym.sym == SDLK_ESCAPE) {
            onSelect(-1);
        }
        else if (ev.key.keysym.sym == SDLK_RETURN) {
            onSelect(m_current);
        }
    }
    else if (ev.type == SDL_MOUSEMOTION) {
        int id = getButtonId(vec2i(ev.motion.x, ev.motion.y));
        if (id >= 0 && m_current != id) {
            m_current = id;
        }
    }
    else if (ev.type == SDL_MOUSEBUTTONUP && ev.button.button == SDL_BUTTON_LEFT) {
        onSelect(getButtonId(vec2i(ev.button.x, ev.button.y)));
    }
}

void Menu::onSelect(int id) {
    Mix_PlayChannel(-1, m_game.getSound("hit.ogg"), 0);
    m_game.popState(); // kill this state

    if (id == 0) {
        m_game.popState(); // kill current world if exists
        m_game.pushState(Game::STATE_WORLD); // create new world
    }
    else if (id == 2) {
        m_game.popState(); // kill current world if exists
    }
}

void Menu::update(float dt) {
}
