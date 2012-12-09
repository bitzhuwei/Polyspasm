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

#include <iostream>

#include "Graphics.h"

#define SKY_SIZE (512)
#define FLUID_GRID_W (1024)

void Graphics::setup(bool fullscreen)
{
	setupVideo(fullscreen);

	GLenum err = glewInit();

	glViewport(0, 0, m_screen->w, m_screen->h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, 1, 1, 7);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glShadeModel(GL_FLAT);
	glCullFace(GL_BACK);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glClearColor(0, 0, 0, 0);
	glEnable(GL_TEXTURE_2D);

	GLint	max;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &max);
	std::cout << "Maximum number of vertex attribs: " << max << std::endl;
	std::cout << "Maximum texture size: " << GL_MAX_TEXTURE_SIZE << std::endl;

	setupShaders();
	setupFBO();
}

void Graphics::setupShaders()
{
	m_shader_config["displace"].push_back("../res/shaders/displace.vs");
	m_shader_config["displace"].push_back("../res/shaders/colorize.fs");

	m_shader_config["colorize"].push_back("../res/shaders/colorize.fs");

	m_shader_config["blur_h"].push_back("../res/shaders/notransform.vs");
	m_shader_config["blur_h"].push_back("../res/shaders/blur_h.fs");

	m_shader_config["blur_v"].push_back("../res/shaders/notransform.vs");
	m_shader_config["blur_v"].push_back("../res/shaders/blur_v.fs");

	m_shader_config["blur"].push_back("../res/shaders/blur.fs");

	m_shader_config["default"].push_back("../res/shaders/default.vs");
	m_shader_config["default"].push_back("../res/shaders/default.fs");

	m_shader_config["drawfb"].push_back("../res/shaders/notransform.vs");
	m_shader_config["drawfb"].push_back("../res/shaders/drawfb.fs");

	m_shader_config["fire"].push_back("../res/shaders/fire.fs");

	m_shader_config["blob"].push_back("../res/shaders/blob.fs");

	m_shader_config["simplex"].push_back("../res/shaders/simplex.fs");

	m_shader_config["scanlines"].push_back("../res/shaders/scanlines.vs");
	m_shader_config["scanlines"].push_back("../res/shaders/scanlines.fs");

	m_shader_config["rng"].push_back("../res/shaders/rng.vs");
	m_shader_config["rng"].push_back("../res/shaders/rng.fs");

	m_shader_config["fade"].push_back("../res/shaders/displace.vs");
	m_shader_config["fade"].push_back("../res/shaders/fade.fs");

	m_shader_config["flat"].push_back("../res/shaders/flat.vs");
	m_shader_config["flat"].push_back("../res/shaders/flat.fs");

	m_shader_config["normals"].push_back("../res/shaders/normals.vs");
	m_shader_config["normals"].push_back("../res/shaders/normals.fs");

	m_shader_config["pt"].push_back("../res/shaders/pt.vs");
	m_shader_config["pt"].push_back("../res/shaders/pt.fs");

	m_shader_config["rt"].push_back("../res/shaders/pt.vs");
	m_shader_config["rt"].push_back("../res/shaders/rt.fs");

	m_shader_config["edges"].push_back("../res/shaders/edges.fs");
	m_shader_config["fireedges"].push_back("../res/shaders/fireedges.fs");

	m_shader_config["mix"].push_back("../res/shaders/mix.fs");

	m_shader_config["advect"].push_back("../res/shaders/advect.fs");
	m_shader_config["buoyancy"].push_back("../res/shaders/buoyancy.fs");
	m_shader_config["jacobian"].push_back("../res/shaders/jacobian.fs");
	m_shader_config["divergence"].push_back("../res/shaders/divergence.fs");
	m_shader_config["gradient"].push_back("../res/shaders/gradient.fs");

	m_shader_config["water"].push_back("../res/shaders/water.vs");
	m_shader_config["water"].push_back("../res/shaders/water.fs");

	m_shader_config["depth"].push_back("../res/shaders/notransform.vs");
	m_shader_config["depth"].push_back("../res/shaders/depth.fs");
}

