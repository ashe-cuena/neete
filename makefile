TARGET=neete_exec

CC=gcc

DEBUG=-g

OPT=-O0

WARN=-Wall

PTHREAD=-pthread

PKGCONFIG=pkg-config

GLIB_COMPILE_RESOURCES = $(shell $(PKGCONFIG) --variable=glib_compile_resources gio-2.0)

resources = $(shell $(GLIB_COMPILE_RESOURCES) --sourcedir=. --generate-dependencies neete.gresource.xml)

CCFLAGS=$(DEBUG) $(OPT) $(WARN) $(PTHREAD) -pipe

GTKLIB=`pkg-config --cflags --libs gtk+-3.0 gtksourceview-3.0`

LD=gcc
LDFLAGS=$(PTHREAD) $(GTKLIB) -export-dynamic -lm -ldl

OBJS=    src/neete.o src/neete_resources.o src/cjson.o src/dynarray.o

all: $(OBJS)
	$(LD) -o $(TARGET) $(OBJS) $(LDFLAGS)

src/neete.o: src/neete_source.c
	$(CC) -c $(CCFLAGS) src/neete_source.c $(GTKLIB) -o src/neete.o

src/cjson.o: src/cJSON.c
	$(CC) -c $(CCFLAGS) src/cJSON.c -o src/cjson.o

src/dynarray.o: src/dynarray.c
	$(CC) -c $(CCFLAGS) src/dynarray.c -o src/dynarray.o

src/neete_resources.o: neete_resources.c
	$(CC) -c $(CCFLAGS) neete_resources.c $(GTKLIB) -o src/neete_resources.o

neete_resources.c: neete.gresource.xml $(resources)
	$(GLIB_COMPILE_RESOURCES) neete.gresource.xml --target=$@ --sourcedir=. --generate-source

clean:
	rm -f src/*.o $(TARGET)
