## Makefile.am for the 'Audio' module of the OSDL library.


SUBDIRS = audioRenderer


# Convenience library for the 'Audio' module:
noinst_LTLIBRARIES = libOSDLAudio.la

libOSDLAudio_la_SOURCES = $(AUDIO_INTERFACES) $(AUDIO_IMPLEMENTATIONS)

libOSDLAudio_la_CPPFLAGS = -I@OSDL_BASIC@

libOSDLAudioIncludedir      = $(includedir)/OSDL
libOSDLAudioInclude_HEADERS = $(AUDIO_INTERFACES)


AUDIO_INTERFACES = \
	OSDLAudible.h                        \
	OSDLAudio.h                          \
	OSDLAudioChannel.h                   \
	OSDLAudioCommon.h                    \
	OSDLMusic.h                          \
	OSDLMusicManager.h                   \
	OSDLSound.h                          \
	OSDLAudioIncludes.h


AUDIO_IMPLEMENTATIONS = \
	OSDLAudible.cc                       \
	OSDLAudio.cc                         \
	OSDLAudioChannel.cc                  \
	OSDLMusic.cc                         \
	OSDLMusicManager.cc                  \
	OSDLSound.cc
	
