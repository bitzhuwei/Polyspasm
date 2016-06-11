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

#include "Animation.h"

#include "BlockData.h"

#define DURATION (20)
#define TRANS_START (18)

#if 1
#define BEAT_START (35)
#else
#define BEAT_START (2)
#endif

/* beats per line */
#define BPL (2)

#define VIEW_ANGLE (52)

/* Permutation table for perlin noise */
int perm256[] = { 151,160,137,91,90,15,
  131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
  190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
  88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
  77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
  102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
  135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
  5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
  223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
  129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
  251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
  49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
  138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
};

FluidAnimation::FluidAnimation()
	: Animation(DURATION, DURATION-TRANS_START)
{
	//m_lines.push_back("Warning:");
	//m_lines.push_back("do");
	//m_lines.push_back("not");
	//m_lines.push_back("ignite");
	//m_lines.push_back("your");
	m_lines.push_back("0123456789");
}

void FluidAnimation::setup(Graphics& gfx)
{
	Animation::setup(gfx);
	m_lastbeat = -1;

	gfx.loadShader("fire");
	//gfx.loadShader("colorize");
	gfx.loadShader("normals");
	gfx.loadShader("advect");
	gfx.loadShader("buoyancy");
	gfx.loadShader("divergence");
	gfx.loadShader("jacobian");
	gfx.loadShader("gradient");
	gfx.loadShader("mix");
	gfx.loadShader("fireedges");
	gfx.loadShader("blur");
	//gfx.loadShader("blob");
	//gfx.loadShader("simplex");

	int	perm512[512];
	int	permmod12[512];
	for (int i = 0; i < 512; ++i) {
		int p = perm256[i%256];
		perm512[i] = p;
		permmod12[i] = p % 12;
	}
	gfx.load1DTexture("perm512", 512, perm512);
	gfx.load1DTexture("permmod12", 512, permmod12);

	gfx.framebuffer("velocity0")->clear(GL_COLOR_BUFFER_BIT);
	gfx.framebuffer("temperature0")->clear(GL_COLOR_BUFFER_BIT);
}

#define V_DISSIPATION (1.0)
#define T_DISSIPATION (0.92)
#define TIMESTEP (0.25)
#define NJACOBIAN (40)
#define SIGMA (0.6)

void FluidAnimation::render(Graphics& gfx, double time, int beat, bool idle)
{
	Animation::updateBeat(time, beat, BEAT_START, BPL);

	int gridSize = gfx.framebuffer("velocity0")->width();
	glPushAttrib(GL_VIEWPORT_BIT);
	glViewport(1, 1, gridSize-2, gridSize-2);

	advection(gfx, "velocity", "velocity", V_DISSIPATION);
	advection(gfx, "velocity", "temperature", T_DISSIPATION);

	buoyancy(gfx, "velocity", "temperature");

	addForces(gfx, "temperature");

	computeDivergence(gfx, "velocity", "divergence");

	gfx.framebuffer("pressure0")->clear(GL_COLOR_BUFFER_BIT);
	for (int i = 0; i < NJACOBIAN; ++i) {
		jacobian(gfx, "pressure", "divergence");
	}

	subtractGradient(gfx, "velocity", "pressure");

	glPopAttrib();

	drawFB(gfx, "temperature0");

	//SDL_Delay((Uint32) (100 * TIMESTEP));
}

void FluidAnimation::renderTransition(Graphics& gfx, Animation* next, double time, int beat)
{
	int gridSize = gfx.framebuffer("velocity0")->width();
	glPushAttrib(GL_VIEWPORT_BIT);
	glViewport(1, 1, gridSize-2, gridSize-2);

	advection(gfx, "velocity", "velocity", V_DISSIPATION);
	advection(gfx, "velocity", "temperature", T_DISSIPATION);

	buoyancy(gfx, "velocity", "temperature");

	computeDivergence(gfx, "velocity", "divergence");

	gfx.framebuffer("pressure0")->clear(GL_COLOR_BUFFER_BIT);
	for (int i = 0; i < NJACOBIAN; ++i) {
		jacobian(gfx, "pressure", "divergence");
	}

	subtractGradient(gfx, "velocity", "pressure");

	glPopAttrib();

	drawFB(gfx, "temperature0");

	//SDL_Delay((Uint32) (100 * TIMESTEP));
}

void FluidAnimation::advection(Graphics& gfx, const std::string& u, const std::string& x, double dissipation)
{
	const char*	shader = "advect";
	int gridSize = gfx.framebuffer(x + "0")->width();
	gfx.framebuffer(x + "1")->bind();
	gfx.shader(shader)->bind();
	gfx.shader(shader)->setUniform("invRes", 1.f / gridSize, 1.f / gridSize);
	gfx.shader(shader)->sampler("x", gfx.framebuffer(x + "0")->color(0));
	gfx.shader(shader)->sampler("u", gfx.framebuffer(u + "0")->color(0));
	gfx.shader(shader)->setUniform("tDelta", (GLfloat) TIMESTEP);
	gfx.shader(shader)->setUniform("dissipation", (GLfloat) dissipation);
	drawQuad();
	gfx.shader(shader)->release();
	gfx.framebuffer(x + "1")->release();
	gfx.framebuffer(x + "1")->swapColorAttachment(gfx.framebuffer(x + "0"), 0);
}

