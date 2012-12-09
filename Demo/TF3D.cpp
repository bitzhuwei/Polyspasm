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
#include "Demo.h"

#include "TF3D.h"
#include "Shader.h"
#include "Graphics.h"

#define INTERPOLATION_LEVEL (3)

#define NUM_VBO			(3)
#define VBO_POSITION	(0)
#define VBO_NORMAL		(1)
#define VBO_INDEX		(2)

TF3D::TF3D(std::string fname)
{
	font = load_font(fname.c_str(), INTERPOLATION_LEVEL);

	if (font == NULL) {
		throw std::string("Failed to load font!");
	}
}

void TF3D::prepare_char(wchar_t chr)
{
	if (!m_vbo[chr]) {
		font_prepare_chr(font, chr, 1);
		free_shape(&font->cshape[chr]);
		m_vbo[chr] = genBuffers(chr, font->cedges[chr]);
	}
}

void TF3D::prepare_string(std::string str)
{
	for (std::string::const_iterator iter = str.begin();
			iter != str.end(); ++iter) {
		prepare_char(*iter);
	}
}

/* reload OpenGL resources */
void TF3D::reloadGL()
{
	std::map<wchar_t,GLuint*>::iterator iter;
	for (iter = m_vbo.begin(); iter != m_vbo.end(); ++iter) {
		wchar_t	chr = iter->first;
		// VBOs were already freed when the GL context was destroyed!
		//glDeleteBuffers(NUM_VBO, iter->second);
		delete iter->second;
		iter->second = NULL;
		iter->second = genBuffers(chr, font->cedges[chr]);
	}
}

