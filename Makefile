.PHONY: all run

all:
	cd linux-2.6.33/ && make ARCH=um

run:
	#linux-2.6.33/linux ubda=image/Ubuntu-EPFL-OS.img mem=128M con0=fd:0,fd:1 con=pts eth0=tuntap,,,192.168.0.254
	linux-2.6.33/linux ubda=image/Ubuntu-EPFL-OS.img mem=128M con0=fd:0,fd:1 con=pts

