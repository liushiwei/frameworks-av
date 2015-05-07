#ifndef __WMAAPI_H__
#define __WMAAPI_H__


#ifdef __cplusplus
extern "C" {
#endif

#define MAX_OUT_BUF_SIZE (128*1024) //128k
#define PACKAGE "wmapro" /* Name of package */
#define VERSION "0.1.0" /* Version number of package */

/*
enum {
    CODEC_ID_WMAV1=0x160,
    CODEC_ID_WMAV2=0x161,

};
*/
enum LyxWmaError {
    WMAPRO_ERR_NoErr = 0,
    WMAPRO_ERR_GetProperty = -1000,
    WMAPRO_ERR_SetProperty,
    WMAPRO_ERR_Init,
    WMAPRO_ERR_Sync,
    WMAPRO_ERR_InvalidData,
    WMAPRO_ERR_InvalidData1,
    WMAPRO_ERR_,
};

enum LyxWmaProperty {
    WMAPRO_Get_Samplerate = 0,
    WMAPRO_Get_Samples,
    WMAPRO_Get_FrameLength,
    WMAPRO_Get_Version,
    
    WMAPRO_Set_MemCallback = 100,
    WMAPRO_Set_WavFormat,
    WMAPRO_Set_Sysfun,
};

/**
 * all in native-endian format
 */
enum SampleFormat {
    SAMPLE_FMT_NONE = -1,
    SAMPLE_FMT_U8,              ///< unsigned 8 bits
    SAMPLE_FMT_S16,             ///< signed 16 bits
    SAMPLE_FMT_S32,             ///< signed 32 bits
    SAMPLE_FMT_FLT,             ///< float
    SAMPLE_FMT_DBL,             ///< double
    SAMPLE_FMT_NB               ///< Number of sample formats. DO NOT USE if dynamically linking to libavcodec
};

/* waveformatex fields specified in Microsoft documentation:
   http://msdn2.microsoft.com/en-us/library/ms713497.aspx */
typedef struct waveformat{
	unsigned short wFormatTag;
	unsigned short nChannels;
	unsigned int nSamplesPerSec;
	unsigned int nAvgBytesPerSec;
	unsigned short nBlockAlign;
	unsigned short wBitsPerSample;
	unsigned short extradata_size;
	unsigned char  *extradata;
}waveformat_t;

typedef struct sysfuncb_s
{
	 void *psys_malloc;
	 void *psys_realloc;
	 void *psys_free;
	 void *psys_memcpy;
	 void *psys_memset;
	 void *psys_memmove;
	 void *psys_qsort;
}psysfuncb_t;

typedef struct tagAudioContext 
{
    /* Codec Context */
    int block_align;
    int nb_packets;
    int frame_number;
    int sample_rate;
    int channels;
    int bit_rate;
    int flags;
    int sample_fmt;
    unsigned int channel_layout;

    /*codec extradata*/
    int extradata_size;
    unsigned char *extradata;    

    int output_size;
    int max_output_size;
    int codec_id;
    void *priv_data;
} AudioContext;


/*
 * �ú���������ɶ�wmapro�������ĳ�ʼ��
 * avctx ָ��LyxAudioContext�ṹ��, ��Ҫ��0, ����Ҫ��ʼ���������codec_id, sample_rate, channels,
 * bit_rate, block_align, extradata_size��extradata, ��Щ��Ϣ�����Դ�demux��ȡ�õ�.
 * 
 * ����ֵ:  WMAPRO_ERR_NoErr    �ɹ�
 *          ����                ʧ��
 */
AudioContext * wmapro_dec_init();


/*
 * �ú����ǽ���������
 * avctx ָ��LyxAudioContext�ṹ��
 * 
 * inbuf ָ�����뻺����
 * input_size ��ʾinbuf�Ĵ�С,��λ���ֽ�, ��С����Ϊblock_align,���򲻻�����֡,
 *            һ���demuxȡ�õ�asf�е�һ��packet�ﶼ����������block_align������
 * outbuf ָ�����������, ҪԤ�ȷ����, ��С���ݲ�ͬ���ļ����в���,��󲻳���128k�ֽ�
 * used ����ʹ����inbuf�Ĵ�С, ֻҪ�������붼�᷵����block_alignһ����ֵ, 
 *      ����������ֵ�ҷ�����ȷ,˵��input_size̫С,��������
 * ����ֵ:  WMAPRO_ERR_NoErr    ��ȷ���,û�г���
 *          ����                ����
 */
int wmapro_dec_decode_frame(AudioContext *avctx, unsigned char *inbuf, int input_size, signed short *outbuf, int *used);


/*
 * �ú��������ͷŽ��뺯����������ڴ�
 * avctx ָ��LyxAudioContext�ṹ��
 */
int wmapro_dec_free(AudioContext *avctx);


/*
 * �ú�������ȡ��һЩ����ֵ
 * avctx ָ��LyxAudioContext�ṹ��
 * property WMAPRO_Get_Samples      ȡ�������˶��ٸ�����, ��λsizeof(short),������decode��ȡ��outbuf�Ĵ�С
 *
 * ����ֵ:  WMAPRO_ERR_NoErr       ��ȷ����
 *          WMAPRO_ERR_GetProperty ����
 */
int wmapro_dec_get_property(AudioContext *avctx, int property, int *value);




int wmapro_dec_set_property(AudioContext *avctx,int property,int value);


/*
 * �ú������������¶�λ��Խ��������и�λ
 * avctx ָ��LyxAudioContext�ṹ��
 */
void wmapro_dec_reset(AudioContext *avctx);

#ifdef __cplusplus
}
#endif


#endif

