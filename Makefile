SUBLIME = $(DESTDIR)/opt/sublime_text/

all: libsublime_text-xim-xcompose.so

libsublime_text-xim-xcompose.so: libsublime_text-xim-xcompose.c
	$(CC) -Wall -Werror -fPIC -shared -ldl `pkg-config gtk+-2.0 --cflags --libs` $^ -o $@

clean:
	rm -f *.so

install:
	install -d $(SUBLIME)
	install -m644 -t $(SUBLIME) libsublime_text-xim-xcompose.so
	install -m755 sublime_text $(SUBLIME)/sublime_text.wrapper

fix:
	install -m644 -t $(SUBLIME) libsublime_text-xim-xcompose.so
	mv $(SUBLIME)/sublime_text $(SUBLIME)/sublime_text.orig
	install -m755 -t $(SUBLIME) sublime_text

unfix:
	mv $(SUBLIME)/sublime_text.orig $(SUBLIME)/sublime_text
	rm -f $(SUBLIME)/libsublime_text-xim-xcompose.so
