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
#ifndef VEC_H
#define VEC_H

#include <cmath>

template <typename T> struct vec2 {
    T x, y;
    inline vec2<T>() : x(0), y(0) {}
    inline vec2<T>(const T x, const T y) : x(x), y(y) {} 

    inline const bool is0() const {
        return x== 0 && y == 0;
    }

    inline const T length() const {
        return (T)hypot(x, y);
    }

    inline const T normalize() {
        const T len = length();
        if (len == (T)0 || len ==(T)1)
            return len;

        x /= len;
        y /= len;
        return len;
    }

    inline const bool operator==(const vec2<T> &other) const {
        return x == other.x && y == other.y;
    }

    inline const bool operator!=(const vec2<T> &other) const {
        return x != other.x || y != other.y;
    }

    inline const bool operator<(const vec2<T> &other) const {
        if (y != other.y) {
            return y < other.y;
        }
        return x < other.x;
    }

    inline const vec2<T>& operator+=(const vec2<T>& other) {
        x += other.x;
        y += other.y;
        return *this;
    }
    inline const vec2<T>& operator-=(const vec2<T>& other) {
        x -= other.x;
        y -= other.y;
        return *this;
    }
    inline const vec2<T>& operator/=(const T& other) {
        x /= other;
        y /= other;
        return *this;
    }
    inline const vec2<T>& operator%=(const T& other) {
        x %= other;
        y %= other;
        return *this;
    }
    inline const vec2<T>& operator*=(const T& other) {
        x *= other;
        y *= other;
        return *this;
    }

    template <typename T2>
    inline explicit operator vec2<T2>() const {
        return vec2<T2>((T2)x, (T2)y);
    }
    inline const vec2<T> operator-() const {
        return vec2<T>(-x, -y);
    }
    inline const vec2<T> operator+(const vec2<T>& other) const {
        return vec2<T>(x + other.x, y + other.y);
    }
    inline const vec2<T> operator-(const vec2<T>& other) const {
        return vec2<T>(x - other.x, y - other.y);
    }
    inline const vec2<T> operator*(const T& other) const {
        return vec2<T>(x * other, y * other);
    }
    inline const vec2<T> operator/(const T& other) const {
        return vec2<T>(x / other, y / other);
    }
    inline const vec2<T> operator%(const T& other) const {
        return vec2<T>(x % other, y % other);
    }
};

using vec2i = vec2<int>;
using vec2f = vec2<float>;
#endif
