default:	ma sv cv

ma:	defs.h ma.c
	gcc -o ma -g ma.c
sv:	defs.h sv.c
	gcc -o sv -g sv.c
cv:	defs.h cv.c
	gcc -o cv -g cv.c
ag:	defs.h ag.c
	gcc -o ag -g ag.c
