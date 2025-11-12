//ref:
//https://lifestyletransfer.com/how-to-add-metadata-to-gstreamer-buffer-in-python/

#ifndef __GST_MY_APP_META_H__
#define __GST_MY_APP_META_H__

#include "DataTypes.h"
#include "AppLog.h"
#include <gst/gst.h>

#define GST_MY_APP_META_INFO        (gst_my_app_meta_get_info())
#define GST_MY_APP_META_API_TYPE    (gst_my_app_meta_api_get_type())

typedef struct _GstMyAppMeta GstMyAppMeta;

struct _GstMyAppMeta
{
    GstMeta meta;

    gint bufSize;
    gchar *buf;
};



#define gst_buffer_add_my_app_meta(b)  ((GstMyAppMeta *)gst_buffer_add_meta((b), GST_MY_APP_META_INFO, NULL))
#define gst_buffer_get_my_app_meta(b)  ((GstMyAppMeta *)gst_buffer_get_meta((b), GST_MY_APP_META_API_TYPE))


GType gst_my_app_meta_api_get_type();

/* implementation */
const GstMetaInfo *gst_my_app_meta_get_info();

GstMyAppMeta *gst_buffer_add_my_app_meta_full(GstBuffer *buffer, gint bufSize, gchar *buf);

#endif