GLuint* TF3D::genBuffers(wchar_t wc, edge_list_t* edge_list)
{
	GLuint*					vbo = new GLuint[NUM_VBO];
	size_t					nvert = edge_list->nvert;
	std::vector<GLuint>		indices;
	std::vector<GLfloat>	vertices;
	std::vector<GLfloat>	normals;
	list_t*					p;
	size_t					voffset = 0;

	indices.reserve(nvert);
	vertices.reserve(nvert*3);
	normals.reserve(nvert*3);

	for (size_t i = 0; i < nvert; ++i) {
		vertices.push_back(edge_list->vertices[i]->vec.x);
		vertices.push_back(edge_list->vertices[i]->vec.y);
		vertices.push_back(0);
		normals.push_back(0);
		normals.push_back(0);
		normals.push_back(1);
	}

	p = edge_list->faces;
	if (p) do {
		face_t*	face = (face_t*) p->data;
		edge_t*	edge = face->outer_component;
		if (face->is_inside && edge) {
			edge_t*	e = edge;
			do {
				indices.push_back(e->origin->id);
				e = e->succ;
			} while (e != edge);
		}
		p = p->succ;
	} while (p != edge_list->faces);

	voffset = nvert;
	for (size_t i = 0; i < nvert; ++i) {
		vertices.push_back(edge_list->vertices[i]->vec.x);
		vertices.push_back(edge_list->vertices[i]->vec.y);
		vertices.push_back(-0.15f);
		normals.push_back(0);
		normals.push_back(0);
		normals.push_back(-1);
	}

	p = edge_list->faces;
	if (p) do {
		face_t*	face = (face_t*) p->data;
		edge_t*	edge = face->outer_component;
		if (face->is_inside && edge) {
			edge_t*	e = edge;
			do {
				indices.push_back(e->origin->id + voffset);
				e = e->pred;
			} while (e != edge);
		}
		p = p->succ;
	} while (p != edge_list->faces);

	voffset = 2*nvert;

	/* add side triangles */
	p = edge_list->faces;
	if (p) do {
		face_t*	face = (face_t*) p->data;
		edge_t* edge = face->outer_component;
		if (!face->is_inside && edge) {
			edge_t*	e = edge;
			do {
				vector_t	v1 = e->origin->vec;
				vector_t	v2 = e->succ->origin->vec;
				GLfloat		d1[3];
				GLfloat		d2[3];
				GLfloat		n[3];
				GLfloat		len;

				d1[0] = v1.x-v1.x;
				d1[1] = v1.y-v1.y;
				d1[2] = 0.15f;
				d2[0] = v2.x-v1.x;
				d2[1] = v2.y-v1.y;
				d2[2] = 0.15f;
				n[0] = d1[1]*d2[2] - d1[2]*d2[1];
				n[1] = d1[2]*d2[0] - d1[0]*d2[2];
				n[2] = d1[0]*d2[1] - d1[1]*d2[0];
				len = (GLfloat) (1.0/sqrt(n[0]*n[0] + n[1]*n[1] + n[2]*n[2]));
				n[0] *= len;
				n[1] *= len;
				n[2] *= len;

				vertices.push_back(v1.x);
				vertices.push_back(v1.y);
				vertices.push_back(-0.15f);
				normals.push_back(n[0]);
				normals.push_back(n[1]);
				normals.push_back(n[2]);
				indices.push_back(voffset++);

				vertices.push_back(v1.x);
				vertices.push_back(v1.y);
				vertices.push_back(0);
				normals.push_back(n[0]);
				normals.push_back(n[1]);
				normals.push_back(n[2]);
				indices.push_back(voffset++);

				vertices.push_back(v2.x);
				vertices.push_back(v2.y);
				vertices.push_back(0);
				normals.push_back(n[0]);
				normals.push_back(n[1]);
				normals.push_back(n[2]);
				indices.push_back(voffset++);

				d1[0] = v1.x-v2.x;
				d1[1] = v1.y-v2.y;
				d1[2] = 0;
				d2[0] = v2.x-v2.x;
				d2[1] = v2.y-v2.y;
				d2[2] = 0.15f;
				n[0] = d1[1]*d2[2] - d1[2]*d2[1];
				n[1] = d1[2]*d2[0] - d1[0]*d2[2];
				n[2] = d1[0]*d2[1] - d1[1]*d2[0];
				len = (GLfloat) (1.0/sqrt(n[0]*n[0] + n[1]*n[1] + n[2]*n[2]));
				n[0] *= len;
				n[1] *= len;
				n[2] *= len;

				vertices.push_back(v2.x);
				vertices.push_back(v2.y);
				vertices.push_back(0);
				normals.push_back(n[0]);
				normals.push_back(n[1]);
				normals.push_back(n[2]);
				indices.push_back(voffset++);

				vertices.push_back(v2.x);
				vertices.push_back(v2.y);
				vertices.push_back(-0.15f);
				normals.push_back(n[0]);
				normals.push_back(n[1]);
				normals.push_back(n[2]);
				indices.push_back(voffset++);

				vertices.push_back(v1.x);
				vertices.push_back(v1.y);
				vertices.push_back(-0.15f);
				normals.push_back(n[0]);
				normals.push_back(n[1]);
				normals.push_back(n[2]);
				indices.push_back(voffset++);

				e = e->succ;
			} while (e != edge);
		}
		list_t*	q = face->inner_components;
		if (q) do {
			edge = (edge_t*) q->data;
			if (!face->is_inside && edge) {
				edge_t*	e = edge;
				do {
					vector_t	v1 = e->origin->vec;
				vector_t	v2 = e->succ->origin->vec;
				GLfloat		d1[3];
				GLfloat		d2[3];
				GLfloat		n[3];
				GLfloat		len;

				d1[0] = v1.x-v1.x;
				d1[1] = v1.y-v1.y;
				d1[2] = 0.15f;
				d2[0] = v2.x-v1.x;
				d2[1] = v2.y-v1.y;
				d2[2] = 0.15f;
				n[0] = d1[1]*d2[2] - d1[2]*d2[1];
				n[1] = d1[2]*d2[0] - d1[0]*d2[2];
				n[2] = d1[0]*d2[1] - d1[1]*d2[0];
				len = (GLfloat) (1.0/sqrt(n[0]*n[0] + n[1]*n[1] + n[2]*n[2]));
				n[0] *= len;
				n[1] *= len;
				n[2] *= len;

				vertices.push_back(v1.x);
				vertices.push_back(v1.y);
				vertices.push_back(-0.15f);
				normals.push_back(n[0]);
				normals.push_back(n[1]);
				normals.push_back(n[2]);
				indices.push_back(voffset++);

				vertices.push_back(v1.x);
				vertices.push_back(v1.y);
				vertices.push_back(0);
				normals.push_back(n[0]);
				normals.push_back(n[1]);
				normals.push_back(n[2]);
				indices.push_back(voffset++);

				vertices.push_back(v2.x);
				vertices.push_back(v2.y);
				vertices.push_back(0);
				normals.push_back(n[0]);
				normals.push_back(n[1]);
				normals.push_back(n[2]);
				indices.push_back(voffset++);

				d1[0] = v1.x-v2.x;
				d1[1] = v1.y-v2.y;
				d1[2] = 0;
				d2[0] = v2.x-v2.x;
				d2[1] = v2.y-v2.y;
				d2[2] = 0.15f;
				n[0] = d1[1]*d2[2] - d1[2]*d2[1];
				n[1] = d1[2]*d2[0] - d1[0]*d2[2];
				n[2] = d1[0]*d2[1] - d1[1]*d2[0];
				len = 1/sqrt(n[0]*n[0] + n[1]*n[1] + n[2]*n[2]);
				n[0] *= len;
				n[1] *= len;
				n[2] *= len;

				vertices.push_back(v2.x);
				vertices.push_back(v2.y);
				vertices.push_back(0);
				normals.push_back(n[0]);
				normals.push_back(n[1]);
				normals.push_back(n[2]);
				indices.push_back(voffset++);

				vertices.push_back(v2.x);
				vertices.push_back(v2.y);
				vertices.push_back(-0.15f);
				normals.push_back(n[0]);
				normals.push_back(n[1]);
				normals.push_back(n[2]);
				indices.push_back(voffset++);

				vertices.push_back(v1.x);
				vertices.push_back(v1.y);
				vertices.push_back(-0.15f);
				normals.push_back(n[0]);
				normals.push_back(n[1]);
				normals.push_back(n[2]);
				indices.push_back(voffset++);
					e = e->succ;
				} while (e != edge);
			}
			q = q->succ;
		} while (q != face->inner_components);
		p = p->succ;
	} while (p != edge_list->faces);

	if (vertices.size() > 0 && indices.size() > 0) {

		glGenBuffers(NUM_VBO, vbo);

		glBindBuffer(GL_ARRAY_BUFFER, vbo[VBO_POSITION]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0])*vertices.size(), &vertices[0],
			GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, vbo[VBO_NORMAL]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(normals[0])*normals.size(), &normals[0],
			GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[VBO_INDEX]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0])*indices.size(), &indices[0],
			GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		nindices[wc] = indices.size();
	} else {
		nindices[wc] = 0;
	}

	return vbo;
}

