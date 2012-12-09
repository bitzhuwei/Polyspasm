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
#include "Texture.h"

#include <SDL/SDL_image.h>

#include <iostream>
#include <vector>
#include <fstream>

#include "Octree.h"

SDL_Surface *Texture::LoadSurface(const std::string& fn)
{
	size_t		size;
	SDL_Surface	*surface;
	SDL_RWops	*rw;

	std::ifstream file(fn.c_str(), std::ios::in | std::ios::binary);

	if (!file.is_open()) {
		throw std::string("Could not open file: ") + fn;
	}

	file.seekg(0, std::ios::end);
	size = (size_t)file.tellg();
	file.seekg(0, std::ios::beg);

	std::vector<char> data(size, 0);
	file.read(&data[0], size);
	file.close();
	
	rw = SDL_RWFromConstMem((void*)&data[0], (int)size);
	surface = IMG_Load_RW(rw, 1);

	if (surface == NULL) {
		std::cout << SDL_GetError();
		throw std::string("Unable to load surface data: ") + SDL_GetError();
	}

	return surface;
}

GLenum Texture::SurfaceFormat(const SDL_Surface* surface)
{
	if (surface->format->BytesPerPixel == 4) {
		if (surface->format->Rmask == 0x000000ff) {
			return GL_RGBA;
		} else {
			return GL_BGRA;
		}
	} else if (surface->format->BytesPerPixel == 3) {
		if (surface->format->Rmask == 0x000000ff) {
			return GL_RGB;
		} else {
			return GL_BGR;
		}
	} else {
		throw std::string("Unsupported color format.");
	}
}

int Texture::TextureSize(int n)
{
	// Round up to next highest power of two.
	--n;
	n |= n >> 1;
	n |= n >> 2;
	n |= n >> 4;
	n |= n >> 8;
	n |= n >> 16;
	++n;
	return n;
}

void Texture::LoadOctree(const std::string& fname)
{
	Octree			octree;
	size_t			size;
	size_t			width;
	uint32_t*		buffer;
	uint32_t*		padded;
	size_t			sqsize;

	octree.load(fname);
	size = octree.bufferSize();
	buffer = octree.buffer();

	std::cout << "Octree depth: " << octree.depth() << std::endl;

	sqsize = (size_t) ceil(sqrtf((float)size));
	width = TextureSize(sqsize);

	// pad the buffer with zeros
	padded = new uint32_t[width*width];
	memcpy(padded, buffer, sizeof(uint32_t) * size);

	// TODO create 2D octree buffer

	m_texture_target = GL_TEXTURE_2D;
	glGenTextures(1, &m_texture_id);
	glBindTexture(m_texture_target, m_texture_id);
	glTexParameterf(m_texture_target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(m_texture_target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(m_texture_target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(m_texture_target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(m_texture_target, GL_GENERATE_MIPMAP, GL_FALSE);
	glTexImage2D(m_texture_target, 0, GL_R32UI, width, width, 0, GL_RED_INTEGER,
		GL_UNSIGNED_INT, padded);
	checkGLErrors();
	glBindTexture(m_texture_target, 0);

	delete buffer;
	delete padded;
}

void Texture::load1DTexture(int width, const int* data)
{
	size_t			pad_width;
	uint32_t*		padded;

	pad_width = TextureSize(width);

	padded = new uint32_t[pad_width];
	memset(padded, 0, sizeof(uint32_t)*pad_width);
	memcpy(padded, data, sizeof(int)*width);

	m_texture_target = GL_TEXTURE_1D;
	glGenTextures(1, &m_texture_id);
	glBindTexture(m_texture_target, m_texture_id);
	glTexParameterf(m_texture_target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(m_texture_target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(m_texture_target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(m_texture_target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(m_texture_target, GL_GENERATE_MIPMAP, GL_FALSE);
	glTexImage1D(m_texture_target, 0, GL_RGBA8UI, pad_width, 0, GL_RGBA_INTEGER,
		GL_UNSIGNED_BYTE, padded);
	checkGLErrors();
	glBindTexture(m_texture_target, 0);

	delete padded;
}

void Texture::LoadTexture(const std::string& fn)
{
	SDL_Surface	*surface;
	char		*data;
	GLenum		texture_format;

	surface = LoadSurface(fn);
	texture_format = SurfaceFormat(surface);

	m_texture_target = GL_TEXTURE_2D;
	glGenTextures(1, &m_texture_id);
	glBindTexture(m_texture_target, m_texture_id);
	glTexParameteri(m_texture_target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(m_texture_target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	size_t	surface_w = TextureSize(surface->w);
	size_t	surface_h = TextureSize(surface->h);

	data = new char[surface_w*surface_h * surface->format->BytesPerPixel];
	memset(data, 0, surface_w*surface_h * surface->format->BytesPerPixel);
	for (int y = 0; y < surface->h; ++y) {
		memcpy(
			&data[y*surface_w*surface->format->BytesPerPixel],
			&((char *)surface->pixels)[y*surface->pitch],
			surface->pitch);
	}

	glTexImage2D(m_texture_target, 0, texture_format, surface_w,
		surface_h, 0, texture_format, GL_UNSIGNED_BYTE, data);

	checkGLErrors();

	glBindTexture(GL_TEXTURE_2D, 0);

	delete[] data;

	SDL_FreeSurface(surface);
}

void Texture::setFormat(GLenum internalfmt, GLsizei width, GLsizei height, GLenum format, bool mipmap, bool interpol)
{
	m_width = width;
	m_height = height;
	m_internalfmt = internalfmt;
	m_fmt = format;
	m_mipmap = mipmap;
	m_interpol = interpol;
	m_texture_target = GL_TEXTURE_2D;

	glGenTextures(1, &m_texture_id);
	glBindTexture(m_texture_target, m_texture_id);
	glTexParameterf(m_texture_target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(m_texture_target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	if (mipmap) {
		glTexParameterf(m_texture_target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(m_texture_target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(m_texture_target, GL_GENERATE_MIPMAP, GL_TRUE);
	} else {
		if (interpol) {
			glTexParameterf(m_texture_target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameterf(m_texture_target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		} else {
			glTexParameterf(m_texture_target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameterf(m_texture_target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		}
		glTexParameteri(m_texture_target, GL_GENERATE_MIPMAP, GL_FALSE);
	}
	glTexImage2D(m_texture_target, 0, internalfmt, width, height, 0,
		format, GL_UNSIGNED_BYTE, 0);
	glBindTexture(m_texture_target, 0);

	checkGLErrors();
}

void Texture::resize(GLsizei width, GLsizei height)
{
	glDeleteTextures(1, &m_texture_id);
	setFormat(m_internalfmt, width, height, m_fmt, m_mipmap, m_interpol);
}

void Texture::swap(Texture* other)
{
	GLuint	tmp_id = other->m_texture_id;
	GLenum	tmp_target = other->m_texture_target;
	other->m_texture_id = m_texture_id;
	other->m_texture_target = m_texture_target;
	m_texture_id = tmp_id;
	m_texture_target = tmp_target;
}

void Texture::noWrapping()
{
	glBindTexture(m_texture_target, m_texture_id);
	glTexParameterf(m_texture_target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(m_texture_target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(m_texture_target, 0);
}
