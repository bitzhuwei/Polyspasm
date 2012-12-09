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

#include <string>
#include <fstream>
#include <streambuf>
#include <iostream>

#include "Shader.h"
#include "Texture.h"
#include "FrameBuffer.h"

Shader::Shader()
	: m_ready(false)
{
	m_prog_id = glCreateProgram();
}

void Shader::unload()
{
	for (size_t i = 0; i < m_shaders.size(); ++i) {
		glDetachShader(m_prog_id, m_shaders[i].id);
		glDeleteShader(m_shaders[i].id);
	}

	glDeleteProgram(m_prog_id);

	m_ready = false;
}

void Shader::load()
{
	m_prog_id = glCreateProgram();

	for (size_t i = 0; i < m_shaders.size(); ++i) {
		loadShader(m_shaders[i]);
	}

	link();
}

void Shader::reload()
{
	for (size_t i = 0; i < m_shaders.size(); ++i) {
		glDetachShader(m_prog_id, m_shaders[i].id);
	}

	for (size_t i = 0; i < m_shaders.size(); ++i) {
		loadShader(m_shaders[i]);
	}

	link();
}

static void echoProgramInfoLog(GLuint id)
{
	GLint	info_len;
	char*	log;

	glGetProgramiv(id, GL_INFO_LOG_LENGTH, &info_len);

	if (info_len > 0) {
		log = new char[info_len];
		glGetProgramInfoLog(id, info_len, &info_len, log);
		std::cout << log << std::endl;
		delete log;
	}
}

void Shader::link()
{
	glLinkProgram(m_prog_id);

	echoProgramInfoLog(m_prog_id);
	GLint	status;
	glGetProgramiv(m_prog_id, GL_LINK_STATUS, &status);
	if (status != GL_TRUE) {
		std::cout << "FAILED to link program!" << std::endl;
		m_ready = false;
	} else {
		m_ready = true;
	}
}

void Shader::bindFragDataLoc(const std::string& name, GLuint colorindex)
{
	if (m_ready) {
		glBindFragDataLocation(m_prog_id, colorindex, name.c_str());
	}
}

GLint Shader::getAttribLocation(const std::string& name)
{
	return glGetAttribLocation(m_prog_id, name.c_str());
}

void Shader::removeSamplers()
{
	m_samplers.clear();
}

void Shader::addSampler(const std::string& name, Texture* texture)
{
	struct sampler tex;
	tex.name = name;
	tex.texture = texture;
	m_samplers.push_back(tex);
}

static void echoShaderInfoLog(GLuint id)
{
	GLint	info_len;
	char*	log;

	glGetShaderiv(id, GL_INFO_LOG_LENGTH, &info_len);

	if (info_len > 0) {
		log = new char[info_len];
		glGetShaderInfoLog(id, info_len, &info_len, log);
		std::cout << log << std::endl;
		delete log;
	}
}

void Shader::loadShader(shader& s)
{
	std::ifstream shader_file(s.fname.c_str());
	std::string shader_code;
	if (!shader_file.is_open())
		throw std::string("Could not load shader: ") + s.fname;
	shader_file.seekg(0, std::ios::end);   
	shader_code.reserve((size_t) shader_file.tellg());
	shader_file.seekg(0, std::ios::beg);
	shader_code.assign((std::istreambuf_iterator<char>(shader_file)),
		std::istreambuf_iterator<char>());
	shader_file.close();

	const char* code = shader_code.c_str();
	
	s.id = glCreateShader(s.type);
	glShaderSource(s.id, 1, &code, NULL);
	glCompileShader(s.id);

	std::cout << "Compiling shader " << s.fname << std::endl;
	echoShaderInfoLog(s.id);
	GLint status;
	glGetShaderiv(s.id, GL_COMPILE_STATUS, &status);
	if (status != GL_TRUE) {
		std::cout << "FAILED to compile shader " << s.fname << std::endl;
	}

	glAttachShader(m_prog_id, s.id);
}

void Shader::bind()
{
	if (m_ready) {
		glUseProgram(m_prog_id);

		m_current_texunit = 0;
		for (size_t i = 0; i < m_samplers.size(); ++i) {
			sampler(m_samplers[i].name, m_samplers[i].texture);
		}
	}
}

void Shader::sampler(const std::string& name, Texture* texture)
{
	sampler(texture->glTarget(), name, texture->glID());
}

void Shader::sampler(const std::string& name, FrameBuffer* fb)
{
	sampler(name, fb->color(0));
}

void Shader::sampler(GLenum target, const std::string& name, GLuint id)
{
	GLint	location = glGetUniformLocation(m_prog_id, name.c_str());
	checkGLErrors();
	if (location == -1) {
		if (m_warned.count(name) == 0) {
			std::cerr << "WARNING: Could not find location for uniform sampler " <<
				name << std::endl;
			m_warned[name] = true;
		}
	} else {
		glUniform1i(location, m_current_texunit);
		glActiveTexture(GL_TEXTURE0 + m_current_texunit);
		glBindTexture(target, id);

		m_current_texunit += 1;
	}
}

void Shader::setUniform(const char* name, GLfloat v)
{
	if (m_ready) {
		GLint loc = glGetUniformLocation(m_prog_id, name);
		glUniform1f(loc, v);
	}
}

void Shader::setUniform(const char* name, GLfloat x, GLfloat y)
{
	if (m_ready) {
		GLint loc = glGetUniformLocation(m_prog_id, name);
		glUniform2f(loc, x, y);
	}
}

void Shader::setUniform(const char* name, GLfloat x, GLfloat y, GLfloat z)
{
	if (m_ready) {
		GLint loc = glGetUniformLocation(m_prog_id, name);
		glUniform3f(loc, x, y, z);
	}
}

void Shader::setUniform(const char* name, GLint v)
{
	if (m_ready) {
		GLint loc = glGetUniformLocation(m_prog_id, name);
		glUniform1i(loc, v);
	}
}

void Shader::setUniformMat4(const char* name, GLfloat* mat)
{
	if (m_ready) {
		GLint loc = glGetUniformLocation(m_prog_id, name);
		glUniformMatrix4fv(loc, 1, GL_FALSE, mat);
	}
}

void Shader::setUniformMat4(const char* name, const mat4f& mat)
{
	if (m_ready) {
		GLint loc = glGetUniformLocation(m_prog_id, name);
		GLfloat matrix[16];
		matrix[0] = mat.m11;
		matrix[1] = mat.m12;
		matrix[2] = mat.m13;
		matrix[3] = mat.m14;
		matrix[4] = mat.m21;
		matrix[5] = mat.m22;
		matrix[6] = mat.m23;
		matrix[7] = mat.m24;
		matrix[8] = mat.m31;
		matrix[9] = mat.m32;
		matrix[10] = mat.m33;
		matrix[11] = mat.m34;
		matrix[12] = mat.m41;
		matrix[13] = mat.m42;
		matrix[14] = mat.m43;
		matrix[15] = mat.m44;
		glUniformMatrix4fv(loc, 1, GL_FALSE, matrix);
	}
}