double TF3D::width(std::string str)
{
	return line_width(font, str.c_str());
}

void TF3D::render(Graphics& gfx, const std::string& str)
{
	const char* s;
	assert(font != NULL);

	s = str.c_str();
	while (*s != '\0') {
		wchar_t 		wc;
		int				n;

		n = mbtowc(&wc, s, MB_CUR_MAX);
		if (n == -1) break;
		else s += n;

		if (n == L' ')
			continue;

		prepare_char(wc);
		
		if (!m_vbo[wc]) continue;

		GLuint*	vbo = m_vbo[wc];

		GLint	loc;

		loc = gfx.shader() ? gfx.shader()->getAttribLocation("position") : 0;
		if (loc != -1) {
			glBindBuffer(GL_ARRAY_BUFFER, vbo[VBO_POSITION]);
			glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 0, NULL);
			glEnableVertexAttribArray(loc);
		}
		if (loc != 0) {
			loc = 0;
			glBindBuffer(GL_ARRAY_BUFFER, vbo[VBO_POSITION]);
			glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 0, NULL);
			glEnableVertexAttribArray(loc);
		}

		loc = gfx.shader() ? gfx.shader()->getAttribLocation("normal") : 1;
		if (loc != -1) {
			glBindBuffer(GL_ARRAY_BUFFER, vbo[VBO_NORMAL]);
			glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 0, NULL);
			glEnableVertexAttribArray(loc);
		}

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[VBO_INDEX]);

		glDrawElements(GL_TRIANGLES, nindices[wc], GL_UNSIGNED_INT, NULL);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		// offset to next character
		glTranslatef(ttf_char_width(font->ttf, wc), 0, 0);
	}
}
