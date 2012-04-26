.PHONY: all run

all:
	cd linux-2.6.33/ && make ARCH=um

run:
	linux-2.6.33/linux ubda=image/Ubuntu-EPFL-OS.img mem=128M con0=fd:0,fd:1 con=pts

debug:
	gdb --args linux-2.6.33/linux ubda=image/Ubuntu-EPFL-OS.img mem=128M con0=fd:0,fd:1 con=pts

