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
#ifndef STATE_H
#define STATE_H

class Game;
struct SDL_Renderer;
struct SDL_Texture;
typedef union SDL_Event SDL_Event;

class State {
public:
    State(Game& game): m_game(game) {}
    virtual ~State() {}
    virtual void render(SDL_Renderer* renderer) = 0;
    virtual void onEvent(SDL_Event& ev) = 0;
    virtual void update(float dt) = 0;

protected:
    Game& m_game;
};

#endif
