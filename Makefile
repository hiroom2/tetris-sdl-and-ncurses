all:
	$(MAKE) -C jni/src

install:
	install -d -m755  $(DESTDIR)/bin/
	install -m755 jni/src/sdl $(DESTDIR)/bin/
	install -m755 jni/src/ncurses $(DESTDIR)/bin/ncurses

clean:
	$(MAKE) -C jni/src clean
