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
#include <stdexcept>
#include <SDL.h>
#include <SDL_ttf.h>
#include "game.h"
#include "sprite.h"

Sprite::Sprite() :
    m_texture(nullptr),
    m_must_destroy(false),
    m_start(0),
    m_count(0)
{
}

Sprite::Sprite(const std::string& filename, const vec2i& size, const vec2i& offset, int start, int count) :
    m_filename(filename),
    m_texture(nullptr),
    m_must_destroy(false),
    m_size(size),
    m_offset(offset),
    m_start(start),
    m_count(count)
{
    // TODO: queue for preloading once renderer is ready
}

Sprite::~Sprite() {
    destroy();
}

/**
 * Unload texture if owned
 */
void Sprite::destroy() {
    if (m_must_destroy && m_texture) {
        SDL_DestroyTexture(m_texture);
    }
    m_texture = nullptr;
}

/**
 * Render sprite frame at specified position
 */
void Sprite::render(SDL_Renderer* renderer, const vec2i& pos, int side, int frame, const vec2f& scale) {
    // lazy loading
    if (m_texture == nullptr && m_filename != "") {
        createFromFile(renderer, m_filename);
    }

    if (m_texture != nullptr) {
        SDL_Rect dst = {
            x: int(pos.x - m_offset.x * scale.x),
            y: int(pos.y - m_offset.y * scale.y),
            w: int(m_size.x * scale.x),
            h: int(m_size.y * scale.y)
        };
        SDL_Rect src = {
            x: m_size.x * (m_start + frame),
            y: m_size.y * side,
            w: m_size.x,
            h: m_size.y
        };

        if (SDL_RenderCopy(renderer, m_texture, &src, &dst) < 0) {
            throw std::runtime_error(SDL_GetError());
        }
    }
}

/**
 * Get cached texture from resource manager
 */
void Sprite::createFromFile(SDL_Renderer* renderer, const std::string& filename) {
    int w, h;

    destroy();

    m_texture = Game::get().getTexture(filename);
    m_must_destroy = false;

    if (SDL_SetTextureBlendMode(m_texture, SDL_BLENDMODE_BLEND) < 0) {
        throw std::runtime_error(SDL_GetError());
    }
    if (SDL_QueryTexture(m_texture, nullptr, nullptr, &w, &h) < 0) {
        throw std::runtime_error(SDL_GetError());
    }

    m_cols = w / m_size.x;
    m_rows = h / m_size.y;
}

/**
 * Create a new texture and render some text on it
 */
void Sprite::createFromText(SDL_Renderer* renderer, const std::string& text, const std::string& fontname, int ptsize, int rgba) {
    TTF_Font* font = Game::get().getFont(fontname, ptsize);
    SDL_Surface* surface;
    SDL_Color color = {Uint8(rgba >> 24 & 0xff), Uint8((rgba >> 16) & 0xff), Uint8((rgba >> 8) & 0xff)};

    destroy();

    if ((surface = TTF_RenderText_Blended(font, text.c_str(), color)) == nullptr) {
        throw std::runtime_error(TTF_GetError());
    }
    else {
        if ((m_texture = SDL_CreateTextureFromSurface(renderer, surface)) == nullptr) {
            SDL_FreeSurface(surface);
            throw std::runtime_error(SDL_GetError());
        }
        else {
            m_size = vec2i(surface->w, surface->h);
            m_offset = m_size / 2;
            m_cols = 1;
            m_rows = 1;
            m_start = 0;
            m_count = 1;
            m_must_destroy = true;
        }
        SDL_FreeSurface(surface);
    }
}

void Sprite::createFromVGradient(SDL_Renderer* renderer, int w, int h, int rgba0, int rgba1) {
    SDL_Surface* surface;

    if ((surface = SDL_CreateRGBSurfaceWithFormat(0, w, h, 32, SDL_PIXELFORMAT_RGBA32)) == nullptr) {
        SDL_FreeSurface(surface);
        throw std::runtime_error(SDL_GetError());
    }
    else {
        int ar = (rgba0 >> 24) & 0xff,
            ag = (rgba0 >> 16) & 0xff,
            ab = (rgba0 >> 8)  & 0xff,
            br = (rgba1 >> 24) & 0xff,
            bg = (rgba1 >> 16) & 0xff,
            bb = (rgba1 >> 8)  & 0xff;

        for (int i = 0; i < surface->h; i++) {
            SDL_Color rgb = {
                Uint8(ar + (br - ar) * i / surface->h),
                Uint8(ag + (bg - ag) * i / surface->h),
                Uint8(ab + (bb - ab) * i / surface->h)
            };
            SDL_Rect rect = {0, i, surface->w, i};
            SDL_FillRect(surface, &rect, SDL_MapRGB(surface->format, rgb.r, rgb.g, rgb.b));
        } 

        if ((m_texture = SDL_CreateTextureFromSurface(renderer, surface)) == nullptr) {
            SDL_FreeSurface(surface);
            throw std::runtime_error(SDL_GetError());
        }
        else {
            m_size = vec2i(surface->w, surface->h);
            m_offset = m_size / 2;
            m_cols = 1;
            m_rows = 1;
            m_start = 0;
            m_count = 1;
            m_must_destroy = true;
        }
        SDL_FreeSurface(surface);
    }
}
