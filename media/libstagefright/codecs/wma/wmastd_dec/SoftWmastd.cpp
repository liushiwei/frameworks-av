
#define LOG_TAG "SoftWmastd"
#include <utils/Log.h>

#include "SoftWmastd.h"

#include <media/stagefright/foundation/ADebug.h>
#include <media/stagefright/MediaDefs.h>
#include "wma_dec_api.h"
namespace android {

template<class T>
static void InitOMXParams(T *params) {
    params->nSize = sizeof(T);
    params->nVersion.s.nVersionMajor = 1;
    params->nVersion.s.nVersionMinor = 0;
    params->nVersion.s.nRevision = 0;
    params->nVersion.s.nStep = 0;
}

SoftWmastd::SoftWmastd(
        const char *name,
        const OMX_CALLBACKTYPE *callbacks,
        OMX_PTR appData,
        OMX_COMPONENTTYPE **component)
    : SimpleSoftOMXComponent(name, callbacks, appData, component)
{
	ALOGI("%s %d\n",__FUNCTION__,__LINE__);
	mInputBufferCount=0;
	mOutputPortSettingsChange=NONE;
    initPorts();
	initDecoder();
	init_flag=0;
	mAnchorTimeUs=0;
	
}

SoftWmastd::~SoftWmastd() 
{
     wma_dec_free(wmactx);
}

void SoftWmastd::initPorts() {
	ALOGI("%s %d\n",__FUNCTION__,__LINE__);
    OMX_PARAM_PORTDEFINITIONTYPE def;
    InitOMXParams(&def);

    def.nPortIndex = 0;
    def.eDir = OMX_DirInput;
    def.nBufferCountMin = kNumBuffers;
    def.nBufferCountActual = def.nBufferCountMin;
    def.nBufferSize = 64*1024/*8192*/;
    def.bEnabled = OMX_TRUE;
    def.bPopulated = OMX_FALSE;
    def.eDomain = OMX_PortDomainAudio;
    def.bBuffersContiguous = OMX_FALSE;
    def.nBufferAlignment = 1;

    def.format.audio.cMIMEType =
        const_cast<char *>(MEDIA_MIMETYPE_AUDIO_WMA);

    def.format.audio.pNativeRender = NULL;
    def.format.audio.bFlagErrorConcealment = OMX_FALSE;
    def.format.audio.eEncoding = OMX_AUDIO_CodingAAC;

    addPort(def);

    def.nPortIndex = 1;
    def.eDir = OMX_DirOutput;
    def.nBufferCountMin = kNumBuffers;
    def.nBufferCountActual = def.nBufferCountMin;
    def.nBufferSize = 128*1024;//kMaxNumSamplesPerBuffer * sizeof(int16_t);
    def.bEnabled = OMX_TRUE;
    def.bPopulated = OMX_FALSE;
    def.eDomain = OMX_PortDomainAudio;
    def.bBuffersContiguous = OMX_FALSE;
    def.nBufferAlignment = 2;

    def.format.audio.cMIMEType = const_cast<char *>("audio/raw");
    def.format.audio.pNativeRender = NULL;
    def.format.audio.bFlagErrorConcealment = OMX_FALSE;
    def.format.audio.eEncoding = OMX_AUDIO_CodingPCM;

    addPort(def);
}
void SoftWmastd::initDecoder() 
{
	//LOGI("TRACE:%s %d %s\n",__FUNCTION__,__LINE__,__FILE__);
	wmactx=wma_dec_init();
	if(wmactx == NULL){
        ALOGE("init wma decoder error!! ret=%d \n", WMA_DEC_ERR_INIT);
		//OMX_ErrorUndefined
    }
    //return OMX_ErrorNone;
}

OMX_ERRORTYPE SoftWmastd::internalGetParameter(OMX_INDEXTYPE index,OMX_PTR params)
{
	
    switch (index) {

        case OMX_IndexParamAudioPcm:
        {
			
            OMX_AUDIO_PARAM_PCMMODETYPE *pcmParams =
                (OMX_AUDIO_PARAM_PCMMODETYPE *)params;

            if (pcmParams->nPortIndex != 1) {
				ALOGE("Err:OMX_ErrorUndefined %s %d %s\n",__FUNCTION__,__LINE__);
                return OMX_ErrorUndefined;
            }

            pcmParams->eNumData = OMX_NumericalDataSigned;
            pcmParams->eEndian = OMX_EndianBig;
            pcmParams->bInterleaved = OMX_TRUE;
            pcmParams->nBitPerSample = 16;
            pcmParams->ePCMMode = OMX_AUDIO_PCMModeLinear;
            pcmParams->eChannelMapping[0] = OMX_AUDIO_ChannelLF;
            pcmParams->eChannelMapping[1] = OMX_AUDIO_ChannelRF;

            if (!isConfigured()) {
                pcmParams->nChannels = 2;
                pcmParams->nSamplingRate = 44100;
            } else {
                pcmParams->nChannels = (wfext.nChannels==1?2:wfext.nChannels);//mVi->channels;
                pcmParams->nSamplingRate = wfext.nSamplesPerSec;
            }
            return OMX_ErrorNone;
        }
        default:
            return SimpleSoftOMXComponent::internalGetParameter(index, params);
    }

}



OMX_ERRORTYPE SoftWmastd::internalSetParameter(OMX_INDEXTYPE index,const OMX_PTR params)
{
	
    switch (index) 
	{
        case OMX_IndexParamStandardComponentRole:
        {
            const OMX_PARAM_COMPONENTROLETYPE *roleParams =
                (const OMX_PARAM_COMPONENTROLETYPE *)params;

            if (strncmp((const char *)roleParams->cRole,
                        "audio_decoder.wma",
                        OMX_MAX_STRINGNAME_SIZE - 1)) {
                return OMX_ErrorUndefined;
            }

            return OMX_ErrorNone;
        }

        case OMX_IndexParamAudioAsf:
        {
            const OMX_AUDIO_PARAM_ASFTYPE *AsfParams =
                (const OMX_AUDIO_PARAM_ASFTYPE *)params;

            if (AsfParams->nPortIndex != 0) {
                return OMX_ErrorUndefined;
            }
            wfext.nSamplesPerSec =AsfParams->nSamplesPerSec;
			wfext.nChannels      =AsfParams->nChannels;
	        wfext.nBlockAlign    =AsfParams->nBlockAlign;
			wfext.wFormatTag     =AsfParams->wFormatTag;
			wfext.nAvgBytesPerSec=AsfParams->nAvgBitratePerSec>>3;
			wfext.extradata_size =AsfParams->extradata_size;
			wfext.extradata      =(char*)AsfParams->extradata;

			ALOGI("%s %d :samplerate =%d \n",__FUNCTION__,__LINE__,wfext.nSamplesPerSec);
	        ALOGI("%s %d :channels   =%d \n",__FUNCTION__,__LINE__,wfext.nChannels);
	        ALOGI("%s %d :bit_rate   =%d \n",__FUNCTION__,__LINE__,wfext.nAvgBytesPerSec);
	        ALOGI("%s %d :codec_tag  =%d \n",__FUNCTION__,__LINE__,wfext.wFormatTag);
	        ALOGI("%s %d :exdatsize  =%d \n",__FUNCTION__,__LINE__,wfext.extradata_size);
            ALOGI("%s %d :block_align=%d \n",__FUNCTION__,__LINE__,wfext.nBlockAlign);
	
			wma_dec_set_property(wmactx,WMA_DEC_Set_Wavfmt,(int)(&wfext));
			init_flag=1;
            return OMX_ErrorNone;
        }

        default:
            return SimpleSoftOMXComponent::internalSetParameter(index, params);
    }
}
bool SoftWmastd::isConfigured() const 
{
    return init_flag > 0;
}

static void dump_pcm_bin(char *path,char *buf,int size)
{
	FILE *fp=fopen(path,"ab+");
	 if(fp!= NULL){
		   fwrite(buf,1,size,fp);
		   fclose(fp);
	}
}

void SoftWmastd::onQueueFilled(OMX_U32 portIndex) 
{
    if (mOutputPortSettingsChange != NONE) {
        return;
    }
    List<BufferInfo *> &inQueue = getPortQueue(0);
    List<BufferInfo *> &outQueue = getPortQueue(1);
	int used,pktbuf_size,nb_sample,ret;
	uint8_t *pktbuf=NULL;//less than 64k
	
    while (!inQueue.empty() && !outQueue.empty()) {
        BufferInfo *inInfo = *inQueue.begin();
        OMX_BUFFERHEADERTYPE *inHeader = inInfo->mHeader;

        BufferInfo *outInfo = *outQueue.begin();
        OMX_BUFFERHEADERTYPE *outHeader = outInfo->mHeader;
        
        if (inHeader->nFlags & OMX_BUFFERFLAG_EOS) {
            inQueue.erase(inQueue.begin());
            inInfo->mOwnedByUs = false;
            notifyEmptyBufferDone(inHeader);

            outHeader->nFilledLen = 0;
            outHeader->nFlags = OMX_BUFFERFLAG_EOS;

            outQueue.erase(outQueue.begin());
            outInfo->mOwnedByUs = false;
            notifyFillBufferDone(outHeader);
            return;
        }

		if (inHeader->nOffset == 0) {
            mAnchorTimeUs = inHeader->nTimeStamp;
            //mNumFramesOutput = 0;
        }
		
        pktbuf = inHeader->pBuffer + inHeader->nOffset;
        pktbuf_size = inHeader->nFilledLen;
		//dump_pcm_bin("/data/indata.pcm",(char*)pktbuf,pktbuf_size);

		int used_cnt=0;
		nb_sample = 0;
		while(used_cnt<pktbuf_size)
		{
			ret=wma_dec_decode_frame(wmactx, pktbuf+used_cnt, pktbuf_size-used_cnt, 
				                          (short*)(outHeader->pBuffer+outHeader->nFilledLen), &used);

			//ALOGI("wma_dec_decode_frame done ret %d ,used %d,used_cnt %d,pktbuf_size %d\n",ret,used,used_cnt,pktbuf_size);
			used_cnt+=used;
			if(ret != WMA_DEC_ERR_NoErr){
				ALOGE("TRACE: wma decode error!! ret=%d, used=%d \n", ret, used);
				used_cnt = pktbuf_size;
			}else{
		    	wma_dec_get_property(wmactx,WMA_DEC_Get_Samples, &nb_sample);	
            	if (nb_sample > 0)
				{
					outHeader->nFilledLen += nb_sample* sizeof(int16_t);
                	outHeader->nOffset = 0;
                	outHeader->nFlags = 0;
					outHeader->nTimeStamp = mAnchorTimeUs+ 
					                    (nb_sample * 1000000ll)/wfext.nChannels/wfext.nSamplesPerSec;
					//dump_pcm_bin("/data/outdata.pcm",(char*)(outHeader->pBuffer),outHeader->nFilledLen);
           		 }
       		 }
			//ALOGI("pktbuf_size/%d  used_cnt/%d  used/%d nb_sample/%d \n",pktbuf_size,used_cnt,used,nb_sample);
		}
       
        inInfo->mOwnedByUs = false;
        inQueue.erase(inQueue.begin());
        inInfo = NULL;
        notifyEmptyBufferDone(inHeader);
        inHeader = NULL;

        outInfo->mOwnedByUs = false;
        outQueue.erase(outQueue.begin());
        outInfo = NULL;
        notifyFillBufferDone(outHeader);
        outHeader = NULL;
		
        ++mInputBufferCount;
    }
}

void SoftWmastd::onPortFlushCompleted(OMX_U32 portIndex) 
{
	ALOGI("%s %d\n",__FUNCTION__,__LINE__);
	wma_dec_reset(wmactx);
}

void SoftWmastd::onPortEnableCompleted(OMX_U32 portIndex, bool enabled) 
{
    if (portIndex != 1) {
        return;
    }

    switch (mOutputPortSettingsChange) {
        case NONE:
            break;

        case AWAITING_DISABLED:
        {
            CHECK(!enabled);
            mOutputPortSettingsChange = AWAITING_ENABLED;
            break;
        }

        default:
        {
            CHECK_EQ((int)mOutputPortSettingsChange, (int)AWAITING_ENABLED);
            CHECK(enabled);
            mOutputPortSettingsChange = NONE;
            break;
        }
	
    }
}


}  // namespace android
android::SoftOMXComponent *createSoftOMXComponent(
        const char *name, const OMX_CALLBACKTYPE *callbacks,
        OMX_PTR appData, OMX_COMPONENTTYPE **component) {
    ALOGI("%s %d \n",__FUNCTION__,__LINE__);
    return new android::SoftWmastd(name, callbacks, appData, component);
}
