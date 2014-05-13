
CFLAGS=-O

first_rule: all

all allbin: ../lib/libW11.dll

alldoc tags:

../lib/libW11.dll: xlib.o ntutil.o event.o dllmain.o
	gcc -shared -mwindows -Wl,--export-all-symbols -Wl,--enable-auto-image-base -o ../lib/libW11.dll xlib.o ntutil.o event.o dllmain.o

xlib.o: xlib.c ntdef.h dllmain.h
	gcc -c xlib.c ${CFLAGS} -I..

ntutil.o: ntutil.c ntdef.h dllmain.h
	gcc -c ntutil.c ${CFLAGS} -I..

event.o: event.c ntdef.h dllmain.h
	gcc -c event.c ${CFLAGS} -I..

dllmain.o: dllmain.c dllmain.h
	gcc -c dllmain.c ${CFLAGS} -I..

debug:
	gcc -c xlib.c -g -I..
	gcc -c event.c -g -I..
	gcc -c ntutil.c -g -I..

clean:
	-rm ../lib/libW11.dll xlib.o ntutil.o event.o
