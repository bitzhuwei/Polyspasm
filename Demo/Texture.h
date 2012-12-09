/**
 * Copyright (c) 2009-2011 Jesper Öqvist <jesper@llbit.se>
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
#ifndef CTTF_TEXTURE_H
#define CTTF_TEXTURE_H

#include "Demo.h"

class Texture {
	std::string	fname;
	GLuint		m_texture_id;
	GLenum		m_texture_target;
	GLsizei		m_width;
	GLsizei		m_height;
	GLenum		m_internalfmt;
	GLenum		m_fmt;
	bool		m_mipmap;
	bool		m_interpol;

public:
	Texture() {}
	~Texture() {}

	static SDL_Surface *LoadSurface(const std::string& fn);
	static GLenum SurfaceFormat(const SDL_Surface* surface);
	static int TextureSize(int n);

	void LoadOctree(const std::string& fn);
	void LoadTexture(const std::string& fn);
	void load1DTexture(int width, const int* data);

	void setFormat(GLenum internalformat, GLsizei width, GLsizei height, GLenum format, bool mipmap, bool interpol);

	void setGLTexture(GLenum target, GLuint id) {
		m_texture_target = target;
		m_texture_id = id;
	}
	GLenum glTarget() const {return m_texture_target;}
	GLuint glID() const {return m_texture_id;}

	void bind() const { glBindTexture(m_texture_target, m_texture_id); }
	void release() const { glBindTexture(m_texture_target, 0); }

	// swap textures
	void swap(Texture* other);

	void noWrapping();

	void resize(GLsizei width, GLsizei height);

	GLsizei width() { return m_width; }
	GLsizei height() { return m_height; }
};

#endif