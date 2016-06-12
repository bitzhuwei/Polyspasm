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
/*
 * 3D Demo
 */

#include "Demo.h"
#include "TF3D.h"

#include <sstream>

#include "Animation.h"
#include "Shader.h"
#include "Audio.h"
#include "Graphics.h"
#include "Texture.h"

#include <time.h>

#if 0
#define TESTING
#endif

// TODO proper error
#define errx(val, msg) do {printf("%s\n",msg);exit(val);} while (0);

#define WINDOW_W (700)
#define WINDOW_H (700)

static bool	running = true;

static void print_help();
static TF3D* load_font(const char* fn);
static void handle_event(SDL_Event* event);
static void on_key(SDLKey key, int down);
static void shutdown();

static void main_loop();
static void restart();
static void pauseunpause();

enum eDemoState {
	DEMO_IDLE,
	DEMO_PAUSED,
	DEMO_PLAYING
};

static eDemoState g_state = DEMO_IDLE;
static double	g_time = 0;
static double	g_duration = 0;
static double	g_next_dur = 0;
static int		g_beat = 0;
static size_t	g_current = 0;
static size_t	g_next = 0;

static std::vector<double> beats;

static GLuint my_fbo;

#ifdef WIN32
#define _DMAIN wmain
#else
#define _DMAIN main
#endif

static std::vector<Animation*> animations;
static Graphics* gfx;

static void integrate(double tdelta);

double g_mouse_x = 0;
double g_mouse_y = 0;
double g_pos_x = 0;
double g_pos_z = 0;
double g_vel_x = 0;
double g_vel_z = 0;

static bool g_show_fps = false;
static bool g_slow_mo = false;
static bool g_mute = false;

static void setupAnimations();

int wmain(int argc, wchar_t* argv[])
{
	/* seed RNG */
	srand((unsigned)time(NULL));

	char		buf[512];
	const char*	fn = NULL;
	bool		fullscreen = false;

	fn = "../res/fonts/STCAIYUN.ttf";

	setlocale(LC_ALL, "");

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO) < 0) {
		errx(1, "Failed to initialize SDL!");
	}
	SDL_WM_SetCaption("Polyspasm", NULL);
	atexit(SDL_Quit);

	for (int i = 1; i < argc; ++i) {
		if (argv[i][0] == '-') {
			std::wstring cmd = argv[i];
			if (!cmd.compare(L"-h")) {
				print_help();
				exit(0);
			}
			else if (!cmd.compare(L"-f")) {
				fullscreen = true;
			}
			else {
				fprintf(stderr, "illegal option: %ws\n", argv[i]);
				print_help();
				exit(1);
			}
		}
		else {
			size_t	num;
			size_t	lim = 512;
			wcstombs_s(&num, buf, argv[i], lim);
			fn = buf;
		}
	}

	if (!fn) {
		fprintf(stderr, "No filename given!\n");
		print_help();
		exit(1);
	}

	gfx = new Graphics(WINDOW_W, WINDOW_H);
	gfx->setup(fullscreen);

	gfx->setFont(load_font(fn));

	animations.push_back(new FluidAnimation());// fire

	setupAnimations();

	beats.push_back(32.33);
	
	main_loop();

	return 0;
}

static void setupAnimations()
{
	for (size_t i = 0; i < animations.size(); ++i)
		animations[i]->setup(*gfx);
}

/* draw fps */
static void draw_fps(double fps)
{
	std::ostringstream oss;
	oss.setf(std::ios::fixed, std::ios::floatfield);
	oss.precision(1);
	oss << fps;
	std::string str = oss.str();

#if 0
	gfx->bindShader("fps");

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, 50, 0, 50);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef((GLfloat)(50 - gfx->font()->width(str) - 0.2), 0.2f, 0);

	glDisable(GL_DEPTH_TEST);
	glDepthMask(0);

	gfx->font()->render(gfx, str.c_str());

	glDepthMask(1);
	glEnable(GL_DEPTH_TEST);
#endif
}

