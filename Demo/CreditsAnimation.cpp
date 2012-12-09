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
#include "TF3D.h"

#define DURATION (34.61)
#define TRANS_DURATION (0)

#define BEAT_START (88)

#define SCANHEIGHT (400)
#define NUMDOTS (14)

#define VIEW_ANGLE (60)

CreditsAnimation::CreditsAnimation()
	: Animation(DURATION, TRANS_DURATION)
{
	m_lines.push_back("song:"); 
	m_lines.push_back("Kung Fu Goldfish"); 
	m_lines.push_back("by lft");
}

void CreditsAnimation::setup(Graphics& gfx)
{
	Animation::setup(gfx);

	gfx.loadShader("normals");
	gfx.loadShader("edges");
}

void CreditsAnimation::render(Graphics& gfx, double time, int beat, bool idle)
{
	double			angle;
	double			distance;
	TF3D*			font = gfx.font();

	Animation::updateBeat(time, beat, BEAT_START, 1);

	if (m_total_time > 10)
		m_total_time -= 10;

	angle = 0;
	if (angle < 1) angle = 1;
	double dist = 2;
	distance = dist * tan(PI / 3) / tan(angle * PI / 360);
	GLfloat near = (GLfloat) (distance-3);
	if (near < 0.001f) near = 0.001f;

	glDisable(GL_BLEND);

	double	displacement = 0.0;
	if (!idle) {
		displacement = 1-m_beattime;
		if (displacement > 1.0) displacement = 1.0;
		else if (displacement < 0.0) displacement = 0.0;
		displacement = pow(displacement, 4.0);
	}

	gfx.framebuffer("fb0")->bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	renderSkybox(gfx, time, beat);
	glClear(GL_DEPTH_BUFFER_BIT);

	if (time >= 12.25) {
		gfx.bindShader("normals");
		glEnable(GL_DEPTH_TEST);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(VIEW_ANGLE, gfx.aspect(), 0.1, 15.0);
		gluLookAt(
			0, 0, 7.5,
			0, 0, 0,
			0, 1, 0);
		text(gfx, font);
	}

	gfx.releaseShader();
	gfx.framebuffer("fb0")->release();

	gfx.bindShader("edges");
	gfx.shader("edges")->sampler("color", gfx.framebuffer("fb0"));
	glDisable(GL_DEPTH_TEST);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glBegin(GL_QUADS);
	glVertex2f(-1,-1);
	glVertex2f( 1,-1);
	glVertex2f( 1, 1);
	glVertex2f(-1, 1);
	glEnd();
	gfx.releaseShader();
}

void CreditsAnimation::text(Graphics& gfx, TF3D* font)
{
	glMatrixMode(GL_MODELVIEW);

	double lineh = 1.2;
	double yoffset = 1 * lineh;

	for (int i = 0; i < 3; ++i) {
		glPushMatrix();
		glTranslated(-font->width(m_lines[i])/2, yoffset - lineh * i, 0.15f);
		gfx.font()->render(gfx, m_lines[i]);
		glPopMatrix();
	}
}

void CreditsAnimation::renderSkybox(Graphics& gfx, double time, int beat)
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
