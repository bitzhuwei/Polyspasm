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

#define DURATION (17.22)
#define TRANS_DURATION (0)

#define BEAT_START (88)

#define SCANHEIGHT (400)
#define NUMDOTS (14)

GreetingsAnimation::GreetingsAnimation()
	: Animation(DURATION, TRANS_DURATION)
{
	m_lines.push_back("Greetings to");
	m_lines.push_back("lft");
	m_lines.push_back("axx0");
	m_lines.push_back("maxim");
	m_lines.push_back("fsfg");
	m_lines.push_back("eekon");
	m_lines.push_back("oldshoes");
	m_lines.push_back("mejf");
	m_lines.push_back("rayben"); 
}

void GreetingsAnimation::setup(Graphics& gfx)
{
	Animation::setup(gfx);

	gfx.loadShader("drawfb");
	gfx.loadShader("blur_h");
	gfx.loadShader("blur_v");
	gfx.loadShader("displace");
	gfx.loadShader("fade");
	gfx.loadShader("scanlines");

	gfx.loadTexture("scanline", "../res/textures/scanline.png");
}

void GreetingsAnimation::render(Graphics& gfx, double time, int beat, bool idle)
{
	double			angle;
	double			distance;
	TF3D*			font = gfx.font();

	GLfloat offset = (GLfloat) (-font->width(m_lines[0])/2);

	Animation::updateBeat(time, beat, BEAT_START, 1);

	if (m_total_time > 10)
		m_total_time -= 10;

	angle = 0;
	if (angle < 1) angle = 1;
	double dist = 2;
	distance = dist * tan(PI / 3) / tan(angle * PI / 360);
	GLfloat near = (GLfloat) (distance-3);
	if (near < 0.001f) near = 0.001f;

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(angle, gfx.aspect(), near, distance+7);
	gluLookAt(
		0, 0, distance,
		0, 0, 0,
		0, 1, 0);

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
	scanlines(gfx, m_total_time, idle);
	gfx.bindShader("displace");
	gfx.shader("displace")->setUniform("color", 0.2f, 1.0f, 0.3f);
	gfx.shader("displace")->setUniform("displacement", (GLfloat) displacement);
	glEnable(GL_DEPTH_TEST);
	text(gfx, m_beattime, beat, offset);
	gfx.releaseShader();
	gfx.framebuffer("fb0")->release();

	blur(gfx);
}

void GreetingsAnimation::scanlines(Graphics& gfx, double time, bool idle)
{
	double	displacement = 0.0;
	if (!idle) {
		displacement = 1-m_beattime;
		if (displacement > 1.0) displacement = 1.0;
		else if (displacement < 0.0) displacement = 0.0;
		displacement = pow(displacement, 4.0);
	}

	gfx.bindShader("scanlines");
	gfx.shader("scanlines")->sampler("tex", gfx.texture("scanline"));
	gfx.shader("scanlines")->setUniform("displacement", (GLfloat) displacement);

	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	glBegin(GL_QUADS);
	glTexCoord2d(-100, SCANHEIGHT-m_total_time*0.1);glVertex2f(-100,-100);
	glTexCoord2d( 100, SCANHEIGHT-m_total_time*0.1);glVertex2f( 100,-100);
	glTexCoord2d( 100,-SCANHEIGHT-m_total_time*0.1);glVertex2f( 100, 100);
	glTexCoord2d(-100,-SCANHEIGHT-m_total_time*0.1);glVertex2f(-100, 100);
	glEnd();
	glDepthMask(GL_TRUE);

	gfx.releaseShader();
}

void GreetingsAnimation::text(Graphics& gfx, double beattime, int beat, GLfloat offset)
{
	glMatrixMode(GL_MODELVIEW);

	int starttick = 2;
	if (m_current_beat < starttick) {
		glPushMatrix();
		glTranslatef(offset, -0.25f, 0.15f);
		gfx.font()->render(gfx, m_lines[0]);
		glPopMatrix();
	} else {
		size_t line = 1 + (m_current_beat - starttick)/2;
		if (line >= m_lines.size())
			line = m_lines.size()-1;

		glPushMatrix();
		glTranslatef(offset, -0.25f, 0.15f);
		gfx.font()->render(gfx, m_lines[0]);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(offset, -0.25f-1, 0.15f);
		gfx.font()->render(gfx, m_lines[line]);
		glPopMatrix();
	}
}

void GreetingsAnimation::blur(Graphics& gfx)
{
	glDisable(GL_DEPTH_TEST);

	gfx.framebuffer("fb1")->bind();
	glClear(GL_COLOR_BUFFER_BIT);
	gfx.bindShader("blur_h");
	gfx.shader("blur_h")->sampler("color", gfx.framebuffer("fb0")->color(0));
	glBegin(GL_QUADS);
	glVertex2f(-1,-1);
	glVertex2f( 1,-1);
	glVertex2f( 1, 1);
	glVertex2f(-1, 1);
	glEnd();
	gfx.framebuffer("fb1")->release();

	gfx.bindShader("blur_v");
	gfx.shader("blur_v")->sampler("color", gfx.framebuffer("fb1")->color(0));

	glBegin(GL_QUADS);
	glVertex2f(-1,-1);
	glVertex2f( 1,-1);
	glVertex2f( 1, 1);
	glVertex2f(-1, 1);
	glEnd();
}
