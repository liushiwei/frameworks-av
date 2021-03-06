/*
 * utils for libavcodec
 * Copyright (c) 2001 Fabrice Bellard.
 * Copyright (c) 2003 Michel Bardiaux for the av_log API
 * Copyright (c) 2002-2004 Michael Niedermayer <michaelni@gmx.at>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
 
/**
 * @file utils.c
 * utils.
 */
 #if 0
#include <assert.h>//jacky 2006/10/18
#include "avformat.h"
#include "dsputil.h"
//#include "mpegvideo.h"
//#include <stdarg.h>
#include <string.h> 
#include <stdio.h>


char *av_strdup(const char *s)
{
    char *ptr;
    int len;
    len = strlen(s) + 1;
    ptr = av_malloc(len);
    if (!ptr)
        return NULL;
    memcpy(ptr, s, len);
    return ptr;
}

/**
 * realloc which does nothing if the block is large enough
 */


/* allocation of static arrays - do not use for normal allocation */
static unsigned int last_static = 0;
static char*** array_static = NULL;
static const unsigned int grow_static = 64; // ^2
void *__av_mallocz_static(void** location, unsigned int size)
{
    unsigned int l = (last_static + grow_static) & ~(grow_static - 1);
    void *ptr = av_mallocz(size);
    if (!ptr)
	return NULL;

    if (location)
    {
		if (l > last_static)
			array_static = av_realloc(array_static, l);
		array_static[last_static++] = (char**) location;
		*location = ptr;
    }
    return ptr;
}
/* free all static arrays and reset pointers to 0 */
void av_free_static(void)
{
    if (array_static)
    {
		unsigned i;
		for (i = 0; i < last_static; i++)
		{
			av_free(*array_static[i]);
			*array_static[i] = NULL;
		}
		av_free(array_static);
		array_static = 0;
    }
    last_static = 0;
}



/* encoder management */


void register_avcodec(AVCodec *format)
{
    AVCodec **p;
    p = &first_avcodec;
    while (*p != NULL) 
		p = &(*p)->next;
    *p = format;
    format->next = NULL;
}



#define INTERNAL_BUFFER_SIZE 32

#define ALIGN(x, a) (((x)+(a)-1)&~((a)-1))

void avcodec_align_dimensions(AVCodecContext *s, int *width, int *height)
{
    int w_align= 1;    
    int h_align= 1;    
    
    switch(s->pix_fmt){
    case PIX_FMT_YUV420P:
    case PIX_FMT_YUV422:
    case PIX_FMT_YUV422P:
    case PIX_FMT_YUV444P:
    case PIX_FMT_GRAY8:
    case PIX_FMT_YUVJ420P:
    case PIX_FMT_YUVJ422P:
    case PIX_FMT_YUVJ444P:
        w_align= 16; //FIXME check for non mpeg style codecs and use less alignment
        h_align= 16;
        break;
    case PIX_FMT_YUV411P:
        w_align=32;
        h_align=8;
        break;
    case PIX_FMT_YUV410P:
        if(s->codec_id == CODEC_ID_SVQ1){
            w_align=64;
            h_align=64;
        }
        break;
    default:
        w_align= 1;
        h_align= 1;
        break;
    }

    *width = ALIGN(*width , w_align);
    *height= ALIGN(*height, h_align);
}




/**
 * allocates a AVCodecContext and set it to defaults.
 * this can be deallocated by simply calling free() 
 */
AVCodecContext *avcodec_alloc_context(void)
{
    AVCodecContext *avctx= av_mallocz(sizeof(AVCodecContext));
    
    if(avctx==NULL) 
		return NULL;
    
    avcodec_get_context_defaults(avctx);
    
    return avctx;
}

/**
 * allocates a AVPFrame and set it to defaults.
 * this can be deallocated by simply calling free() 
 */
AVFrame *avcodec_alloc_frame(void)
{
    AVFrame *pic= av_mallocz(sizeof(AVFrame));
    
    return pic;
}


/** 
 * decode a frame. 
 * @param buf bitstream buffer, must be FF_INPUT_BUFFER_PADDING_SIZE larger then the actual read bytes
 * because some optimized bitstream readers read 32 or 64 bit at once and could read over the end
 * @param buf_size the size of the buffer in bytes
 * @param got_picture_ptr zero if no frame could be decompressed, Otherwise, it is non zero
 * @return -1 if error, otherwise return the number of
 * bytes used. 
 */
