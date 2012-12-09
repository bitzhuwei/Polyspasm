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

#include "common.h"
#include "mat.h"

#define DURATION (27.95)
#define TRANS_WAIT (24.4)
#define TRANS_DURATION (DURATION-TRANS_WAIT)

#define NUMQUADS (60*2)

#define BEAT_START (56)
#define BPL (2)

#define VIEW_ANGLE (80)

double g_sky_trans_dur = TRANS_DURATION;

/* Permutation table for perlin noise */
extern int perm256[];

extern double g_mouse_x;
extern double g_mouse_y;
extern double g_pos_x;
extern double g_pos_z;

SkyAnimation::SkyAnimation()
	: Animation(DURATION, TRANS_DURATION)
{
	m_lines.push_back("Warning:");
	m_lines.push_back("do");
	m_lines.push_back("not");
	m_lines.push_back("submerge");
	m_lines.push_back("your");
	m_lines.push_back("computer");
	m_lines.push_back("in");
	m_lines.push_back("ordinary");
	m_lines.push_back("water");
}

void SkyAnimation::setup(Graphics& gfx)
{
	Animation::setup(gfx);

	gfx.loadShader("default");
	//gfx.loadShader("drawfb");
	gfx.loadShader("pt");
	gfx.loadShader("water");
	gfx.loadShader("depth");

	gfx.loadOctree("broville", "../res/octree/broville.octree");
	gfx.load1DTexture("blockColors", 256, blockColors);
	gfx.load1DTexture("blockEmittance", 256, blockEmittance);

	/* clear the pt bufs*/
	gfx.framebuffer("sky_front")->bind();
	glClear(GL_COLOR_BUFFER_BIT);
	gfx.framebuffer("sky_front")->release();
	gfx.framebuffer("sky_back")->bind();
	glClear(GL_COLOR_BUFFER_BIT);
	gfx.framebuffer("sky_back")->release();
	gfx.framebuffer("sky_left")->bind();
	glClear(GL_COLOR_BUFFER_BIT);
	gfx.framebuffer("sky_left")->release();
	gfx.framebuffer("sky_right")->bind();
	glClear(GL_COLOR_BUFFER_BIT);
	gfx.framebuffer("sky_right")->release();
	gfx.framebuffer("sky_up")->bind();
	glClear(GL_COLOR_BUFFER_BIT);
	gfx.framebuffer("sky_up")->release();
	gfx.framebuffer("sky_down")->bind();
	glClear(GL_COLOR_BUFFER_BIT);
	gfx.framebuffer("sky_down")->release();

	int	perm512[512];
	int	permmod12[512];
	for (int i = 0; i < 512; ++i) {
		int p = perm256[i%256];
		perm512[i] = p;
		permmod12[i] = p % 12;
	}
	gfx.load1DTexture("perm512", 512, perm512);
	gfx.load1DTexture("permmod12", 512, permmod12);
}

