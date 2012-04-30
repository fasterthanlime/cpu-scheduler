.PHONY: all run debug report

all:
	cd linux-2.6.33/ && make ARCH=um

run:
	linux-2.6.33/linux ubda=image/Ubuntu-EPFL-OS.img mem=128M con0=fd:0,fd:1 con=pts

debug:
	gdb --args linux-2.6.33/linux ubda=image/Ubuntu-EPFL-OS.img mem=128M con0=fd:0,fd:1 con=pts

report:
	rm -rf report
	mkdir -p report/source/kernel
	mkdir -p report/source/include
	cp linux-2.6.33/include/linux/sched.h report/source/include/
	cp linux-2.6.33/kernel/sched.c report/source/kernel/
	cp linux-2.6.33/kernel/sched_dummy.c report/source/kernel/
	cp linux-2.6.33/kernel/sched_fair.c report/source/kernel/
	cp linux-2.6.33/kernel/sched_rt.c report/source/kernel/