int avcodec_decode_video(AVCodecContext *avctx, AVFrame *picture, 
                         int *got_picture_ptr,
                         uint8_t *buf, int buf_size)
{
    int ret;
    
    ret = avctx->codec->decode(avctx, picture, got_picture_ptr, 
                               buf, buf_size);

    emms_c(); //needed to avoid a emms_c() call before every return;
    
    if (*got_picture_ptr)                           
        avctx->frame_number++;
    return ret;
}

/* decode an audio frame. return -1 if error, otherwise return the
   *number of bytes used. If no frame could be decompressed,
   *frame_size_ptr is zero. Otherwise, it is the decompressed frame
   *size in BYTES. */





AVCodec *avcodec_find_decoder_by_name(const char *name)
{
    AVCodec *p;
    p = first_avcodec;
    while (p)
	{
        if (p->decode != NULL && strcmp(name,p->name) == 0)
            return p;
        p = p->next;
    }
    return NULL;
}

AVCodec *avcodec_find(enum CodecID id)
{
    AVCodec *p;
    p = first_avcodec;
    while (p)
	{
        if (p->id == id)
            return p;
        p = p->next;
    }
    return NULL;
}

void avcodec_string(char *buf, int buf_size, AVCodecContext *enc, int encode)
{
    const char *codec_name;
    AVCodec *p;
    char buf1[32];
    char channels_str[100];
    int bitrate;

 
    p = avcodec_find_decoder(enc->codec_id);

    if (p) 
	{
        codec_name = p->name;
        if (!encode && enc->codec_id == CODEC_ID_MP3) 
		{
            if (enc->sub_id == 2)
                codec_name = "mp2";
            else if (enc->sub_id == 1)
                codec_name = "mp1";
        }
    } 
	else if (enc->codec_id == CODEC_ID_MPEG2TS) 
	{
        /* fake mpeg2 transport stream codec (currently not
           registered) */
        codec_name = "mpeg2ts";
    } 
	else if (enc->codec_name[0] != '\0') 
	{
        codec_name = enc->codec_name;
    } 
	else 
	{
        /* output avi tags */
        if (enc->codec_type == CODEC_TYPE_VIDEO) 
		{
            printf(buf1, sizeof(buf1), "%c%c%c%c", 
                     enc->codec_tag & 0xff,
                     (enc->codec_tag >> 8) & 0xff,
                     (enc->codec_tag >> 16) & 0xff,
                     (enc->codec_tag >> 24) & 0xff);
        }
		else 
		{
            printf(buf1, sizeof(buf1), "0x%04x", enc->codec_tag);
        }
        codec_name = buf1;
    }

    switch(enc->codec_type) 
	{
    case CODEC_TYPE_VIDEO:
        printf(buf, buf_size,
                 "Video: %s%s",
                 codec_name, enc->mb_decision ? " (hq)" : "");
        if (enc->codec_id == CODEC_ID_RAWVIDEO)
		{
     //       printf(buf + strlen(buf), buf_size - strlen(buf), //jacky 2006/10/24
     //                ", %s",
     //                avcodec_get_pix_fmt_name(enc->pix_fmt));
        }
        if (enc->width)
		{
            printf(buf + strlen(buf), buf_size - strlen(buf),
                     ", %dx%d, %0.2f fps",
                     enc->width, enc->height, 
                     (float)enc->frame_rate / enc->frame_rate_base);
        }
        if (encode) 
		{
            printf(buf + strlen(buf), buf_size - strlen(buf),
                     ", q=%d-%d", enc->qmin, enc->qmax);
        }
        bitrate = enc->bit_rate;
        break;
    case CODEC_TYPE_AUDIO:
        printf(buf, buf_size,
                 "Audio: %s",
                 codec_name);
        switch (enc->channels) 
		{
            case 1:
                strcpy(channels_str, "mono");
                break;
            case 2:
                strcpy(channels_str, "stereo");
                break;
            case 6:
                strcpy(channels_str, "5:1");
                break;
            default:
                sprintf(channels_str, "%d channels", enc->channels);
                break;
        }
        if (enc->sample_rate)
		{
            printf(buf + strlen(buf), buf_size - strlen(buf),
                     ", %d Hz, %s",
                     enc->sample_rate,
                     channels_str);
        }
        
        /* for PCM codecs, compute bitrate directly */
        switch(enc->codec_id)
		{
        case CODEC_ID_PCM_S16LE:
        case CODEC_ID_PCM_S16BE:
        case CODEC_ID_PCM_U16LE:
        case CODEC_ID_PCM_U16BE:
            bitrate = enc->sample_rate * enc->channels * 16;
            break;
        case CODEC_ID_PCM_S8:
        case CODEC_ID_PCM_U8:
        case CODEC_ID_PCM_ALAW:
        case CODEC_ID_PCM_MULAW:
            bitrate = enc->sample_rate * enc->channels * 8;
            break;
        default:
            bitrate = enc->bit_rate;
            break;
        }
        break;
    case CODEC_TYPE_DATA:
        printf(buf, buf_size, "Data: %s", codec_name);
        bitrate = enc->bit_rate;
        break;
    default:
        break;
		//av_abort(); //jacky 2006/10/18
    }
    if (encode)
	{
        if (enc->flags & CODEC_FLAG_PASS1)
            printf(buf + strlen(buf), buf_size - strlen(buf),
                     ", pass 1");
        if (enc->flags & CODEC_FLAG_PASS2)
            printf(buf + strlen(buf), buf_size - strlen(buf),
                     ", pass 2");
    }
    if (bitrate != 0) 
	{
        printf(buf + strlen(buf), buf_size - strlen(buf), 
                 ", %d kb/s", bitrate / 1000);
    }
}

