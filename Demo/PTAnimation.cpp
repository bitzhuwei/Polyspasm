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

#include "vec.h"
#include "Animation.h"

#include "common.h"

#include "BlockData.h"

#if 1
#define DURATION (0.29)
#define TRANS_WAIT (15.18)
#else
/* DEBUG */
#define DURATION (20)
#define TRANS_WAIT (1.68)
#endif

#define NUMQUADS (130)

#define PTSELECT "pt"
#define NOEDGES
#define RANDOMQUADS

extern double g_mouse_x;
extern double g_mouse_y;
extern double g_pos_x;
extern double g_pos_z;

PTAnimation::PTAnimation()
	: Animation(DURATION, DURATION-TRANS_WAIT)
{
}

void PTAnimation::setup(Graphics& gfx)
{
	Animation::setup(gfx);

	gfx.loadShader(PTSELECT);
	gfx.loadShader("edges");
	gfx.loadShader("default");

	gfx.loadOctree("octree", "../res/octree/world.octree");
	gfx.load1DTexture("blockColors", 256, blockColors);
	gfx.load1DTexture("blockEmittance", 256, blockEmittance);

	/* clear the ptbuf */
	gfx.framebuffer("ptbuf1")->bind();
	glClear(GL_COLOR_BUFFER_BIT);
	gfx.framebuffer("ptbuf1")->release();
}

void PTAnimation::render(Graphics& gfx, double time, int beat, bool idle)
{
	renderToFBO(gfx, time, beat);

	drawRTBuf(gfx);
}

void PTAnimation::renderTransition(Graphics& gfx, Animation* next, double time, int beat)
{
	renderToFBO(gfx, time+TRANS_WAIT, beat);

#if 0
	next->render(gfx, time, beat, false);
#endif

#ifndef NOEDGES
	gfx.framebuffer("fb0")->bind();
	glClear(GL_COLOR_BUFFER_BIT);
	drawRTBuf(gfx);
	gfx.framebuffer("fb0")->release();
#endif

	gfx.bindShader("default");

#ifndef NOEDGES
	gfx.shader("default")->sampler("tex", gfx.framebuffer("fb0")->color(0));
#else
	gfx.shader("default")->sampler("tex", gfx.framebuffer("ptbuf1")->color(0));
#endif

	int nslices = 5;

	glDisable(GL_BLEND);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, 1, 0, 1);
	glBegin(GL_QUADS);
	GLfloat fact = 0.5f;
	GLfloat	uslice = 1.f/nslices;
	GLfloat	tslice = fact/nslices;
	GLfloat tfslice = 1-(nslices-1)*tslice;
	for (int i = 0; i < nslices; ++i) {
		GLfloat	t0 = i*tslice;
		GLfloat	t1 = t0 + tfslice;
		GLfloat	u0 = (nslices-i-1)*uslice;
		GLfloat	u1 = u0 + uslice;
		if ((time/m_trans_dur) < t0) {
			glTexCoord2f(u0,0); glVertex2f(u0,0);
			glTexCoord2f(u1,0); glVertex2f(u1,0);
			glTexCoord2f(u1,1); glVertex2f(u1,1);
			glTexCoord2f(u0,1); glVertex2f(u0,1);
		} else if ((time/m_trans_dur) < t1) {
			GLfloat y1 = (t1 - (GLfloat) (time/m_trans_dur)) / (t1-t0);
			GLfloat	y0 = y1-1;
			glTexCoord2f(u0,0); glVertex2f(u0,y0);
			glTexCoord2f(u1,0); glVertex2f(u1,y0);
			glTexCoord2f(u1,1); glVertex2f(u1,y1);
			glTexCoord2f(u0,1); glVertex2f(u0,y1);
		}
	}
	glEnd();
	gfx.releaseShader();

	gfx.releaseShader();
}

void PTAnimation::renderToFBO(Graphics& gfx, double time, int beat)
{
	double	alpha;
	double	view_angle;

	if (beat != m_lastbeat) {
		m_lastbeat = beat;
		m_beattime = 0;
	}

	m_beattime += time - m_lasttime;
	m_lasttime = time;

	alpha = 1-m_beattime;
	if (alpha < 0.0) alpha = 0.0;
	alpha = pow(alpha, 4.0);
	
	view_angle = 75;

	doRT(gfx, time, view_angle, alpha);
}

