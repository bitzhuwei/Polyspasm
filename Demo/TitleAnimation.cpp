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

#if 1
#define TITLE_WAIT (6.92)
#define DURATION (15.57)
#define TRANS_WAIT (12.68)
#else
/* DEBUG */
#define TITLE_WAIT (0)
#define DURATION (5.57)
#define TRANS_WAIT (0.58)
#endif

#define SCANHEIGHT (400)
#define NUMDOTS (14)

TitleAnimation::TitleAnimation()
	: Animation(DURATION, DURATION-TRANS_WAIT)
{
	lines.push_back("./Polyspasm");
	lines.push_back("./Polyspasm|");
	std::string dots;
	for (int i = 0; i < NUMDOTS; ++i) {
		lines.push_back(std::string("by llbit") + dots);
		lines.push_back(std::string("by llbit") + dots + "|");
		dots += ".";
	}

	m_total_time = 0;
}

void TitleAnimation::setup(Graphics& gfx)
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

void TitleAnimation::render(Graphics& gfx, double time, int beat, bool idle)
{
	double			angle;
	double			distance;
	double			dur;
	TF3D*			font = gfx.font();

	GLfloat offset = (GLfloat) (-font->width(lines[1])/2);

	Animation::updateBeat(time, beat, 0, 1);

	if (m_total_time > 10)
		m_total_time -= 10;

	if (time < TITLE_WAIT) dur = 0;
	else dur = pow((time-TITLE_WAIT) / (DURATION-TITLE_WAIT), 2.0);

	angle = 107 * dur;
	if (angle < 1) angle = 1;
	double dist = 2*(1-dur) + 1.2*dur;
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
	glRotated(-88 * dur, 1, 0, 0);
	glRotated(88 * dur, 0, 0, 1);

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

void TitleAnimation::scanlines(Graphics& gfx, double time, bool idle)
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

void TitleAnimation::text(Graphics& gfx, double beattime, int beat, GLfloat offset)
{
	glMatrixMode(GL_MODELVIEW);

	int starttick = 5;
	int caret = 0;
	if (beattime < 0.9584375 / 2) caret = 1;
	if (beat < starttick) {
		glPushMatrix();
		glTranslatef(offset, -0.25f, 0.15f);
		gfx.font()->render(gfx, lines[caret]);
		glPopMatrix();
	} else {
		glPushMatrix();
		glTranslatef(offset, -0.25f, 0.15f);
		gfx.font()->render(gfx, lines[0]);
		glPopMatrix();

		glPushMatrix();
		glTranslatef(offset, -0.25f-1, 0.15f);
		int n = beat-starttick;
		if (n >= NUMDOTS) n = NUMDOTS-1;
		gfx.font()->render(gfx, lines[2 + caret + n*2]);
		glPopMatrix();
	}
}

void TitleAnimation::blur(Graphics& gfx)
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

void TitleAnimation::renderTransition(Graphics& gfx, Animation* next, double time, int beat)
{
	double			angle;
	double			distance;
	double			dur;
	double			ttime = time;
	TF3D*			font = gfx.font();
	GLfloat			offset = (GLfloat) (-font->width(lines[1])/2);
	double			trans_dur = pow(time / m_trans_dur, 2.0);

	double	alpha;

	if (time < m_trans_dur) alpha = pow(time / m_trans_dur, 2.0);
	else alpha = 1;

	time += TRANS_WAIT;

	render(gfx, time, beat, false);

	if (time < TITLE_WAIT) dur = 0;
	else dur = pow((time-TITLE_WAIT) / (DURATION-TITLE_WAIT), 2.0);

	angle = 107 * dur;
	if (angle < 1) angle = 1;
	double dist = 2*(1-dur) + 1.2*dur;
	distance = dist * tan(PI / 3) / tan(angle * PI / 360);
	GLfloat near = (GLfloat) (distance-3);
	if (near < 0.001f) near = 0.001f;

	/*glClear(GL_DEPTH_BUFFER_BIT);
	next->renderToFBO(gfx, ttime, beat);*/

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glDisable(GL_BLEND);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(angle, gfx.aspect(), near, distance+7);
	gluLookAt(
		0, 0, distance,
		0, 0, 0,
		0, 1, 0);
	glRotated(-88 * dur, 1, 0, 0);
	glRotated(88 * dur, 0, 0, 1);

	double	displacement = 1-m_beattime;
	if (displacement > 1.0) displacement = 1.0;
	else if (displacement < 0.0) displacement = 0.0;
	displacement = pow(displacement, 4.0);

	gfx.framebuffer("fb1")->bind();
	glClear(GL_COLOR_BUFFER_BIT);
	gfx.bindShader("fade");
	gfx.shader("fade")->setUniform("displacement", (GLfloat) displacement);
	gfx.shader("fade")->setUniform("alpha", (GLfloat) alpha);
	text(gfx, m_beattime, beat, offset);
	gfx.releaseShader();
	gfx.framebuffer("fb1")->release();

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	gfx.bindShader("drawfb");
	gfx.shader("drawfb")->sampler("tex", gfx.framebuffer("fb1"));
	glBegin(GL_QUADS);
	glVertex2f(-1,-1);
	glVertex2f( 1,-1);
	glVertex2f( 1, 1);
	glVertex2f(-1, 1);
	glEnd();
	gfx.releaseShader();

	glDisable(GL_BLEND);
}