void SkyAnimation::render(Graphics& gfx, double time, int beat, bool idle)
{
	Animation::updateBeat(time, beat, BEAT_START, BPL);

	doPT(gfx, time);

#if 1
	//gfx.framebuffer("fb0")->bind();
	glClear(GL_DEPTH_BUFFER_BIT);
	renderSkybox(gfx, time, beat);
	//gfx.framebuffer("fb0")->release();

	/*gfx.bindShader("drawfb");
	gfx.shader("drawfb")->sampler("tex", gfx.framebuffer("fb0"));
	drawQuad();
	gfx.releaseShader();*/
#endif

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	//glDepthFunc(GL_LESS);
	//glClearDepth(1);
	gfx.framebuffer("fb0")->bind();
	glClear(GL_DEPTH_BUFFER_BIT);
	drawText(gfx, beat, time);
	gfx.framebuffer("fb0")->release();

	//glDepthFunc(GL_GREATER);
	//glClearDepth(0);
	glCullFace(GL_FRONT);
	gfx.framebuffer("fb1")->bind();
	glClear(GL_DEPTH_BUFFER_BIT);
	drawText(gfx, beat, time);
	gfx.framebuffer("fb1")->release();
	glCullFace(GL_BACK);

#if 0
	gfx.bindShader("depth");
	gfx.shader("depth")->sampler("depthFront", gfx.framebuffer("fb0")->depth());
	gfx.shader("depth")->sampler("depthBack", gfx.framebuffer("fb1")->depth());
	drawQuad();
	gfx.releaseShader();

#else
	gfx.bindShader("water");
	if (gfx.shader("water")->isReady()) {
		gfx.shader("water")->sampler("sky_front", gfx.framebuffer("sky_front"));
		gfx.shader("water")->sampler("sky_back", gfx.framebuffer("sky_back"));
		gfx.shader("water")->sampler("sky_left", gfx.framebuffer("sky_left"));
		gfx.shader("water")->sampler("sky_right", gfx.framebuffer("sky_right"));
		gfx.shader("water")->sampler("sky_up", gfx.framebuffer("sky_up"));
		gfx.shader("water")->sampler("sky_down", gfx.framebuffer("sky_down"));
		gfx.shader("water")->sampler("depthFront", gfx.framebuffer("fb0")->depth());
		gfx.shader("water")->sampler("depthBack", gfx.framebuffer("fb1")->depth());
		gfx.shader("water")->sampler("perm", gfx.texture("perm512"));
		gfx.shader("water")->sampler("perm12", gfx.texture("permmod12"));
		gfx.shader("water")->setUniform("offset", (GLfloat) time, (GLfloat) time);
		gfx.shader("water")->setUniform("alpha", (GLfloat) 1.0f);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glRotated(g_mouse_y*45, 1, 0, 0);
		glRotated(g_mouse_x*70, 0, 1, 0);
		glRotated(30 * time, 0, 1, 0);
		glRotated(5 * sin(time), 0, 0, 1);
		GLfloat matrix[16];
		glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
		mat4f mat(matrix);
		mat.invert();
		gfx.shader("water")->setUniformMat4("textTransform", mat);

		drawText(gfx, beat, time);

		gfx.releaseShader();
	}
#endif
}

void SkyAnimation::renderTransition(Graphics& gfx, Animation* next, double time, int beat)
{
	double ttime = time + TRANS_WAIT;
	Animation::updateBeat(ttime, beat, BEAT_START, BPL);

	doPT(gfx, time);

#if 1
	//gfx.framebuffer("fb0")->bind();
	glClear(GL_DEPTH_BUFFER_BIT);
	renderSkybox(gfx, ttime, beat);
	//gfx.framebuffer("fb0")->release();

	/*gfx.bindShader("drawfb");
	gfx.shader("drawfb")->sampler("tex", gfx.framebuffer("fb0"));
	drawQuad();
	gfx.releaseShader();*/
#endif

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	//glDepthFunc(GL_LESS);
	//glClearDepth(1);
	gfx.framebuffer("fb0")->bind();
	glClear(GL_DEPTH_BUFFER_BIT);
	drawText(gfx, beat, time);
	gfx.framebuffer("fb0")->release();

	//glDepthFunc(GL_GREATER);
	//glClearDepth(0);
	glCullFace(GL_FRONT);
	gfx.framebuffer("fb1")->bind();
	glClear(GL_DEPTH_BUFFER_BIT);
	drawText(gfx, beat, time);
	gfx.framebuffer("fb1")->release();
	glCullFace(GL_BACK);

#if 0
	gfx.bindShader("depth");
	gfx.shader("depth")->sampler("depthFront", gfx.framebuffer("fb0")->depth());
	gfx.shader("depth")->sampler("depthBack", gfx.framebuffer("fb1")->depth());
	drawQuad();
	gfx.releaseShader();

#else
	double trans_zero_alpha = 3;
	if (time > trans_zero_alpha) return;

	gfx.bindShader("water");
	if (gfx.shader("water")->isReady()) {
		gfx.shader("water")->sampler("sky_front", gfx.framebuffer("sky_front"));
		gfx.shader("water")->sampler("sky_back", gfx.framebuffer("sky_back"));
		gfx.shader("water")->sampler("sky_left", gfx.framebuffer("sky_left"));
		gfx.shader("water")->sampler("sky_right", gfx.framebuffer("sky_right"));
		gfx.shader("water")->sampler("sky_up", gfx.framebuffer("sky_up"));
		gfx.shader("water")->sampler("sky_down", gfx.framebuffer("sky_down"));
		gfx.shader("water")->sampler("depthFront", gfx.framebuffer("fb0")->depth());
		gfx.shader("water")->sampler("depthBack", gfx.framebuffer("fb1")->depth());
		gfx.shader("water")->sampler("perm", gfx.texture("perm512"));
		gfx.shader("water")->sampler("perm12", gfx.texture("permmod12"));
		gfx.shader("water")->setUniform("offset", (GLfloat) time, (GLfloat) time);

		double alpha = time / trans_zero_alpha;
		alpha = 1.0 - pow(alpha, 2.0);

		gfx.shader("water")->setUniform("alpha", (GLfloat) alpha);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glRotated(g_mouse_y*45, 1, 0, 0);
		glRotated(g_mouse_x*70, 0, 1, 0);
		glRotated(30 * ttime, 0, 1, 0);
		glRotated(5 * sin(ttime), 0, 0, 1);
		GLfloat matrix[16];
		glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
		mat4f mat(matrix);
		mat.invert();
		gfx.shader("water")->setUniformMat4("textTransform", mat);

		drawText(gfx, beat, ttime);

		glDisable(GL_BLEND);

		gfx.releaseShader();
	}
#endif
}

