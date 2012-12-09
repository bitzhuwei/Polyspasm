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
#ifndef CTTF_TF3D_H
#define CTTF_TF3D_H

#include <string>
#include <map>

#include "GL/gl.h"

#include "Shader.h"

typedef struct font font_t;
typedef struct edge_list edge_list_t;
typedef struct shape shape_t;
class Graphics;

class TF3D {
private:
	font_t*						font;
	std::map<wchar_t,GLuint*>	m_vbo;
	std::map<wchar_t,size_t>	nindices;
public:
	TF3D(std::string fname);

	void prepare_char(wchar_t chr);
	void prepare_string(std::string str);

	void render(Graphics& gfx, const std::string& str);

	double width(std::string str);

	GLuint* genBuffers(wchar_t wc, edge_list_t* edge_list);

	void reloadGL();
};

#endif