void PTAnimation::doRT(Graphics& gfx, double time, double view_angle,
		double alpha)
{
	bool reset = true;
	static double mx = g_mouse_x;
	static double my = g_mouse_y;
	static double px = g_pos_x;
	static double pz = g_pos_z;

	mx = g_mouse_x;
	my = g_mouse_y;
	px = g_pos_x;
	pz = g_pos_z;

	glDisable(GL_BLEND);

	GLsizei	fbwidth = gfx.framebuffer("ptbuf0")->width();
	GLsizei	fbheight = gfx.framebuffer("ptbuf0")->height();

	gfx.framebuffer("ptbuf0")->bindDraw();
	gfx.framebuffer("ptbuf1")->bindRead();
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	glReadBuffer(GL_COLOR_ATTACHMENT0);
	glBlitFramebuffer(0, 0, fbwidth, fbheight, 0, 0, fbwidth, fbheight,
		GL_COLOR_BUFFER_BIT, GL_NEAREST);
	if (!reset) {
		glDrawBuffer(GL_COLOR_ATTACHMENT1);
		glReadBuffer(GL_COLOR_ATTACHMENT1);
		glBlitFramebuffer(0, 0, fbwidth, fbheight, 0, 0, fbwidth, fbheight,
			GL_COLOR_BUFFER_BIT, GL_NEAREST);
	}
	gfx.framebuffer("ptbuf0")->useAllAttachments();
	gfx.framebuffer("ptbuf0")->releaseDraw();
	gfx.framebuffer("ptbuf1")->releaseRead();

	if (reset) {
		gfx.framebuffer("ptbuf1")->bind();
		glDrawBuffer(GL_COLOR_ATTACHMENT1);
		glClear(GL_COLOR_BUFFER_BIT);
		gfx.framebuffer("ptbuf1")->useAllAttachments();
		gfx.framebuffer("ptbuf1")->release();
		gfx.framebuffer("ptbuf0")->bind();
		glDrawBuffer(GL_COLOR_ATTACHMENT1);
		glClear(GL_COLOR_BUFFER_BIT);
		gfx.framebuffer("ptbuf0")->useAllAttachments();
		gfx.framebuffer("ptbuf0")->release();
	}

	checkGLErrors();

	gfx.framebuffer("ptbuf0")->bind();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(view_angle, gfx.aspect(), 0.01, 4);
	gluLookAt(
		0, 0, 0,
		0, 0, -1,
		0, 1, 0);
	glRotated(32 + g_mouse_y*45, 1, 0, 0);
	glRotated(g_mouse_x*70, 0, 1, 0);

	GLfloat	a = -0.5f;
	GLfloat	b = 0.5f;

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glDepthMask(GL_FALSE);

	gfx.bindShader(PTSELECT);
	gfx.shader(PTSELECT)->sampler("octree", gfx.texture("octree"));
	gfx.shader(PTSELECT)->sampler("blockColors", gfx.texture("blockColors"));
	gfx.shader(PTSELECT)->sampler("blockEmittance", gfx.texture("blockEmittance"));
	gfx.shader(PTSELECT)->sampler("prevSamples", gfx.framebuffer("ptbuf1")->color(0));
	gfx.shader(PTSELECT)->sampler("prevSampleCount", gfx.framebuffer("ptbuf1")->color(1));
	gfx.shader(PTSELECT)->setUniform("depth", 9);
	gfx.shader(PTSELECT)->setUniform("alpha", (GLfloat) alpha);
	gfx.shader(PTSELECT)->setUniform("seed", randf(), randf());
	gfx.shader(PTSELECT)->setUniform("origin", (GLfloat)g_pos_x, 0, (GLfloat)g_pos_z);
	gfx.shader(PTSELECT)->setUniform("origin", 0, 0, (GLfloat)(-230 + 10*time));

#ifdef RANDOMQUADS
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

		drawQuad(w);
	}
#else
	glBegin(GL_QUADS);

	/* front */
	glVertex3f(a,a,a);
	glVertex3f(b,a,a);
	glVertex3f(b,b,a);
	glVertex3f(a,b,a);

	/* back */
	glVertex3f(a,b,b);
	glVertex3f(b,b,b);
	glVertex3f(b,a,b);
	glVertex3f(a,a,b);

	/* left */
	glVertex3f(a,a,a);
	glVertex3f(a,b,a);
	glVertex3f(a,b,b);
	glVertex3f(a,a,b);

	/* right */
	glVertex3f(b,a,a);
	glVertex3f(b,a,b);
	glVertex3f(b,b,b);
	glVertex3f(b,b,a);

	/* up */
	glVertex3f(a,b,a);
	glVertex3f(b,b,a);
	glVertex3f(b,b,b);
	glVertex3f(a,b,b);

	/* down */
	glVertex3f(a,a,b);
	glVertex3f(b,a,b);
	glVertex3f(b,a,a);
	glVertex3f(a,a,a);

	glEnd();
#endif
	gfx.releaseShader();
	gfx.framebuffer("ptbuf0")->release();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);

	gfx.framebuffer("ptbuf0")->swapColorAttachment(gfx.framebuffer("ptbuf1"), 0);
	gfx.framebuffer("ptbuf0")->swapColorAttachment(gfx.framebuffer("ptbuf1"), 1);
}

void PTAnimation::drawQuad(vec3f w)
{
	vec3f	u;
	vec3f	v;

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

void PTAnimation::drawRTBuf(Graphics& gfx)
{
#ifndef NOEDGES
	gfx.bindShader("edges");
	gfx.shader("edges")->sampler("color", gfx.framebuffer("ptbuf1")->color(0));
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
#else
	gfx.bindShader("default");
	gfx.shader("default")->sampler("tex", gfx.framebuffer("ptbuf1")->color(0));
#endif

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
	gfx.releaseShader();
}
