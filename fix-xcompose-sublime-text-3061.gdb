# Works *only* on Sublime Text build 3061 for 64-bit Linux.
# Run as: $ GTK_IM_MODULE=xim gdb -script fix-xcompose-sublime-text-3061.gdb
file /opt/sublime_text/sublime_text
set follow-fork-mode child
set detach-on-fork off
run
inferior 2
set follow-fork-mode parent
set detach-on-fork on
b *0x5b3267
c
del 1
set $multicontext = (GtkIMMulticontext*) $r13
set $window = (GtkWindow*) $rbx
b gtk_widget_show if widget==$window
c
fin
del 2
call gtk_im_context_set_client_window($multicontext,$window->bin.container.widget.window)
detach inferiors 1 2
quit
