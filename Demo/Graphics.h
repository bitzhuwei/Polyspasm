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
#ifndef CTTF_GRAPHICS_H
#define CTTF_GRAPHICS_H

#include <vector>
#include <map>
#include <iostream>

#include "Shader.h"
#include "FrameBuffer.h"
#include "Texture.h"
#include "TF3D.h"

class Graphics
{
private:
	SDL_Surface*						m_screen;
	int									m_width;
	int									m_height;
	std::map<std::string,std::vector<std::string> >	m_shader_config;
	std::map<std::string,Shader*>		m_shaders;
	std::map<std::string,FrameBuffer*>	m_framebuffers;
	std::map<std::string,Texture*>		m_textures;
	Shader*								m_shader;
	TF3D*								m_font;
	
	void setupVideo(bool fullscreen);

	/* build the shader config map */
	void setupShaders();

	/* Initialize Framebuffer Objects */
	void setupFBO();

	void resizeFBOs();
public:
	Graphics(int width, int height)
		: m_width(width), m_height(height), m_shader(NULL) { }

	float aspect() { return m_width / (float) m_height; }

	void setup(bool fullscreen);
	void shutdown();

	void resize(int width, int height);

	void swap_buffers();

	void bindShader(const std::string& name);
	void releaseShader();

	Shader* shader();

	int width();

	int height();

	Shader* shader(const std::string& name);

	FrameBuffer* framebuffer(const std::string& name);

	void add_framebuffer(const std::string& name, bool depth_buf, bool interpol);

	void loadVertexShader(const std::string& name, const std::string& fname);

	void loadFragmentShader(const std::string& name, const std::string& fname);

	void loadShader(const std::string& name);

	void reloadShaders();

	void loadTexture(const std::string& name, const std::string& fname);
	void loadOctree(const std::string& name, const std::string& fname);
	void load1DTexture(const std::string& name, int width, int* data);

	Texture* texture(const std::string& name);

	void setFont(TF3D* font) { m_font = font; }
	TF3D* font() { return m_font; }
};

#endif
