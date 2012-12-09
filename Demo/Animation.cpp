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

void Animation::setup(Graphics& gfx)
{
	m_lastbeat = 0;
	m_beattime = 1000;
	m_lasttime = 0;
}

void Animation::updateBeat(double time, int beat, int start, int bpl)
{
	m_beattime += time - m_lasttime;
	m_lasttime = time;

	m_current_beat = (beat-start) / bpl;

	if (m_current_beat >= 0 && m_current_beat != m_lastbeat) {
		m_lastbeat = m_current_beat;
		m_beattime = 0;
	}

	if (time < m_lasttime) {
		m_beattime = time;
		m_lasttime = time;
	} else {
		m_beattime += time - m_lasttime;
		m_lasttime = time;
	}
}