void FluidAnimation::buoyancy(Graphics& gfx, const std::string& u, const std::string& temp)
{
	const char*	shader = "buoyancy";
	int gridSize = gfx.framebuffer(u + "0")->width();
	gfx.framebuffer(u + "1")->bind();
	gfx.shader(shader)->bind();
	gfx.shader(shader)->setUniform("invRes", 1.f / gridSize, 1.f / gridSize);
	gfx.shader(shader)->sampler("u", gfx.framebuffer(u + "0")->color(0));
	gfx.shader(shader)->sampler("temp", gfx.framebuffer(temp + "0")->color(0));
	gfx.shader(shader)->setUniform("tDelta", (GLfloat) TIMESTEP);
	gfx.shader(shader)->setUniform("sigma", (GLfloat) SIGMA);
	drawQuad();
	gfx.releaseShader();
	gfx.framebuffer(u + "1")->release();
	gfx.framebuffer(u + "1")->swapColorAttachment(gfx.framebuffer(u + "0"), 0);
}

#define GRIDSCALE (1.25f)

void FluidAnimation::computeDivergence(Graphics& gfx, const std::string& u, const std::string& dest)
{
	const char*	shader = "divergence";
	int gridSize = gfx.framebuffer(u + "0")->width();
	gfx.framebuffer(dest)->bind();
	gfx.shader(shader)->bind();
	gfx.shader(shader)->setUniform("invRes", 1.f / gridSize, 1.f / gridSize);
	gfx.shader(shader)->sampler("w", gfx.framebuffer(u + "0")->color(0));
	gfx.shader(shader)->setUniform("halfrdx", (GLfloat) (0.5f / GRIDSCALE));
	drawQuad();
	gfx.shader(shader)->release();
	gfx.framebuffer(dest)->release();
}

void FluidAnimation::jacobian(Graphics& gfx, const std::string& x, const std::string& b)
{
	const char*	shader = "jacobian";
	int gridSize = gfx.framebuffer(x + "0")->width();
	gfx.framebuffer(x + "1")->bind();
	gfx.shader(shader)->bind();
	gfx.shader(shader)->setUniform("invRes", 1.f / gridSize, 1.f / gridSize);
	gfx.shader(shader)->sampler("x", gfx.framebuffer(x + "0")->color(0));
	gfx.shader(shader)->sampler("b", gfx.framebuffer(b)->color(0));
	gfx.shader(shader)->setUniform("alpha", (GLfloat) (- GRIDSCALE*GRIDSCALE));
	gfx.shader(shader)->setUniform("rBeta", 0.25f);
	drawQuad();
	gfx.shader(shader)->release();
	gfx.framebuffer(x + "1")->release();
	gfx.framebuffer(x + "1")->swapColorAttachment(gfx.framebuffer(x + "0"), 0);
}

void FluidAnimation::subtractGradient(Graphics& gfx, const std::string& u, const std::string& p)
{
	const char*	shader = "gradient";
	int gridSize = gfx.framebuffer(u + "0")->width();
	gfx.framebuffer(u + "1")->bind();
	gfx.shader(shader)->bind();
	gfx.shader(shader)->setUniform("invRes", 1.f / gridSize, 1.f / gridSize);
	gfx.shader(shader)->sampler("w", gfx.framebuffer(u + "0")->color(0));
	gfx.shader(shader)->sampler("p", gfx.framebuffer(p + "0")->color(0));
	gfx.shader(shader)->setUniform("halfrdx", (GLfloat) (0.5f / GRIDSCALE));
	drawQuad();
	gfx.shader(shader)->release();
	gfx.framebuffer(u + "1")->release();
	gfx.framebuffer(u + "1")->swapColorAttachment(gfx.framebuffer(u + "0"), 0);
}

