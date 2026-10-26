LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

ANDROID := 1

LOCAL_MODULE := main

# Paths

SRB2_PATH := ../../../..

SRC_JNI := .
SRC_MAIN := $(SRB2_PATH)/src

SRC_HWR := $(SRC_MAIN)/hardware/
SRC_SDL := $(SRC_MAIN)/sdl/
SRC_MINIUPNPC := $(SRB2_PATH)/libs/miniupnpc

SRC_APK := $(SRC_MAIN)/android/
SRC_XTRA := $(SRC_MAIN)/xtra/

ifeq ($(OS),Windows_NT)
WINDOWSHELL=1
endif

MAKE_DIR := $(LOCAL_PATH)/$(SRC_MAIN)/Makefile.d
XTRA_MAKE_DIR := $(LOCAL_PATH)/$(SRC_XTRA)/Makefile.d

# Compile flags

# SRB2's Android hardware renderer uses OpenGL ES 1.1 fixed-function calls.
LOCAL_CFLAGS += -DUNIXCOMMON -DLINUX \
				-DHAVE_SDL -DHAVE_MIXER -DHAVE_MIXERX \
				-DHWRENDER -DNOTHAVE_GLES -DNOTHAVE_GLES2 \
				-DTOUCHINPUTS -DNATIVESCREENRES -DDIRECTFULLSCREEN \
				-DHAVE_ZLIB -DHAVE_PNG -DHAVE_CURL \
				-DHAVE_MINIUPNPC -DMINIUPNP_STATICLIB \
				-DMINIUPNPC_SET_SOCKET_TIMEOUT -D_BSD_SOURCE -D_DEFAULT_SOURCE -D_XOPEN_SOURCE=600 \
				-DHAVE_WHANDLE -DHAVE_THREADS -DLOGCAT -DCOMPVERSION \
				-DNONX86 -DNOASM -DNOMUMBLE \
				-DLUA_ANDROID

# Includes

include $(MAKE_DIR)/platform.mk
include $(MAKE_DIR)/util.mk

LOCAL_C_INCLUDES += $(LOCAL_PATH) \
	$(LOCAL_PATH)/$(SRC_MINIUPNPC) \
	$(LOCAL_PATH)/$(SRC_MINIUPNPC)/include \
	$(LOCAL_PATH)/$(SRC_MINIUPNPC)/include/miniupnpc \
	$(LOCAL_PATH)/$(SRC_MINIUPNPC)/src

# Source files

LOCAL_SRC_FILES := $(call List,$(LOCAL_PATH)/$(SRC_JNI)/Sourcefile)
# LOCAL_SRC_FILES += $(call List,$(LOCAL_PATH)/$(SRC_APK)/Sourcefile)
# LOCAL_SRC_FILES += $(call List,$(LOCAL_PATH)/$(SRC_XTRA)/Sourcefile)
LOCAL_SRC_FILES += $(call List,$(LOCAL_PATH)/$(SRC_MAIN)/Sourcefile)
LOCAL_SRC_FILES += $(call List,$(LOCAL_PATH)/$(SRC_MAIN)/blua/Sourcefile)
LOCAL_SRC_FILES += $(call List,$(LOCAL_PATH)/$(SRC_MAIN)/netcode/Sourcefile)
LOCAL_SRC_FILES += $(call List,$(LOCAL_PATH)/$(SRC_HWR)/Sourcefile)
LOCAL_SRC_FILES += $(call List,$(LOCAL_PATH)/$(SRC_SDL)/Sourcefile)

LOCAL_SRC_FILES += $(SRC_SDL)/SDL_main/SDL_android_main.c $(SRC_SDL)/mixer_sound.c $(SRC_SDL)/i_threads.c
# ogl_sdl.c is normally supplied by the desktop makefile. The Android NDK
# build consumes Sourcefile lists directly, so include the SDL GL bridge here.
LOCAL_SRC_FILES += $(SRC_SDL)/ogl_sdl.c
LOCAL_SRC_FILES += $(SRC_MAIN)/comptime.c $(SRC_MAIN)/md5.c
LOCAL_SRC_FILES += $(SRC_MINIUPNPC)/src/igd_desc_parse.c \
	$(SRC_MINIUPNPC)/src/miniupnpc.c \
	$(SRC_MINIUPNPC)/src/minixml.c \
	$(SRC_MINIUPNPC)/src/minisoap.c \
	$(SRC_MINIUPNPC)/src/minissdpc.c \
	$(SRC_MINIUPNPC)/src/miniwget.c \
	$(SRC_MINIUPNPC)/src/upnpcommands.c \
	$(SRC_MINIUPNPC)/src/upnpdev.c \
	$(SRC_MINIUPNPC)/src/upnpreplyparse.c \
	$(SRC_MINIUPNPC)/src/upnperrors.c \
	$(SRC_MINIUPNPC)/src/connecthostport.c \
	$(SRC_MINIUPNPC)/src/portlistingparse.c \
	$(SRC_MINIUPNPC)/src/receivedata.c \
	$(SRC_MINIUPNPC)/src/addr_is_reserved.c

# Libraries

LOCAL_SHARED_LIBRARIES := SDL2 hidapi \
	SDL2_mixer libmpg123 \
	libpng #libgme
LOCAL_STATIC_LIBRARIES := libcurl
LOCAL_LDLIBS := -lGLESv1_CM -lEGL -llog -lz

include $(BUILD_SHARED_LIBRARY)
