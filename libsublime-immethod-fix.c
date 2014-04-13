#define _GNU_SOURCE
// #define VERBOSE

#include <assert.h>
#include <dlfcn.h>
#include <stdio.h>
#include <string.h>
#include <gtk/gtk.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#ifdef VERBOSE
#define DEBUG(fmt, ...) do { \
    FILE* err = fopen("/tmp/libsublime-immethod-fix.log", "a"); \
    if (err) { \
      fprintf(err, fmt, __VA_ARGS__); \
      fclose(err); \
    } \
  } while(0)
#else
#define DEBUG(fmt, ...)
#endif

/* See gtkimcontextxim.c */
GType gtk_type_im_context_xim = 0;

#define GTK_TYPE_IM_CONTEXT_XIM            (gtk_type_im_context_xim)
#define GTK_IM_CONTEXT_XIM(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GTK_TYPE_IM_CONTEXT_XIM, GtkIMContextXIM))
#define GTK_IM_CONTEXT_XIM_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), GTK_TYPE_IM_CONTEXT_XIM, GtkIMContextXIMClass))
#define GTK_IS_IM_CONTEXT_XIM(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTK_TYPE_IM_CONTEXT_XIM))
#define GTK_IS_IM_CONTEXT_XIM_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GTK_TYPE_IM_CONTEXT_XIM))
#define GTK_IM_CONTEXT_XIM_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), GTK_TYPE_IM_CONTEXT_XIM, GtkIMContextXIMClass))

typedef struct _GtkIMContextXIM       GtkIMContextXIM;
typedef struct _GtkIMContextXIMClass  GtkIMContextXIMClass;

struct _GtkIMContextXIMClass
{
  GtkIMContextClass parent_class;
};

typedef struct _StatusWindow StatusWindow;
typedef struct _GtkXIMInfo GtkXIMInfo;

struct _GtkIMContextXIM
{
  GtkIMContext object;

  GtkXIMInfo *im_info;

  gchar *locale;
  gchar *mb_charset;

  GdkWindow *client_window;
  GtkWidget *client_widget;

  /* The status window for this input context; we claim the
   * status window when we are focused and have created an XIC
   */
  StatusWindow *status_window;

  gint preedit_size;
  gint preedit_length;
  gunichar *preedit_chars;
  XIMFeedback *feedbacks;

  gint preedit_cursor;

  XIMCallback preedit_start_callback;
  XIMCallback preedit_done_callback;
  XIMCallback preedit_draw_callback;
  XIMCallback preedit_caret_callback;

  XIMCallback status_start_callback;
  XIMCallback status_done_callback;
  XIMCallback status_draw_callback;

  XIMCallback string_conversion_callback;

  XIC ic;

  guint filter_key_release : 1;
  guint use_preedit : 1;
  guint finalizing : 1;
  guint in_toplevel : 1;
  guint has_focus : 1;
};

static GClassInitFunc orig_gtk_im_context_xim_class_init;
static GType (*orig_g_type_module_register_type)(GTypeModule *,
                                                 GType, const gchar *,
                                                 const GTypeInfo *, GTypeFlags);
static gboolean (*orig_gtk_im_context_xim_filter_keypress)(GtkIMContext *context,
                                                           GdkEventKey *event);

static gboolean
hook_gtk_im_context_xim_filter_keypress(GtkIMContext *context, GdkEventKey *event) {
  GtkIMContextXIM *im_context_xim = GTK_IM_CONTEXT_XIM(context);
  if (!im_context_xim->client_window) {
    DEBUG("im_context_xim == %p\n", im_context_xim);
    DEBUG("event->window == %p\n", event->window);

    gtk_im_context_set_client_window(context, event->window);
  }

  return orig_gtk_im_context_xim_filter_keypress(context, event);
}

static void
hook_gtk_im_context_xim_class_init (GtkIMContextXIMClass *class) {
  orig_gtk_im_context_xim_class_init(class, NULL); /* wat? */

  GtkIMContextClass *im_context_class = GTK_IM_CONTEXT_CLASS (class);

  assert(!orig_gtk_im_context_xim_filter_keypress);
  orig_gtk_im_context_xim_filter_keypress = im_context_class->filter_keypress;
  im_context_class->filter_keypress = hook_gtk_im_context_xim_filter_keypress;
  DEBUG("orig_gtk_im_context_xim_filter_keypress: %p\n",
        orig_gtk_im_context_xim_filter_keypress);
}

GType
g_type_module_register_type (GTypeModule *module,
                             GType parent_type,
                             const gchar *type_name,
                             const GTypeInfo *type_info,
                             GTypeFlags flags) {
  if (!orig_g_type_module_register_type) {
    orig_g_type_module_register_type = dlsym(RTLD_NEXT, "g_type_module_register_type");
    assert(orig_g_type_module_register_type);
  }

  if (type_name && !strcmp(type_name, "GtkIMContextXIM")) {
    assert(!orig_gtk_im_context_xim_class_init);
    orig_gtk_im_context_xim_class_init = type_info->class_init;

    assert(sizeof(GtkIMContextXIM) == type_info->instance_size);

    const GTypeInfo hook_im_context_xim_info =
    {
      type_info->class_size,
      type_info->base_init,
      type_info->base_finalize,
      (GClassInitFunc) hook_gtk_im_context_xim_class_init,
      type_info->class_finalize,
      type_info->class_data,
      type_info->instance_size,
      type_info->n_preallocs,
      type_info->instance_init,
    };

    DEBUG("orig_gtk_im_context_xim_class_init: %p\n", orig_gtk_im_context_xim_class_init);

    gtk_type_im_context_xim =
      orig_g_type_module_register_type(module, parent_type, type_name,
                                       &hook_im_context_xim_info, flags);

    return gtk_type_im_context_xim;
  }

  return orig_g_type_module_register_type(module, parent_type, type_name, type_info, flags);
}
