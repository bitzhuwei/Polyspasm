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
#ifndef DEMO_SHADER_H
#define DEMO_SHADER_H

#include <GL/gl.h>

#include <vector>
#include <map>
#include <string>
#include "mat.h"

class Texture;
class FrameBuffer;

class Shader {
private:

	struct sampler {
		std::string	name;
		Texture*	texture;
	};

	struct shader {
		std::string	fname;
		GLenum		type;
		GLenum		id;
	};

	GLenum					m_prog_id;

	std::vector<shader>		m_shaders;
	std::vector<sampler>	m_samplers;
	std::map<std::string,bool>	m_warned;

	bool					m_ready;

	GLint					m_current_texunit;

	void setup();
	void loadShader(shader& s);
	void sampler(GLenum target, const std::string& name, GLuint id);
public:

	Shader();

	void removeSamplers();

	void addSampler(const std::string& name, Texture* texture);
	void sampler(const std::string& name, Texture* texture);
	void sampler(const std::string& name, FrameBuffer* fb);

	void bindFragDataLoc(const std::string& name, GLuint colorindex);

	void bind();
	void release() { glUseProgram(0); }

	void unload();
	void load();
	void reload();
	void link();

	GLint getAttribLocation(const std::string& name);

	void loadShader(std::string fname, GLenum type) {
		shader s;
		s.fname = fname;
		s.type = type;
		loadShader(s);
		m_shaders.push_back(s);
	}

	void loadVertexShader(std::string fname) {
		loadShader(fname, GL_VERTEX_SHADER);
	}

	void loadFragmentShader(std::string fname)  {
		loadShader(fname, GL_FRAGMENT_SHADER);
	}

	bool isReady() { return m_ready; }

	void setUniform(const char* name, GLfloat v);
	void setUniform(const char* name, GLfloat x, GLfloat y);
	void setUniform(const char* name, GLfloat x, GLfloat y, GLfloat z);
	void setUniform(const char* name, GLint v);
	void setUniformMat4(const char* name, GLfloat* mat);
	void setUniformMat4(const char* name, const mat4f& mat);
};

#endif
