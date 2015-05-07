#if 0
#ifndef AVIO_H
#define AVIO_H
#include <stdio.h>
/* output byte stream handling */

typedef int64_t offset_t;

/* unbuffered I/O */





#define URL_RDONLY 0
#define URL_WRONLY 1
#define URL_RDWR   2

typedef int URLInterruptCB(void);

/*int url_open(URLContext **h, const char *filename, int flags);*/
int url_read(URLContext *h, unsigned char *buf, int size);
int url_write(URLContext *h, unsigned char *buf, int size);
offset_t url_seek(URLContext *h, offset_t pos, int whence);
int url_close(URLContext *h);
//int url_exist(const char *filename);
offset_t url_filesize(URLContext *h);
int url_get_max_packet_size(URLContext *h);
//void url_get_filename(URLContext *h, char *buf, int buf_size);

/* the callback is called in blocking functions to test regulary if
   asynchronous interruption is needed. -EINTR is returned in this
   case by the interrupted function. 'NULL' means no interrupt
   callback is given. */






//extern URLProtocol *first_protocol;


int register_protocol(URLProtocol *protocol);

int init_put_byte(ByteIOContext *s,
                  unsigned char *buffer,
                  int buffer_size,
                  int write_flag,
                  void *opaque,
                  int (*read_packet)(void *opaque, uint8_t *buf, int buf_size),
                  void (*write_packet)(void *opaque, uint8_t *buf, int buf_size),
                  int (*seek)(void *opaque, offset_t offset, int whence));

void put_byte(ByteIOContext *s, int b);
void put_buffer(ByteIOContext *s, const unsigned char *buf, int size);
void put_le64(ByteIOContext *s, uint64_t val);
void put_be64(ByteIOContext *s, uint64_t val);
void put_le32(ByteIOContext *s, unsigned int val);
void put_be32(ByteIOContext *s, unsigned int val);
void put_le16(ByteIOContext *s, unsigned int val);
void put_be16(ByteIOContext *s, unsigned int val);
void put_tag(ByteIOContext *s, const char *tag);

//void put_be64_double(ByteIOContext *s, double val);
void put_strz(ByteIOContext *s, const char *buf);

offset_t url_fseek(ByteIOContext *s, offset_t offset, int whence);
void url_fskip(ByteIOContext *s, offset_t offset);
offset_t url_ftell(ByteIOContext *s);
int url_feof(ByteIOContext *s);

#define URL_EOF (-1)
int url_fgetc(ByteIOContext *s);
#ifdef __GNUC__
int url_fprintf(ByteIOContext *s, const char *fmt, ...) __attribute__ ((__format__ (__printf__, 2, 3)));
#else
int url_fprintf(ByteIOContext *s, const char *fmt, ...);
#endif
char *url_fgets(ByteIOContext *s, char *buf, int buf_size);

void put_flush_packet(ByteIOContext *s);

int get_buffer(ByteIOContext *s, unsigned char *buf, int size);
int get_byte(ByteIOContext *s);
unsigned int get_le32(ByteIOContext *s);
uint64_t get_le64(ByteIOContext *s);
unsigned int get_le16(ByteIOContext *s);

//////by ren//////double get_be64_double(ByteIOContext *s);
char *get_strz(ByteIOContext *s, char *buf, int maxlen);
unsigned int get_be16(ByteIOContext *s);
unsigned int get_be32(ByteIOContext *s);
uint64_t get_be64(ByteIOContext *s);

//static inline int url_is_streamed(ByteIOContext *s)

static __inline int url_is_streamed(ByteIOContext *s) //jacky 2006/10/18
{
    return s->is_streamed;
}

int url_fdopen(ByteIOContext *s, URLContext *h);
int url_setbufsize(ByteIOContext *s, int buf_size);
int url_fopen(ByteIOContext *s, const char *filename, int flags);
int url_fclose(ByteIOContext *s);
URLContext *url_fileno(ByteIOContext *s);
int url_fget_max_packet_size(ByteIOContext *s);

int url_open_buf(ByteIOContext *s, uint8_t *buf, int buf_size, int flags);
int url_close_buf(ByteIOContext *s);

int url_open_dyn_buf(ByteIOContext *s);
int url_open_dyn_packet_buf(ByteIOContext *s, int max_packet_size);
int url_close_dyn_buf(ByteIOContext *s, uint8_t **pbuffer);

/* file.c */
//extern URLProtocol file_protocol;
//extern URLProtocol pipe_protocol;



#endif
#endif
