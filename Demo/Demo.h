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
#ifndef DEMO_DEMO_H
#define DEMO_DEMO_H

#include <stdio.h>
#include <math.h>
#include <locale.h>
#include <wchar.h>
#include <assert.h>
#include <SDL/SDL.h>
#include <GL/glew.h>
#include <GL/glu.h>

#include <vector>
#include <string>
#include <iostream>
#include <map>

#ifndef PI
#define PI (3.1415926535897932384626433832795028841968)
#endif

extern "C" {
#include "../cttf/shape.h"
#include "../cttf/triangulate.h"
#include "../cttf/ttf.h"
#include "../cttf/text.h"
}

void checkGLErrors();

#endif
