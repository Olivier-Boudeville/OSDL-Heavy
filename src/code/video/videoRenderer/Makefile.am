## Makefile.am for the submodule 'Renderer' of the 'Video' module 
## of the OSDL library.


# Convenience library for the 'Renderer' submodule :
noinst_LTLIBRARIES = libOSDLVideoRenderer.la

libOSDLVideoRenderer_la_SOURCES = $(VIDEO_RENDERER_INTERFACES) $(VIDEO_RENDERER_IMPLEMENTATIONS)

libOSDLVideoRenderer_la_CPPFLAGS = -I@OSDL_BASIC@ -I@OSDL_VIDEO@ -I@OSDL_EVENTS@

libOSDLVideoRendererIncludedir      = $(includedir)/OSDL
libOSDLVideoRendererInclude_HEADERS = $(VIDEO_RENDERER_INTERFACES)


VIDEO_RENDERER_INTERFACES = \
	OSDLRendererIncludes.h                \
	OSDLSprite.h                          \
	OSDLVideoRenderer.h                   \
	OSDLVideoRendererIncludes.h


VIDEO_RENDERER_IMPLEMENTATIONS = \
	OSDLSprite.cc                         \
	OSDLVideoRenderer.cc
	
	