void SkyAnimation::drawText(Graphics& gfx, int beat, double time)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(VIEW_ANGLE, gfx.aspect(), 0.1, 5.0);
	gluLookAt(
		0, 0, 0,
		0, 0, -1,
		0, 1, 0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.f, 0.f, -3.f);
	glRotated(20 * sin(0.5*time), 0, 1, 0);
	glRotated(5 * sin(time), 0, 0, 1);
	//glRotated(g_mouse_y*45, 1, 0, 0);
	//glRotated(g_mouse_x*70, 0, 1, 0);

	size_t line_index;
	if (m_current_beat < 0) line_index = 0;
	else line_index = m_current_beat;
	if ((unsigned)m_current_beat >= m_lines.size())
		line_index = m_lines.size()-1;
	const std::string& line = m_lines[line_index];
	//double scale = 2 /gfx.font()->width(line);
	//scale *= alpha;
	//double scale = 1.2;
	//glScaled(scale, scale, scale);
	glTranslated(-gfx.font()->width(line)/2, -0.25, 0.07);
	glPushMatrix();

	gfx.font()->render(gfx, line);
	glPopMatrix();
}

void SkyAnimation::doPT(Graphics& gfx, double time)
{
	m_quads.clear();
	m_quads.reserve(NUMQUADS);

	/* random quads on a sphere */
	for (int i = 0; i < NUMQUADS; ++i) {
		vec3f	w;
		do {
			w.x = randf();
			w.y = randf();
			w.z = randf();
			if (w.square_length() <= 1 && w.square_length() > EPSILON) {
				w.normalize();
				break;
			}
		} while (true);

		m_quads.push_back(w);
	}

	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glDepthMask(GL_FALSE);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90, 1, 0.01, 4);

	glPushMatrix();
	gluLookAt(
		0, 0, 0,
		0, 0, -1,
		0, 1, 0);
	bindBuf(gfx, "sky_front");
	drawQuads(m_quads);
	releaseBuf(gfx, "sky_front");
	glPopMatrix();

	glPushMatrix();
	gluLookAt(
		0, 0, 0,
		0, 0, 1,
		0, 1, 0);
	bindBuf(gfx, "sky_back");
	drawQuads(m_quads);
	releaseBuf(gfx, "sky_back");
	glPopMatrix();

	glPushMatrix();
	gluLookAt(
		0, 0, 0,
		-1, 0, 0,
		0, 1, 0);
	bindBuf(gfx, "sky_left");
	drawQuads(m_quads);
	releaseBuf(gfx, "sky_left");
	glPopMatrix();

	glPushMatrix();
	gluLookAt(
		0, 0, 0,
		1, 0, 0,
		0, 1, 0);
	bindBuf(gfx, "sky_right");
	drawQuads(m_quads);
	releaseBuf(gfx, "sky_right");
	glPopMatrix();

	glPushMatrix();
	gluLookAt(
		0, 0, 0,
		0, 1, 0,
		0, 0, 1);
	bindBuf(gfx, "sky_up");
	drawQuads(m_quads);
	releaseBuf(gfx, "sky_up");
	glPopMatrix();

	glPushMatrix();
	gluLookAt(
		0, 0, 0,
		0, -1, 0,
		0, 0, -1);
	bindBuf(gfx, "sky_down");
	drawQuads(m_quads);
	releaseBuf(gfx, "sky_down");
	glPopMatrix();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
}

