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
#ifndef DEMO_ANIMATION_H
#define DEMO_ANIMATION_H

#include <vector>
#include <string>

#include "Shader.h"
#include "FrameBuffer.h"
#include "Graphics.h"
#include "vec.h"

class TF3D;

class Animation {
protected:
	double						m_duration;
	double						m_trans_dur;
	int							m_lastbeat;
	double						m_beattime;
	double						m_lasttime;
	double						m_total_time;
	int							m_current_beat;

public:

	Animation(double duration, double trans_dur)
		: m_duration(duration), m_trans_dur(trans_dur), m_total_time(0) { }

	double getTransitionDuration() { return m_trans_dur; }
	double getDuration() { return m_duration; }
	double getTransitionStart() { return getDuration() - getTransitionDuration(); }

	virtual void render(Graphics& g, double time, int beat, bool idle) = 0;
	virtual void renderTransition(Graphics& g, Animation* next, double time, int beat) { }
	virtual void setup(Graphics& gfx);
	virtual void shutdown() { }

	void updateBeat(double time, int beat, int start, int bpl);
};

class TitleAnimation : public Animation {
	std::vector<std::string>	lines;

	void scanlines(Graphics& gfx, double time, bool idle);
	void text(Graphics& gfx, double beattime, int beat, GLfloat offset);
	void blur(Graphics& gfx);
public:
	TitleAnimation();

	void setup(Graphics& gfx);

	void render(Graphics& g, double time, int beat, bool idle);
	void renderTransition(Graphics& g, Animation* next, double time, int beat);
};

class GreetingsAnimation : public Animation {
	std::vector<std::string>	m_lines;

	void scanlines(Graphics& gfx, double time, bool idle);
	void text(Graphics& gfx, double beattime, int beat, GLfloat offset);
	void blur(Graphics& gfx);
public:
	GreetingsAnimation();

	void setup(Graphics& gfx);

	void render(Graphics& g, double time, int beat, bool idle);
};

class CreditsAnimation : public Animation {
	std::vector<std::string>	m_lines;

	void text(Graphics& gfx, TF3D* font);
	void renderSkybox(Graphics& gfx, double time, int beat);
public:
	CreditsAnimation();

	void setup(Graphics& gfx);

	void render(Graphics& g, double time, int beat, bool idle);
};

class SkyAnimation : public Animation {
private:
	std::vector<std::string>	m_lines;
	std::vector<vec3f>			m_quads;

	void renderSkybox(Graphics& g, double time, int beat);
	void doPT(Graphics& gfx, double time);
	void drawQuads(std::vector<vec3f>& quads);
	void bindBuf(Graphics& gfx, const std::string& name);
	void releaseBuf(Graphics& gfx, const std::string& name);
	void drawQuad();
	void drawText(Graphics& gfx, int beat, double time);
public:
	SkyAnimation();

	void setup(Graphics& gfx);

	void render(Graphics& g, double time, int beat, bool idle);
	void renderTransition(Graphics& g, Animation* next, double time, int beat);
};

class SobelAnimation : public Animation {
private:
	std::vector<vec3f>			m_quads;

	void renderSkybox(Graphics& g, double time, int beat);
	void doPT(Graphics& gfx, double time);
	void drawQuads(std::vector<vec3f>& quads);
	void bindBuf(Graphics& gfx, const std::string& name);
	void releaseBuf(Graphics& gfx, const std::string& name);
	void drawQuad();
public:
	SobelAnimation();

	void setup(Graphics& gfx);

	void render(Graphics& g, double time, int beat, bool idle);
};

class PTAnimation : public Animation {
private:

	void doRT(Graphics& gfx, double time, double view_angle, double alpha);
	void drawRTBuf(Graphics& gfx);
	void drawQuad(vec3f w);
public:
	PTAnimation();

	void setup(Graphics& gfx);

	void render(Graphics& g, double time, int beat, bool idle);
	void renderToFBO(Graphics& g, double time, int beat);
	void renderTransition(Graphics& g, Animation* next, double time, int beat);
};

class FluidAnimation : public Animation {
private:
	std::vector<std::string>	m_lines;

	void advection(Graphics& gfx, const std::string& u, const std::string& x, double dissipation);
	void buoyancy(Graphics& gfx, const std::string& u, const std::string& temp);
	void jacobian(Graphics& gfx, const std::string& x, const std::string& b);
	void addInitialForces(Graphics& gfx, const std::string& dest);
	void addForces(Graphics& gfx, const std::string& dest);
	void computeDivergence(Graphics& gfx, const std::string& u, const std::string& dest);
	void subtractGradient(Graphics& gfx, const std::string& u, const std::string& x);
	void drawFB(Graphics& gfx, const std::string& name);
	void drawQuad();
	void drawBottomLine(Graphics& gfx);
	void testSimplex(Graphics& gfx, double time);
public:
	FluidAnimation();

	void setup(Graphics& gfx);

	void render(Graphics& g, double time, int beat, bool idle);
	void renderTransition(Graphics& g, Animation* next, double time, int beat);
};

#endif
