## Makefile.am for the submodule 'Renderer' of the 'Audio' module 
## of the OSDL library.


# Convenience library for the 'Renderer' submodule :
noinst_LTLIBRARIES = libOSDLAudioRenderer.la

libOSDLAudioRenderer_la_SOURCES = $(AUDIO_RENDERER_INTERFACES) $(AUDIO_RENDERER_IMPLEMENTATIONS)

libOSDLAudioRenderer_la_CPPFLAGS = -I@OSDL_BASIC@ -I@OSDL_AUDIO@ -I@OSDL_EVENTS@ -I@OSDL_ENGINE@

libOSDLAudioRendererIncludedir      = $(includedir)/OSDL
libOSDLAudioRendererInclude_HEADERS = $(AUDIO_RENDERER_INTERFACES)


AUDIO_RENDERER_INTERFACES = \
	OSDLAudioRenderer.h                   \
	OSDLAudioRendererIncludes.h


AUDIO_RENDERER_IMPLEMENTATIONS = \
	OSDLAudioRenderer.cc
	
	