void Graphics::loadShader(const std::string& name)
{
	if (!m_shaders[name]) {

		if (m_shader_config.find(name) == m_shader_config.end()) {
			throw std::string("No config for shader named ") + name;
		}

		std::vector<std::string>::const_iterator iter = m_shader_config[name].begin();
		for ( ; iter != m_shader_config[name].end(); ++iter) {
			if (0 == iter->compare(iter->length() - 3, 3, ".vs")) {
				loadVertexShader(name, *iter);
			} else {
				loadFragmentShader(name, *iter);
			}
		}
		shader(name)->link();

	}
}

void Graphics::setupFBO()
{
	add_framebuffer("fb0", true, false);
	add_framebuffer("fb1", true, false);

	add_framebuffer("ptbuf0", false, false);
	m_framebuffers["ptbuf0"]->addColorAttachment(GL_R8UI, GL_RED_INTEGER, false, false);

	add_framebuffer("ptbuf1", false, false);
	m_framebuffers["ptbuf1"]->addColorAttachment(GL_R8UI, GL_RED_INTEGER, false, false);

	m_framebuffers["sky_ptbuf"] = new FrameBuffer(SKY_SIZE, SKY_SIZE, false, true);
	m_framebuffers["sky_ptbuf"]->addColorAttachment(GL_R8UI, GL_RED_INTEGER, false, false);

	m_framebuffers["sky_front"] = new FrameBuffer(SKY_SIZE, SKY_SIZE, false, true);
	m_framebuffers["sky_front"]->addColorAttachment(GL_R8UI, GL_RED_INTEGER, false, false);

	m_framebuffers["sky_back"] = new FrameBuffer(SKY_SIZE, SKY_SIZE, false, true);
	m_framebuffers["sky_back"]->addColorAttachment(GL_R8UI, GL_RED_INTEGER, false, false);

	m_framebuffers["sky_left"] = new FrameBuffer(SKY_SIZE, SKY_SIZE, false, true);
	m_framebuffers["sky_left"]->addColorAttachment(GL_R8UI, GL_RED_INTEGER, false, false);

	m_framebuffers["sky_right"] = new FrameBuffer(SKY_SIZE, SKY_SIZE, false, true);
	m_framebuffers["sky_right"]->addColorAttachment(GL_R8UI, GL_RED_INTEGER, false, false);

	m_framebuffers["sky_up"] = new FrameBuffer(SKY_SIZE, SKY_SIZE, false, true);
	m_framebuffers["sky_up"]->addColorAttachment(GL_R8UI, GL_RED_INTEGER, false, false);

	m_framebuffers["sky_down"] = new FrameBuffer(SKY_SIZE, SKY_SIZE, false, true);
	m_framebuffers["sky_down"]->addColorAttachment(GL_R8UI, GL_RED_INTEGER, false, false);

	Texture*	tex;
	tex = new Texture();
	tex->setFormat(GL_RG16F, FLUID_GRID_W, FLUID_GRID_W, GL_RG, false, true);
	m_framebuffers["velocity0"] = new FrameBuffer(tex, false);
	tex = new Texture();
	tex->setFormat(GL_RG16F, FLUID_GRID_W, FLUID_GRID_W, GL_RG, false, true);
	m_framebuffers["velocity1"] = new FrameBuffer(tex, false);

	tex = new Texture();
	tex->setFormat(GL_R16F, FLUID_GRID_W, FLUID_GRID_W, GL_RED, false, true);
	m_framebuffers["pressure0"] = new FrameBuffer(tex, false);
	tex->setFormat(GL_R16F, FLUID_GRID_W, FLUID_GRID_W, GL_RED, false, true);
	m_framebuffers["pressure1"] = new FrameBuffer(tex, false);

	tex = new Texture();
	tex->setFormat(GL_RGB16F, FLUID_GRID_W, FLUID_GRID_W, GL_RGB, false, true);
	m_framebuffers["temperature0"] = new FrameBuffer(tex, false);
	tex = new Texture();
	tex->setFormat(GL_RGB16F, FLUID_GRID_W, FLUID_GRID_W, GL_RGB, false, true);
	m_framebuffers["temperature1"] = new FrameBuffer(tex, true);

	tex = new Texture();
	tex->setFormat(GL_R16F, FLUID_GRID_W, FLUID_GRID_W, GL_RED, false, true);
	m_framebuffers["divergence"] = new FrameBuffer(tex, false);
}

void Graphics::resizeFBOs()
{
	m_framebuffers["fb0"]->resize(m_width, m_height);
	m_framebuffers["fb1"]->resize(m_width, m_height);
	m_framebuffers["ptbuf0"]->resize(m_width, m_height);
	m_framebuffers["ptbuf1"]->resize(m_width, m_height);
}