static void main_loop()
{
	double	fps = 0;
	size_t	frames = 0;
	double	second = 0;
	Uint32	ticks;

	try {
		ticks = SDL_GetTicks();
		while (running) {
			SDL_Event event;
			Uint32 diff;
			double tdelta;

			/* increment time */
			diff = SDL_GetTicks() - ticks;
			tdelta = diff / 1000.0;
			if (g_slow_mo)
				tdelta *= 0.5;
			ticks += diff;

			frames += 1;
			second += tdelta;
			if (second > 1) {
				fps = frames / second;
				frames = 0;
				second = 0;

				if (g_show_fps) {
					std::ostringstream oss;
					oss.setf(std::ios::fixed, std::ios::floatfield);
					oss.precision(1);
					oss << fps;
					std::string str = oss.str();

					std::cout << "fps: " << str << std::endl;
				}
			}

			int	prev_anim = g_current;
			if (g_state == DEMO_PLAYING) {
				g_time += tdelta;
				g_duration += tdelta;

				size_t prev = g_current;
				while (animations[g_current]->getDuration() <= g_duration) {
					g_duration -= animations[g_current]->getDuration();
					g_duration += g_next_dur;
					g_next_dur = 0;
					g_current = (g_current + 1) % animations.size();
				}

				g_next = (g_current + 1) % animations.size();

				for (size_t i = g_beat; i < beats.size(); ++i) {
					if (g_time > beats[i]) {
						g_beat = i + 1;
					}
					else {
						break;
					}
				}

				if (g_current < prev) {
					restart();
				}
			}

			if (g_current != prev_anim) {
				std::cout << "time: " << g_time << std::endl;
				std::cout << "beat: " << g_beat << std::endl;
			}

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			if (g_state == DEMO_IDLE) {
				g_time += tdelta;
				for (size_t i = g_beat; i < beats.size(); ++i) {
					if (g_time > beats[i]) {
						g_beat = i + 1;
					}
					else {
						break;
					}
				}
				if (g_beat > 4) {
					g_time -= beats[g_beat - 1];
					g_beat = 0;
				}
				animations[g_current]->render(*gfx, fmod(g_time, animations[g_current]->getDuration()), g_beat, true);
			}
			else {
				if (g_duration > animations[g_current]->getTransitionStart()) {
					//g_next_dur = g_duration-animations[g_current]->getTransitionStart();
					g_next_dur = 0;
					animations[g_current]->renderTransition(*gfx, animations[g_next],
						g_duration - animations[g_current]->getTransitionStart(), g_beat);
				}
				else {
					animations[g_current]->render(*gfx, g_duration, g_beat, false);
				}
			}

			if (g_show_fps)
				draw_fps(fps);

			gfx->swap_buffers();

			integrate(tdelta);

			while (SDL_PollEvent(&event)) {
				handle_event(&event);
			}
		}
	}
	catch (std::string msg) {
		std::cerr << "Fatal exception:" << std::endl <<
			msg << std::endl;
	}
}

static void integrate(double tdelta)
{
	g_pos_x += tdelta * g_vel_x;
	g_pos_z += tdelta * g_vel_z;
}

/* Attempt to load a TrueType font file.
 */
TF3D* load_font(const char* fn)
{
	TF3D*	font = NULL;
	try {
		std::cout << "Loading typeface: " << fn << std::endl;
		font = new TF3D(fn);
		//font->prepare_string("0123456789.abcdefghijklmnopqrstuvxyz");
		std::cout << "  :done" << std::endl;
	}
	catch (std::string str) {
		errx(1, str.c_str());
	}
	return font;
}

static void shutdown()
{
	gfx->shutdown();
	delete gfx;
	exit(0);
}

void print_help()
{
	printf("usage: 3dtext FONT [OPTIONS]\n");
	printf("  where FONT is either the filename of a TrueTypeFont or SHAPE file\n");
	printf("  and TEXT is the text to render (if no shape is specified)\n");
	printf("  and OPTIONS is one of\n");
	printf("    -h    show help\n");
}

