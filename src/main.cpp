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

int main(int argc, char** argv) try {
    Game game;
    game.init(argc, argv);
    game.pushState(Game::STATE_MENU);
    game.run();
    return 0;
}
catch (std::exception& e) {
    SDL_LogCritical(SDL_LOG_CATEGORY_ERROR, "%s", e.what());
    return -1;
}
