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
#include "game.h"
#include "label.h"

Label::Label(): Object("Label"), m_ttl(1) {
    m_solid = false;
}

void Label::setText(const std::string& text) {
    m_text = text;
    m_sprite.destroy();
}

void Label::render(SDL_Renderer* renderer, const vec2i& pos) {
    if (!m_sprite.exists()) {
        m_sprite.createFromText(renderer, m_text, "LinBiolinum_Rah.ttf", 24, 64, 128, 64);
    }
    vec2f scale ((1 - 0.25 * m_ttl*m_ttl), (1 - 0.25 * m_ttl*m_ttl));
    vec2i dst = pos + vec2i(0, - 96 - 32 + int(32 * m_ttl));

    m_sprite.render(renderer, dst, 0, 0, scale);
}

void Label::update(float dt) {
    m_ttl -= dt * 0.5;
    if (m_ttl < 0) {
        m_ttl = 0;
        m_alive = 0;
    }
}

static bool init = Game::get().setFactory("Label", []() {
    return (Object*) new Label();
});