unsigned avcodec_version( void )
{
  return LIBAVCODEC_VERSION_INT;
}

unsigned avcodec_build( void )
{
  return LIBAVCODEC_BUILD;
}

/* must be called before any other functions */
/*
void avcodec_init(void)
{
    static int inited = 0;

    if (inited != 0)
	return;
    inited = 1;

    dsputil_static_init();
}
*/
/**
 * Flush buffers, should be called when seeking or when swicthing to a different stream.
 */
void avcodec_flush_buffers(AVCodecContext *avctx)
{
    if(avctx->codec->flush)
        avctx->codec->flush(avctx);
}

void avcodec_default_free_buffers(AVCodecContext *s){
    int i, j;

    if(s->internal_buffer==NULL) return;
    
    for(i=0; i<INTERNAL_BUFFER_SIZE; i++)
	{
        InternalBuffer *buf= &((InternalBuffer*)s->internal_buffer)[i];
        for(j=0; j<4; j++)
		{
            av_freep(&buf->base[j]);
            buf->data[j]= NULL;
        }
    }
    av_freep(&s->internal_buffer);
    
    s->internal_buffer_count=0;
}

int64_t av_rescale(int64_t a, int b, int c){
    uint64_t h, l;
    assert(c > 0);
    assert(b >=0);
    
    if(a<0) return -av_rescale(-a, b, c);
    
    h= a>>32;
    if(h==0) return a*b/c;
    
    l= a&0xFFFFFFFF;
    l *= b;
    h *= b;

    l += (h%c)<<32;

    return ((h/c)<<32) + l/c;
}

/* av_log API */

#ifdef AV_LOG_TRAP_PRINTF
#undef stderr
#undef fprintf
#endif

static int av_log_level = AV_LOG_DEBUG;

static void av_log_default_callback(AVCodecContext* avctx, int level, const char* fmt, va_list vl)
{
    static int print_prefix=1;

    if(level>av_log_level)
	    return;
    //if(avctx && print_prefix)
      //  fprintf(stderr, "[%s @ %p]", avctx->codec ? avctx->codec->name : "?", avctx); //jacky 2006/10/18
        
    print_prefix= strstr(fmt, "\n") != NULL;
        
//    vfprintf(stderr, fmt, vl);  //jacky 2006/10/18
}

static void (*av_log_callback)(AVCodecContext*, int, const char*, va_list) = av_log_default_callback;

void av_log(AVCodecContext* avctx, int level, const char *fmt, ...)
{
    va_list vl;
    va_start(vl, fmt);
    av_vlog(avctx, level, fmt, vl);
    va_end(vl);
}

void av_vlog(AVCodecContext* avctx, int level, const char *fmt, va_list vl)
{
    av_log_callback(avctx, level, fmt, vl);
}

int av_log_get_level(void)
{
    return av_log_level;
}

void av_log_set_level(int level)
{
    av_log_level = level;
}

void av_log_set_callback(void (*callback)(AVCodecContext*, int, const char*, va_list))
{
    av_log_callback = callback;
}
#endif
