all: server client

server: server.c glue.h
	gcc -g -o $@ $< $(shell pkg-config --cflags --libs dbus-glib-1)
glue.h: p2p-object.xml
	dbus-binding-tool --mode=glib-server --prefix=p2p_object --output=$@ $<

client: client.c bind.h
	gcc -g -o $@ $< $(shell pkg-config --cflags --libs dbus-glib-1)
bind.h: p2p-object.xml
	dbus-binding-tool --mode=glib-client --output=$@ $<

check: all
	./server testing

clean:
	rm -rf server client *.h
