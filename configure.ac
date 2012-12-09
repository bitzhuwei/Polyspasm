# Set project name and version
AC_INIT([Demo], [1.0])

# Non-recursive make
AM_INIT_AUTOMAKE([subdir-objects])

# Check for programs
AC_PROG_CXX
AC_PROG_CC

# Check for libraries
AC_SEARCH_LIBS([glBegin], [gl GL], [], [AC_MSG_ERROR(OpenGL is required)])
AC_CHECK_LIB([GLU], [gluLookAt], [], [AC_MSG_ERROR(GLU is required)])
AC_CHECK_LIB([GLEW], [glewInit], [], [AC_MSG_ERROR(glew library is required)])
AC_CHECK_LIB([SDL], [SDL_Init], [], [AC_MSG_ERROR(SDL library is required)])
AC_CHECK_LIB([SDL_mixer], [Mix_OpenAudio], [], \
			 [AC_MSG_ERROR(SDL_mixer library is required)])
AC_CHECK_LIB([SDL_image], [IMG_LoadPNG_RW], [], \
			 [AC_MSG_ERROR(SDL_image is required)])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
