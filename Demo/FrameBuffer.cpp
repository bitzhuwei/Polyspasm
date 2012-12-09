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

#include "FrameBuffer.h"
#include "Texture.h"

static GLenum	attachment_id[] = {
	GL_COLOR_ATTACHMENT0,
	GL_COLOR_ATTACHMENT1,
	GL_COLOR_ATTACHMENT2,
	GL_COLOR_ATTACHMENT3,
	GL_COLOR_ATTACHMENT4,
	GL_COLOR_ATTACHMENT5,
	GL_COLOR_ATTACHMENT6,
	GL_COLOR_ATTACHMENT7,
	GL_COLOR_ATTACHMENT8,
	GL_COLOR_ATTACHMENT9,
	GL_COLOR_ATTACHMENT10,
	GL_COLOR_ATTACHMENT11,
	GL_COLOR_ATTACHMENT12,
	GL_COLOR_ATTACHMENT13,
	GL_COLOR_ATTACHMENT14,
	GL_COLOR_ATTACHMENT15,
};

FrameBuffer::FrameBuffer(Texture* color0, bool depth)
{
	setup(color0, depth);
}

FrameBuffer::FrameBuffer(std::vector<Texture*>& color, bool depth)
{
	setup(color[0], depth);
	for (size_t i = 1; i < color.size(); ++i) {
		addColorAttachment(color[i]);
	}
}

FrameBuffer::FrameBuffer(GLsizei width, GLsizei height, bool depth, bool interpol)
{
	Texture*	texture = new Texture();
	texture->setFormat(GL_RGBA16F, width, height, GL_RGBA, false, interpol);

	setup(texture, depth);
}

void FrameBuffer::setup(Texture* color0, bool depth)
{
	m_width = color0->width();
	m_height = color0->height();

	/* Create render buffer object for depth buffering */
	if (depth) {
		m_depth = new Texture();
		m_depth->setFormat(GL_DEPTH_COMPONENT24, m_width, m_height, GL_DEPTH_COMPONENT, false, false);
	} else {
		m_depth = NULL;
	}

	/* Create and bind new FBO */
	glGenFramebuffers(1, &m_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

	glFramebufferTexture2D(GL_FRAMEBUFFER, attachment_id[m_color.size()], GL_TEXTURE_2D, color0->glID(), 0);

	m_color.push_back(color0);

	if (m_depth != NULL) {
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depth->glID(), 0);
	}

	GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (result != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "Failed to create frame buffer object!" << std::endl;
	}

	useAllAttachments();

	/* Release FBO */
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::resize(GLsizei width, GLsizei height)
{
	m_width = width;
	m_height = height;
	for (size_t i = 0; i < m_color.size(); ++i) {
		m_color[i]->resize(m_width, m_height);
	}

	if (m_depth != NULL) {
		m_depth->resize(m_width, m_height);
	}

	// rebind the textures
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	for (size_t i = 0; i < m_color.size(); ++i) {
		glFramebufferTexture2D(GL_FRAMEBUFFER, attachment_id[i], GL_TEXTURE_2D, m_color[i]->glID(), 0);
	}
	if (m_depth != NULL) {
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depth->glID(), 0);
	}
	// check status
	GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (result != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "Failed to create frame buffer object!" << std::endl;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::useAllAttachments()
{
	glDrawBuffers(m_color.size(), attachment_id);
}

void FrameBuffer::addColorAttachment(GLenum internalfmt, GLenum format, bool mipmap, bool interpol)
{
	Texture*	texture = new Texture();

	texture->setFormat(internalfmt, m_width, m_height, format, mipmap, interpol);

	addColorAttachment(texture);

}

void FrameBuffer::addColorAttachment(Texture* tex)
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, attachment_id[m_color.size()], tex->glTarget(), tex->glID(), 0);
	GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	if (result != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "Failed to attach extra color buffer!" << std::endl;
		delete tex;
		return;
	}

	m_color.push_back(tex);

	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	useAllAttachments();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::swapColorAttachment(FrameBuffer* other, int index)
{
	m_color[index]->swap(other->m_color[index]);

	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, attachment_id[index], m_color[index]->glTarget(), m_color[index]->glID(), 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, other->m_fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, attachment_id[index], other->m_color[index]->glTarget(), other->m_color[index]->glID(), 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	checkGLErrors();
}

void FrameBuffer::bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
}

void FrameBuffer::release()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::clear(GLbitfield bits)
{
	bind();
	glClear(bits);
	release();
}

void FrameBuffer::bindDraw()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
}

void FrameBuffer::releaseDraw()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void FrameBuffer::bindRead()
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);
}

void FrameBuffer::releaseRead()
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
}


Texture* FrameBuffer::color(int i)
{
	return m_color[i];
}