void Graphics::setupVideo(bool fullscreen)
{
	uint32_t	flags = SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_OPENGL | SDL_RESIZABLE;

	if (fullscreen) {
		flags |= SDL_FULLSCREEN;
		m_width = 1600;
		m_height = 900;
	}

	m_screen = SDL_SetVideoMode(m_width, m_height, 0, flags);
	if (!m_screen)
		throw std::string("Failed to set video mode");

	SDL_ShowCursor(fullscreen ? SDL_DISABLE : SDL_ENABLE);
}

void Graphics::shutdown()
{
}

void Graphics::resize(int width, int height)
{
	// un-load all shaders
	/*std::map<std::string,Shader*>::const_iterator iter;
	for (iter = shaders.begin(); iter != shaders.end(); ++iter) {
		std::cout << "Unloading shader " << iter->first << std::endl;
		iter->second->unload();
	}*/

	m_width = width;
	m_height = height;
	//setupVideo();
	glViewport(0, 0, m_width, m_height);

	// re-load all shaders
	/*for (iter = shaders.begin(); iter != shaders.end(); ++iter) {
		std::cout << "Loading shader " << iter->first << std::endl;
		iter->second->load();
	}*/

	/* don't need to reload all gl stuff after resize if
	 * we don't call SDL_SetVideoMode?
	 */
	/*font->reloadGL();
	create_textures();*/
	resizeFBOs();
}

void Graphics::swap_buffers()
{
	SDL_GL_SwapBuffers();
}

void Graphics::bindShader(const std::string& name)
{
	m_shader = m_shaders[name];
	m_shader->bind();
	m_shader->setUniform("invRes", 1.f/m_width, 1.f/m_height);
}

void Graphics::releaseShader()
{
	m_shader->release();
	m_shader = NULL;
}

Shader* Graphics::shader()
{
	return m_shader;
}

int Graphics::width()
{
	return m_width;
}

int Graphics::height()
{
	return m_height;
}

Shader* Graphics::shader(const std::string& name)
{
	if (!m_shaders[name])
		throw std::string("No such shader: ") + name;
	return m_shaders[name];
}

FrameBuffer* Graphics::framebuffer(const std::string& name)
{
	if (!m_framebuffers[name])
		throw std::string("No such framebuffer: ") + name;
	return m_framebuffers[name];
}

void Graphics::add_framebuffer(const std::string& name, bool depth_buf, bool interpol)
{
	m_framebuffers[name] = new FrameBuffer(m_width, m_height, depth_buf, interpol);
}

void Graphics::loadVertexShader(const std::string& name, const std::string& fname)
{
	if (!m_shaders[name]) {
		m_shaders[name] = new Shader();
	}
	m_shaders[name]->loadVertexShader(fname);
}

void Graphics::loadFragmentShader(const std::string& name, const std::string& fname)
{
	if (!m_shaders[name]) {
		m_shaders[name] = new Shader();
	}
	m_shaders[name]->loadFragmentShader(fname);
}

void Graphics::reloadShaders()
{
	std::map<std::string,Shader*>::const_iterator iter;
	for (iter = m_shaders.begin(); iter != m_shaders.end(); ++iter) {
		std::cout << "Reloading shader " << iter->first << std::endl;
		iter->second->reload();
	}
}

void Graphics::loadTexture(const std::string& name, const std::string& fname)
{
	if (!m_textures[name]) {// only load if not already loaded
		Texture*	tex = new Texture();
		tex->LoadTexture(fname);
		m_textures[name] = tex;
	}
}

void Graphics::loadOctree(const std::string& name, const std::string& fname)
{
	if (!m_textures[name]) {
		std::cout << "Loading octree " << fname << std::endl;
		Texture*	tex = new Texture();
		tex->LoadOctree(fname);
		m_textures[name] = tex;
	}
}

void Graphics::load1DTexture(const std::string& name, int width, int* data)
{
	if (!m_textures[name]) {
		Texture*	tex = new Texture();
		tex->load1DTexture(width, data);
		m_textures[name] = tex;
	}
}

Texture* Graphics::texture(const std::string& name)
{
	if (!m_textures[name])
		throw std::string("No such texture: ") + name;
	return m_textures[name];
}
