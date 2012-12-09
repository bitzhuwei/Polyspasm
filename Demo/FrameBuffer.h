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
#ifndef CTTF_FRAMEBUFFER_H
#define CTTF_FRAMEBUFFER_H

#include <GL/gl.h>

#include <vector>

class Texture;

class FrameBuffer {
private:

	GLuint					m_fbo;
	std::vector<Texture*>	m_color;
	Texture*				m_depth;
	GLsizei					m_width;
	GLsizei					m_height;

	void setup(Texture* color0, bool depth);
public:
	FrameBuffer(std::vector<Texture*>& color, bool depth);
	FrameBuffer(Texture* color0, bool depth);
	FrameBuffer(GLint width, GLint height, bool depth_buf, bool interpol);

	void addColorAttachment(Texture* tex);
	void addColorAttachment(GLenum internalfmt, GLenum format, bool mipmap, bool interpol);

	void swapColorAttachment(FrameBuffer* other, int index);

	void useAllAttachments();

	void bind();
	void release();

	void clear(GLbitfield bits);

	void bindDraw();
	void releaseDraw();

	void bindRead();
	void releaseRead();

	Texture* color(int i);
	Texture* depth() { return m_depth; }

	void resize(GLsizei width, GLsizei height);

	GLuint glID() { return m_fbo; }
	GLsizei width() { return m_width; }
	GLsizei height() { return m_height; }
};

#endif
