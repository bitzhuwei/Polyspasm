/**
 * Copyright (c) 2012 Jesper Öqvist <jesper@llbit.se>
 *
 * This file is part of Polyspasm.
 *
 * Polyspasm is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * Polyspasm is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Polyspasm; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#ifndef CTTF_OCTREE_H
#define CTTF_OCTREE_H

#include <cstdlib>
#include <string>
#include <vector>
#include <stdint.h>

class Octree {
private:
	class OctNode {
	public:
		int			type;
		OctNode**	children;

		OctNode();
		~OctNode();

		size_t load(const std::vector<int>& data, size_t offset);
		size_t bufferSize();
		size_t dumpData(size_t index, uint32_t* data);
	};

	int			m_depth;
	OctNode*	m_root;

	void load(const std::vector<int>& data);
public:
	Octree();

	void load(const std::string& fname);

	size_t bufferSize() { return m_root->bufferSize(); }
	uint32_t* buffer();
	size_t depth() { return m_depth; }
};

#endif