void SkyAnimation::bindBuf(Graphics& gfx, const std::string& name)
{
	GLsizei	fbwidth = gfx.framebuffer("sky_ptbuf")->width();
	GLsizei	fbheight = gfx.framebuffer("sky_ptbuf")->height();

	glPushAttrib(GL_VIEWPORT_BIT);
	glViewport(0,0, fbwidth, fbheight);

	gfx.framebuffer("sky_ptbuf")->bindDraw();
	gfx.framebuffer(name)->bindRead();
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	glReadBuffer(GL_COLOR_ATTACHMENT0);
	glBlitFramebuffer(0, 0, fbwidth, fbheight, 0, 0, fbwidth, fbheight,
		GL_COLOR_BUFFER_BIT, GL_NEAREST);
	glDrawBuffer(GL_COLOR_ATTACHMENT1);
	glReadBuffer(GL_COLOR_ATTACHMENT1);
	glBlitFramebuffer(0, 0, fbwidth, fbheight, 0, 0, fbwidth, fbheight,
		GL_COLOR_BUFFER_BIT, GL_NEAREST);
	gfx.framebuffer("sky_ptbuf")->useAllAttachments();
	gfx.framebuffer("sky_ptbuf")->releaseDraw();
	gfx.framebuffer(name)->releaseRead();

	checkGLErrors();

	gfx.framebuffer("sky_ptbuf")->bind();

	gfx.shader("pt")->bind();
	gfx.shader("pt")->setUniform("invRes", 1.f/fbwidth, 1.f/fbheight);
	gfx.shader("pt")->sampler("octree", gfx.texture("broville"));
	gfx.shader("pt")->sampler("blockColors", gfx.texture("blockColors"));
	gfx.shader("pt")->sampler("blockEmittance", gfx.texture("blockEmittance"));
	gfx.shader("pt")->sampler("prevSamples", gfx.framebuffer(name));
	gfx.shader("pt")->sampler("prevSampleCount", gfx.framebuffer(name)->color(1));
	gfx.shader("pt")->setUniform("depth", 9);
	gfx.shader("pt")->setUniform("alpha", 0.f);
	gfx.shader("pt")->setUniform("seed", randf(), randf());
	gfx.shader("pt")->setUniform("origin", 5.1f, +50.1f, -12.1f);
}

void SkyAnimation::releaseBuf(Graphics& gfx, const std::string& name)
{
	gfx.releaseShader();
	gfx.framebuffer("sky_ptbuf")->release();
	gfx.framebuffer("sky_ptbuf")->swapColorAttachment(gfx.framebuffer(name), 0);
	gfx.framebuffer("sky_ptbuf")->swapColorAttachment(gfx.framebuffer(name), 1);

	glPopAttrib();
}

void SkyAnimation::drawQuads(std::vector<vec3f>& quads)
{
	vec3f	u;
	vec3f	v;

	for (size_t i = 0; i < quads.size(); ++i) {
		vec3f&	w = quads[i];

		if (abs(w.x) > 0.1) {
			u = vec3f(0, 1, 0);
		} else {
			// w is approximately in the yz plane
			u = vec3f(1, 0, 0);
		}

		v = w.cross(u);
		v.normalize();
		u = v.cross(w);

		float scale = 0.025f;
		u *= scale;
		v *= scale;
		w *= 1;
		vec3f	x0 = w - u - v;
		vec3f	x1 = w + u - v;
		vec3f	x2 = w + u + v;
		vec3f	x3 = w - u + v;
		glBegin(GL_QUADS);
		glVertex3f(x0.x, x0.y, x0.z);
		glVertex3f(x1.x, x1.y, x1.z);
		glVertex3f(x2.x, x2.y, x2.z);
		glVertex3f(x3.x, x3.y, x3.z);
		glEnd();
	}
}

