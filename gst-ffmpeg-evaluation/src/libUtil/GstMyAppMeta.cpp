
#include "GstMyAppMeta.h"

//using namespace ngv;

GType gst_my_app_meta_api_get_type()
{
    static volatile GType type;
    static const gchar *tags[] = {"this", "is", "my_app", NULL};
    if (g_once_init_enter(&type))
    {
        GType _type = gst_meta_api_type_register("GstMyAppMetaAPI", tags);
        g_once_init_leave(&type, _type);
    }
    //g_print("gst_my_app_meta_api_get_type() called: type=%d\n", type);
    return type;
}

static gboolean
gst_my_app_meta_init(GstMeta *meta, gpointer params, GstBuffer *buffer)
{
    GstMyAppMeta *xmeta = (GstMyAppMeta *)meta;
    xmeta->bufSize = 0;
    xmeta->buf = NULL;

    //g_print("gst_my_app_meta_init() called");

    return TRUE;
}

static gboolean
gst_my_app_meta_transform(GstBuffer *transbuf, GstMeta *meta, GstBuffer *buffer, GQuark type, gpointer data)
{
    GstMyAppMeta *emeta = (GstMyAppMeta *)meta;
    /* we always copy no matter what transform */
    gst_buffer_add_my_app_meta_full(transbuf, emeta->bufSize, emeta->buf);

    //g_print("gst_my_app_meta_transform() called");
    return TRUE;
}

static void
gst_my_app_meta_free(GstMeta *meta, GstBuffer *buffer)
{
    GstMyAppMeta *emeta = (GstMyAppMeta *)meta;
    g_free(emeta->buf);
    emeta->buf = NULL;
}

const GstMetaInfo *
gst_my_app_meta_get_info()
{
    static const GstMetaInfo *meta_info = NULL;
    if (g_once_init_enter(&meta_info))
    {
        const GstMetaInfo *mi = gst_meta_register(GST_MY_APP_META_API_TYPE,
                                                  "GstMyAppMeta",
                                                  sizeof(GstMyAppMeta),
                                                  gst_my_app_meta_init,
                                                  gst_my_app_meta_free,
                                                  gst_my_app_meta_transform);
        g_once_init_leave(&meta_info, mi);
    }

    //g_print( "gst_my_app_meta_get_info() called: meta_info(api=%d,type=%d,size=%d)\n", meta_info->api, meta_info->type, meta_info->size);

    return meta_info;
}

GstMyAppMeta *
gst_buffer_add_my_app_meta_full(GstBuffer *buffer, gint bufSize, gchar *buf)
{
    GstMyAppMeta *meta;
    g_return_val_if_fail(GST_IS_BUFFER(buffer), NULL);
    meta = (GstMyAppMeta *)gst_buffer_add_meta(buffer, GST_MY_APP_META_INFO, NULL);
    meta->bufSize = bufSize;
    meta->buf = g_strdup(buf);
    return meta;
}