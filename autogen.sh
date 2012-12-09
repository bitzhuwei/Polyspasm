#!/bin/sh
autoreconf --install --force && \
	echo 'You can now cd test && ../configure && make'
