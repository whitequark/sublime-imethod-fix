DESTDIR = /opt/sublime_text/

all: libsublime-immethod-fix.so

libsublime-immethod-fix.so: libsublime-immethod-fix.c
	$(CC) -Wall -Werror -fPIC -shared -ldl `pkg-config gtk+-2.0 --cflags --libs` $^ -o $@

clean:
	rm -f *.so

install:
	install -m644 -t $(DESTDIR) libsublime-immethod-fix.so

fix: install
	mv $(DESTDIR)/sublime_text $(DESTDIR)/sublime_text.orig
	install -m755 -t $(DESTDIR) sublime_text

unfix:
	mv $(DESTDIR)/sublime_text.orig $(DESTDIR)/sublime_text
	rm -f $(DESTDIR)/libsublime-immethod-fix.so
