#include <sys/types.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <glib.h>
#include <glib/gi18n.h>
#include <glib-object.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

#include <libbpgdec.h>

static void free_buffer(guchar *pixels,gpointer data){g_free (pixels);}

static GdkPixbuf *scale_pixbuf(GdkPixbuf *source,gint dest_width,gint dest_height){
  gdouble wratio;
  gdouble hratio;
  gint    source_width;
  gint    source_height;
  source_width=gdk_pixbuf_get_width(source);
  source_height=gdk_pixbuf_get_height(source);
  if (source_width<=dest_width && source_height<=dest_height) return g_object_ref(source);
  wratio=(gdouble)source_width/(gdouble)dest_width;
  hratio=(gdouble)source_height/(gdouble)dest_height;
  if(hratio>wratio) dest_width=rint(source_width/hratio);
  else dest_height=rint(source_height/wratio);
  return gdk_pixbuf_scale_simple(source,MAX(dest_width,1),MAX(dest_height,1),GDK_INTERP_HYPER);
}

static GdkPixbuf *gdkpixbuf_from_bpg(const char *filename){
  int width=0,height=0;
  guchar *pixdata=(guchar *)bpg_to_rgb(filename,&width,&height);
  if(pixdata==NULL) return NULL;
  GdkPixbuf *pixbuf=gdk_pixbuf_new_from_data(pixdata,GDK_COLORSPACE_RGB,0,8,width,height,width*3,free_buffer,NULL);
  return pixbuf;
}

static void bpg_thumbnail_create(const char *ifilename,const char *ofilename,int size){
  GdkPixbuf *pixbuf=gdkpixbuf_from_bpg(ifilename);
  if(pixbuf!=NULL){
    GdkPixbuf *scaled=scale_pixbuf(pixbuf,(gint)size,(gint)size);
    g_object_unref(pixbuf);
    pixbuf=scaled;
    gdk_pixbuf_save(pixbuf,ofilename,"png",NULL,NULL);
    g_object_unref(pixbuf);
  }
}

int main(int argc, char **argv){
    if(argc<4) exit(1);
    bpg_thumbnail_create(argv[1],argv[2],atoi(argv[3]));
    return 0;
}
