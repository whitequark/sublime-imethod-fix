libsublime-immethod-fix.so: libsublime-immethod-fix.c
	$(CC) -Wall -Werror -fPIC -shared -ldl `pkg-config gtk+-2.0 --cflags --libs` $^ -o $@
