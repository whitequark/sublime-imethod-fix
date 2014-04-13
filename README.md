Sublime XIM bugfix
==================

This library fixes an annoying bug in [Sublime Text][st] where it ignores
[XCompose][] key sequences.

[st]: http://sublimetext.com
[XCompose]: https://wiki.edubuntu.org/ComposeKey

Building
--------

Header files for X11 and Gtk2 are required, e.g. packages `libx11-dev libgtk2.0-dev` on Debian.

    $ make

Installation
------------

    $ sudo make fix

Usage
-----

Make sure your GTK input method is set to `xim`, e.g. with `export GTK_IM_METHOD=xim`.

    $ subl

Uninstallation
--------------

    $ sudo make unfix

License
-------

[MIT](LICENSE.txt). This library is not derived from Sublime Text.
