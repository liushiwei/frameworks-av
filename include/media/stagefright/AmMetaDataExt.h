/*
 * Copyright (C) 2012 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef AM_META_DATA_EXT_H_

#define AM_META_DATA_EXT_H_

#include <sys/types.h>

#include <stdint.h>

#include <utils/RefBase.h>
#include <utils/KeyedVector.h>
#include <utils/String8.h>

namespace android {

// The following keys map to int32_t data unless indicated otherwise.
enum {
    // DRM information to implement secure pipeline
    kKeyDrmID             = 'drID',  // raw data

    // kKeyFrameRate already exists, but we want a real number like 29.97.
    kKeyFrameRateQ16      = 'frQF',  // int32_t (video frame rate fps in Q16)
    // Key to store codec specific data.
    kKeyCodecSpecific     = 'cosp',  // raw data
    // Keys for WMA codec initialization parameters.
    kKeyWMABlockAlign     = 'blkA',  // int32_t
    kKeyWMABitsPerSample  = 'btSp',  // int32_t
    kKeyWMAFormatTag      = 'foTg',  // int32_t

    // Key to pass native window to OMX components.
    kKeyNativeWindow      = 'nawi',  // pointer

    // Key to pass CryptoInfo to player.
    kKeyCryptoInfo        = 'cryi',  // pointer

    // Key to adjust the timing of video/audio format change notification.
    kKeyFormatChangeTime = 'fctu',  // int64_t (usecs)

    // Keys to pass the original PTS and DTS to the decoder component.
    kKeyPTSFromContainer = 'ptsC',  // int64_t (usecs)
    kKeyDTSFromContainer = 'dtsC',  // int64_t (usecs)
    kKeyMediaTimeOffset  = 'mOfs',  // int64_t (usecs)

    // Keys to support various PCM formats.
    kKeyPCMBitsPerSample = 'Pbps',  // int32_t
    kKeyPCMDataEndian    = 'Pend',  // int32_t (OMX_ENDIANTYPE)
    kKeyPCMDataSigned    = 'Psgn',  // int32_t (OMX_NUMERICALDATATYPE)

};

}  // namespace android

#endif  // AM_META_DATA_EXT_H_