void FluidAnimation::addInitialForces(Graphics& gfx, const std::string& dest)
{
	std::string	d0 = dest + "0";
	std::string	d1 = dest + "1";
	std::string	d2 = "density0";

	gfx.framebuffer(d1)->bind();
	gfx.bindShader("blob");
	gfx.shader("blob")->setUniform("pos", 256.f, 256.f);
	gfx.shader("blob")->setUniform("radius", 250.f);
	drawQuad();
	gfx.releaseShader();
	gfx.framebuffer(d1)->release();

	gfx.framebuffer(d2)->bind();
	gfx.bindShader("simplex");
	gfx.shader("simplex")->sampler("perm", gfx.texture("perm512"));
	gfx.shader("simplex")->sampler("perm12", gfx.texture("permmod12"));
	gfx.shader("simplex")->setUniform("offset", (GLfloat) m_lasttime, (GLfloat) m_lasttime);
	drawQuad();
	gfx.releaseShader();
	gfx.framebuffer(d2)->release();

	gfx.framebuffer(d1)->bind();
	gfx.bindShader("mix");
	gfx.shader("mix")->sampler("dest", gfx.framebuffer(d0)->color(0));
	gfx.shader("mix")->sampler("src", gfx.framebuffer(d2)->color(0));
	gfx.shader("mix")->setUniform("width", gfx.framebuffer(d0)->color(0)->width());
	drawQuad();
	gfx.releaseShader();
	gfx.framebuffer(d1)->release();

	gfx.framebuffer(d1)->release();
	gfx.framebuffer(d1)->swapColorAttachment(gfx.framebuffer(d0), 0);
}

void FluidAnimation::addForces(Graphics& gfx, const std::string& dest)
{
	std::string	d0 = dest + "0";
	std::string	d1 = dest + "1";
	std::string	d2 = "velocity1";
	std::string	d3 = "pressure1";

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90, gfx.aspect(), 0.1, 15);
	gluLookAt(
		0, 0, 2.5,
		0, 0, 0,
		0, 1, 0);
	glRotated(15 * sin(0.2 * m_lasttime), 0, 1, 0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);

	size_t line_index = m_current_beat;
	if (line_index < 0)
		line_index = 0;
	if (line_index >= m_lines.size())
		line_index = m_lines.size()-1;
	const std::string& line = m_lines[line_index];

	gfx.framebuffer(d1)->bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	gfx.bindShader("normals");
	glTranslated(-gfx.font()->width(line)/2, -0.75, 0.07);
	glPushMatrix();
	gfx.font()->render(gfx, line);
	glPopMatrix();

	gfx.releaseShader();
	gfx.framebuffer(d1)->release();

	glDisable(GL_DEPTH_TEST);

	gfx.framebuffer(d2)->bind();
	gfx.bindShader("fireedges");
	gfx.shader("fireedges")->sampler("color", gfx.framebuffer(d1)->color(0));
	gfx.shader("fireedges")->sampler("perm", gfx.texture("perm512"));
	gfx.shader("fireedges")->sampler("perm12", gfx.texture("permmod12"));
	gfx.shader("fireedges")->setUniform("offset", (GLfloat) m_beattime, (GLfloat) m_beattime);
	drawQuad();
	gfx.releaseShader();
	gfx.framebuffer(d2)->release();

	gfx.framebuffer(d3)->bind();
	gfx.bindShader("blur");
	gfx.shader("blur")->sampler("source", gfx.framebuffer(d2)->color(0));
	drawQuad();
	gfx.releaseShader();
	gfx.framebuffer(d3)->release();

	gfx.framebuffer(d1)->bind();
	gfx.bindShader("mix");
	gfx.shader("mix")->sampler("dest", gfx.framebuffer(d0)->color(0));
	gfx.shader("mix")->sampler("src", gfx.framebuffer(d3)->color(0));
	gfx.shader("mix")->setUniform("width", gfx.framebuffer(d0)->color(0)->width());
	drawQuad();
	gfx.releaseShader();
	gfx.framebuffer(d1)->release();

	gfx.framebuffer(d1)->release();
	gfx.framebuffer(d1)->swapColorAttachment(gfx.framebuffer(d0), 0);
}

void FluidAnimation::drawFB(Graphics& gfx, const std::string& name)
{
	gfx.bindShader("fire");
	gfx.shader("fire")->sampler("value", gfx.framebuffer(name)->color(0));
	drawQuad();
	gfx.releaseShader();
}

void FluidAnimation::drawQuad()
{
	glDisable(GL_DEPTH_TEST);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, 1, 0, 1);
	glBegin(GL_QUADS);
	glTexCoord2f(0,0); glVertex2f(0,0);
	glTexCoord2f(1,0); glVertex2f(1,0);
	glTexCoord2f(1,1); glVertex2f(1,1);
	glTexCoord2f(0,1); glVertex2f(0,1);
	glEnd();

	checkGLErrors();
}

void FluidAnimation::drawBottomLine(Graphics& gfx)
{
	int width = gfx.framebuffer("velocity0")->color(0)->width();
	glDisable(GL_DEPTH_TEST);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, width, 0, width);
	glBegin(GL_LINES);
	glVertex2i(0,1);
	glVertex2i(width,1);
	glEnd();
}

void FluidAnimation::testSimplex(Graphics& gfx, double time)
{
	const char*	shader = "simplex";
	gfx.shader(shader)->bind();
	gfx.shader(shader)->sampler("perm", gfx.texture("perm512"));
	gfx.shader(shader)->sampler("perm12", gfx.texture("permmod12"));
	gfx.shader(shader)->setUniform("offset", (GLfloat) time, (GLfloat) time);
	drawQuad();
	gfx.shader(shader)->release();
}