void handle_event(SDL_Event* event)
{
	static int cx = 0;
	static int cy = 0;
	static bool	mousedown = false;
	switch (event->type) {
	case SDL_MOUSEBUTTONDOWN:
		mousedown = true;
		break;
	case SDL_MOUSEBUTTONUP:
		mousedown = false;
		break;
	case SDL_MOUSEMOTION:
		if (mousedown) {
			g_mouse_x += (event->motion.x - cx) / (double)gfx->width();
			g_mouse_y += (event->motion.y - cy) / (double)gfx->height();
		}
		cx = event->motion.x;
		cy = event->motion.y;
		break;
	case SDL_QUIT:
		shutdown();
		break;
	case SDL_KEYDOWN:
		on_key(event->key.keysym.sym, 1);
		break;
	case SDL_KEYUP:
		on_key(event->key.keysym.sym, 0);
		break;
	case SDL_VIDEORESIZE:
		gfx->resize(event->resize.w, event->resize.h);
		break;
	default:;
	}
}

/* (re)start demo */
static void restart()
{
	g_state = DEMO_PLAYING;
	g_time = 0;
	g_duration = 0;
	g_next_dur = 0;
	g_beat = 0;
	g_current = 0;
	g_next = 0;
	setupAnimations();
}

/* pause/resume demo */
static void pauseunpause()
{
	if (g_state == DEMO_PLAYING) {
		g_state = DEMO_PAUSED;
	}
	else if (g_state == DEMO_PAUSED) {
		g_state = DEMO_PLAYING;
	}
	else {// DEMO_IDLE
		g_state = DEMO_PLAYING;
		g_time = 0;
		g_beat = 0;
		g_duration = 0;
		g_next_dur = 0;
		setupAnimations();
	}
}

static void on_key(SDLKey key, int down)
{
	switch (key) {
	case SDLK_q:
		if (down)
			running = false;
		break;
	case SDLK_r:
		if (down)
			gfx->reloadShaders();
		break;
	case SDLK_RIGHT:
		if (down) {
			g_current = (g_current + 1) % animations.size();
			g_time = 0;
			g_duration = 0;
			g_next_dur = 0;
			g_beat = 0;
			g_next = g_current;
		}
		break;
	case SDLK_LEFT:
		if (down) {
			if (g_current > 0)
				g_current = g_current - 1;
			g_time = 0;
			g_duration = 0;
			g_next_dur = 0;
			g_beat = 0;
			g_next = g_current;
		}
		break;
	case SDLK_RETURN:
		if (down)
			restart();
		break;
	case SDLK_SPACE:
		if (down)
			pauseunpause();
		break;
	case SDLK_f:
		if (down)
			g_show_fps = !g_show_fps;
		break;
	case SDLK_t:
		if (down)
			g_slow_mo = !g_slow_mo;
		break;
	case SDLK_m:
		if (down) {
			g_mute = !g_mute;
			Mix_VolumeMusic(g_mute ? 0 : MIX_MAX_VOLUME);
		}
		break;
	case SDLK_w:
		g_vel_z = down ? 2 : 0;
		break;
	case SDLK_s:
		g_vel_z = down ? -2 : 0;
		break;
	case SDLK_a:
		g_vel_x = down ? -2 : 0;
		break;
	case SDLK_d:
		g_vel_x = down ? 2 : 0;
		break;
	default:;
	}
}

void checkGLErrors()
{
	GLenum	err = glGetError();
	if (err != GL_NO_ERROR) {
		std::cerr << "OpenGL reported an error: ";
		switch (err) {
		case GL_INVALID_ENUM:
			std::cerr << "GL_INVALID_ENUM" << std::endl;
			break;
		case GL_INVALID_VALUE:
			std::cerr << "GL_INVALID_VALUE" << std::endl;
			break;
		case GL_INVALID_OPERATION:
			std::cerr << "GL_INVALID_OPERATION" << std::endl;
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			std::cerr << "GL_INVALID_FRAMEBUFFER_OPERATION" << std::endl;
			break;
		case GL_OUT_OF_MEMORY:
			std::cerr << "GL_OUT_OF_MEMORY" << std::endl;
			break;
		case GL_STACK_UNDERFLOW:
			std::cerr << "GL_STACK_UNDERFLOW" << std::endl;
			break;
		case GL_STACK_OVERFLOW:
			std::cerr << "GL_STACK_OVERFLOW" << std::endl;
			break;
		}
		std::cerr << std::endl;
		exit(1);
	}
}
