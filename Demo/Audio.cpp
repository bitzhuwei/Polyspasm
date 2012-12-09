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

#include "Audio.h"

void Audio::setup()
{
	Mix_OpenAudio(44100, AUDIO_S16, 2, 4096);
	m_song = Mix_LoadMUS("../res/mp3/Lft - Kung fu goldfish.ogg");
	if (!m_song) {
		throw std::string("Could not load song!");
	}
}

void Audio::shutdown()
{
	Mix_FreeMusic(m_song);
	Mix_CloseAudio();
}

void Audio::start()
{
	Mix_PlayMusic(m_song, -1);
}

void Audio::pause()
{
	Mix_PauseMusic();
}

void Audio::resume()
{
	Mix_ResumeMusic();
}