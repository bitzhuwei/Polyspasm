Polyspasm
=========

Polyspasm is a demo I created in late 2012 for the course
EDAN35 High Performance Computer Graphics.

The code is written in C++ and is quite ugly due to the time requirements
that I was working with.

The song used is Kung Fu Goldfish by Linus "lft" Åkesson.
Parts of the Minecraft map Broville v10 by oldshoes are also used in the demo.

All code is copyright of Jesper Öqvist, and distributed under the GPLv2 license.
See the LICENSE file for the full GPLv2 license.

Usage
-----

* Press `Q` to exit.
* Press `SPACE` to start or pause the demo.
* Press `RETURN` to restart the demo.
* `LEFT`/`RIGHT` switches to the next/previous animation.
* Press `R` to reload shaders.

Report
------

There is a report in the `report` subdirectory with more details about
the effects used in the demo, and some conclusions and thoughts about the
development.

Building
--------

Visual Studio 2010 project files are provided to build the demo.
A shell script to set up a configure script with autotools is provided but
might need tweaking to work.


Ubuntu
------

Required packages:

* libgl1-mesa-dev
* libglu1-mesa-dev
* libglew1.5-dev
* libsdl-mixer1.2-dev

Follow these steps to build (may require tweaking of Makefile.am):

1. `./autogen.sh`
2. `cd test`
3. `../configure`
4. `make`
