LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
        MediaSender.cpp                 \
        Parameters.cpp                  \
        rtp/RTPSender.cpp               \
        source/Converter.cpp            \
        source/MediaPuller.cpp          \
        source/PlaybackSession.cpp      \
        source/RepeaterSource.cpp       \
        source/TSPacketizer.cpp         \
        source/WifiDisplaySource.cpp    \
        source/VdinMediaSource.cpp      \
        VideoFormats.cpp                \

LOCAL_C_INCLUDES:= \
        $(TOP)/frameworks/av/media/libstagefright \
        $(TOP)/frameworks/native/include/media/openmax \
        $(TOP)/frameworks/av/media/libstagefright/mpeg2ts \

LOCAL_SHARED_LIBRARIES:= \
        libbinder                       \
        libhardware                     \
        libcutils                       \
        liblog                          \
        libgui                          \
        libmedia                        \
        libstagefright                  \
        libstagefright_foundation       \
        libui                           \
        libutils                        \

LOCAL_MODULE:= libstagefright_wfd

LOCAL_MODULE_TAGS:= optional

include $(BUILD_SHARED_LIBRARY)
################################################################################

include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
        wfd.cpp                 \

LOCAL_SHARED_LIBRARIES:= \
        libbinder                       \
        libgui                          \
        libmedia                        \
        libstagefright                  \
        libstagefright_foundation       \
        libstagefright_wfd              \
        libutils                        \
        libcutils                       \
        liblog                          \

LOCAL_MODULE:= wfd

LOCAL_MODULE_TAGS := optional

include $(BUILD_EXECUTABLE)

################################################################################
