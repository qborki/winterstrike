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

    inline const T squareLength() const {
        return (T)(x * x + y * y);
    }

    inline const T length() const {
        return (T)hypot(x, y);
    }

    inline const void normalize() {
        const T len = length();
        if (len != (T)0 && len !=(T)1) {
            x /= len;
            y /= len;
        }
    }

    inline const bool operator==(const vec2<T> &other) const {
        return x == other.x && y == other.y;
    }

    inline const bool operator!=(const vec2<T> &other) const {
        return x != other.x || y != other.y;
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

    template <typename T2 = T>
    inline const vec2<T2> round() const {
        return vec2<T2>((T2)std::round(x), (T2)std::round(y));
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

// allow vec2 to be used as key in unordered_map
namespace std {
	template<typename T>
	struct hash<vec2<T>> {
		inline void hash_combine(size_t &seed, size_t hash) const {
			hash += 0x9e3779b9 + (seed << 6) + (seed >> 2);
			seed ^= hash;
		}

		inline size_t operator()(vec2<T> const& v, size_t seed = 0) const {
			hash<T> hasher;
			hash_combine(seed, hasher(v.x));
			hash_combine(seed, hasher(v.y));
			return seed;
		}
	};
}

using vec2i = vec2<int>;
using vec2f = vec2<float>;
#endif