void SkyAnimation::renderSkybox(Graphics& gfx, double time, int beat)
{
	gfx.bindShader("default");

	glDisable(GL_LIGHTING);
	glDisable(GL_BLEND);
	glShadeModel(GL_FLAT);
	glColor4f(1,1,1,1);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(VIEW_ANGLE, gfx.aspect(), 0.01, 4);
	gluLookAt(
		0, 0, 0,
		0, 0, -1,
		0, 1, 0);
	glRotated(g_mouse_y*45, 1, 0, 0);
	glRotated(g_mouse_x*70, 0, 1, 0);
	glRotated(30 * time, 0, 1, 0);
	glRotated(5 * sin(time), 0, 0, 1);

	GLfloat	a = -0.5f;
	GLfloat	b = 0.5f;

	glDepthMask(GL_FALSE);

	/* front */
	gfx.shader("default")->sampler("tex", gfx.framebuffer("sky_front"));
	glBegin(GL_QUADS);
	glTexCoord2i(0,0); glVertex3f(a,a,a);
	glTexCoord2i(1,0); glVertex3f(b,a,a);
	glTexCoord2i(1,1); glVertex3f(b,b,a);
	glTexCoord2i(0,1); glVertex3f(a,b,a);
	glEnd();

	/* back */
	gfx.shader("default")->sampler("tex", gfx.framebuffer("sky_back"));
	glBegin(GL_QUADS);
	glTexCoord2i(1,1); glVertex3f(a,b,b);
	glTexCoord2i(0,1); glVertex3f(b,b,b);
	glTexCoord2i(0,0); glVertex3f(b,a,b);
	glTexCoord2i(1,0); glVertex3f(a,a,b);
	glEnd();

	/* left */
	gfx.shader("default")->sampler("tex", gfx.framebuffer("sky_left"));
	glBegin(GL_QUADS);
	glTexCoord2i(1,0); glVertex3f(a,a,a);
	glTexCoord2i(1,1); glVertex3f(a,b,a);
	glTexCoord2i(0,1); glVertex3f(a,b,b);
	glTexCoord2i(0,0); glVertex3f(a,a,b);
	glEnd();

	/* right */
	gfx.shader("default")->sampler("tex", gfx.framebuffer("sky_right"));
	glBegin(GL_QUADS);
	glTexCoord2i(0,0); glVertex3f(b,a,a);
	glTexCoord2i(1,0); glVertex3f(b,a,b);
	glTexCoord2i(1,1); glVertex3f(b,b,b);
	glTexCoord2i(0,1); glVertex3f(b,b,a);
	glEnd();

	/* up */
	gfx.shader("default")->sampler("tex", gfx.framebuffer("sky_up"));
	glBegin(GL_QUADS);
	glTexCoord2i(0,0); glVertex3f(a,b,a);
	glTexCoord2i(1,0); glVertex3f(b,b,a);
	glTexCoord2i(1,1); glVertex3f(b,b,b);
	glTexCoord2i(0,1); glVertex3f(a,b,b);
	glEnd();

	/* down */
	gfx.shader("default")->sampler("tex", gfx.framebuffer("sky_down"));
	glBegin(GL_QUADS);
	glTexCoord2i(0,0); glVertex3f(a,a,b);
	glTexCoord2i(1,0); glVertex3f(b,a,b);
	glTexCoord2i(1,1); glVertex3f(b,a,a);
	glTexCoord2i(0,1); glVertex3f(a,a,a);
	glEnd();

	glDepthMask(GL_TRUE);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void SkyAnimation::drawQuad()
{
	glDisable(GL_DEPTH_TEST);
	glBegin(GL_QUADS);
	glVertex2f(-1,-1);
	glVertex2f( 1,-1);
	glVertex2f( 1, 1);
	glVertex2f(-1, 1);
	glEnd();

	checkGLErrors();
}
