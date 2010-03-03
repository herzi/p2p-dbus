all: server client

server: server.c
	gcc -g -o $@ $< $(shell pkg-config --cflags --libs dbus-glib-1)

client: client.c
	gcc -g -o $@ $< $(shell pkg-config --cflags --libs dbus-glib-1)

check: all
	./server

