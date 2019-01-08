/* Winter-Strike Game
 * Copyright (C) 2019 Kumok Boris
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
#define _USE_MATH_DEFINES
#include <cmath>
#include <limits>
#include <stdexcept>
#include <SDL.h>
#include "object.h"
#include "world.h"

int Object::max_object_id = 0;

Object::Object(const std::string& classname) :
    m_classname(classname),
    m_object_id(max_object_id++),
    m_world(nullptr),
    m_dir(1, 0),
    m_z(1),
    m_facing(0),
    m_alive(true),
    m_solid(true),
    m_collider(true) {

    // std::cout << "Create " << m_classname << " (object #" << m_object_id << ")" << std::endl;
    SDL_LogDebug(SDL_LOG_CATEGORY_TEST, "Create %s  (object #%d)", m_classname.c_str(), m_object_id);
}

Object::~Object() {
    // std::cout << "Delete " << m_classname << " (object #" << m_object_id << ")" << std::endl;
    SDL_LogDebug(SDL_LOG_CATEGORY_TEST, "Delete %s  (object #%d)", m_classname.c_str(), m_object_id);
}

void Object::render(SDL_Renderer* renderer, const vec2i& screenCoords) {
}

void Object::update(float dt) {
}

/**
 * Calculate facing direction (as in sprites) from vector
 */
void Object::setDirection(const vec2f& dir) {
    float a = dir.x != 0 ? (dir.y / dir.x) : (dir.y > 0 ? 999 : -999);

    if (a > 2.4142135623731) { // tn(67.5)
        m_facing = dir.x < 0 ? 4 : 0;
    }
    else if (a > 0.4142135623731) { // tn(22.5)
        m_facing = dir.x < 0 ? 5 : 1;
    }
    else if (a > -0.4142135623731) { // tn(-22.5)
        m_facing = dir.x < 0 ? 6 : 2;
    }
    else if (a > -2.4142135623731) { // tn(-67.5)
        m_facing = dir.x < 0 ? 7 : 3;
    }
    else {
        m_facing = dir.x < 0 ? 0 : 4;
    }

    m_dir = dir;
    m_dir.normalize();
}

void Object::onCollision(Object* other) {
}

void Object::onHit(Object* other, int hp) {
}

/**
 * Build world path using a-star algorithm with diagonal heuristic
 */
void Object::buildPathTo(const vec2f& pos) {
    static const vec2i steps[8] = {{-1, -1}, {0, -1}, {+1, -1}, {-1, 0}, {+1, 0}, {-1, +1}, {0, +1}, {+1, +1}};
    static const float weights[8] = {M_SQRT2, 1, M_SQRT2, 1, 1, M_SQRT2, 1, M_SQRT2};

    vec2i start(round(m_pos.x), round(m_pos.y));
    vec2i goal(round(pos.x), round(pos.y));

    m_nodes.clear();
    m_queue = Node::PrioQ();

    Node* cur = &m_nodes[start];
	cur->idx = start;
	cur->parent = nullptr;
	cur->actual = 0;
	cur->heuristic = 99999;

	m_queue.push(cur);

	while (!m_queue.empty() && m_queue.size() < 50) {
		cur = m_queue.top();
		if (cur->idx == goal) {
			break;
		}
		m_queue.pop();

		for (int i = 0; i < 8; ++i) {
            vec2i idx = cur->idx + steps[i];

            if (m_world->isPassable(idx)) {
                Node* next = &m_nodes[idx];
                float actual = cur->actual + weights[i];

                if (actual < next->actual) {
                    next->idx = idx;
                    next->parent = cur;
                    next->actual = actual;
                    next->heuristic = std::max(std::abs(idx.x - goal.x), std::abs(idx.y - goal.y));

                    m_queue.push(next);
                }
            }
		}
	}

    vec2f last;

    // if we couldn't reach the goal, build path to the closest node instead
    if (cur->idx != goal) {
        for (auto& it : m_nodes) {
            if (it.second.heuristic < cur->heuristic) {
                cur = &it.second;
            }
        }
        last = (vec2f)cur->idx;
    }
    else {
        // precise destination
        last = pos;
    }

    m_path.clear();
    for (cur = cur->parent; cur; cur = cur->parent) {
        m_path.push_front((vec2f)(cur->idx));
    }

    // remove initial node
    if (!m_path.empty()) {
        m_path.pop_front();
    }

    // add final destination
    m_path.push_back(last);
}
