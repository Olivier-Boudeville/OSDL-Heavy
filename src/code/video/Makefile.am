## Makefile.am for the 'Video' module of the OSDL library.


SUBDIRS = graphicalUserInterface twoDimensional videoRenderer


# Convenience library for the 'Video' module :
noinst_LTLIBRARIES = libOSDLVideo.la

libOSDLVideo_la_SOURCES = $(VIDEO_INTERFACES) $(VIDEO_IMPLEMENTATIONS)

libOSDLVideo_la_CPPFLAGS = -I@OSDL_BASIC@ -I@OSDL_VIDEO_TWO_DIMENSIONAL@ -I@OSDL_VIDEO_RENDERER@ -I@OSDL_EVENTS@ -I@OSDL_ENGINE@

libOSDLVideoIncludedir      = $(includedir)/OSDL
libOSDLVideoInclude_HEADERS = $(VIDEO_INTERFACES)


VIDEO_INTERFACES = \
	OSDLFromGfx.h                        \
	OSDLOpenGL.h                         \
	OSDLOverlay.h                        \
	OSDLPalette.h                        \
	OSDLPixel.h                          \
	OSDLSurface.h                        \
	OSDLVideo.h                          \
	OSDLVideoIncludes.h                  \
	OSDLVideoTypes.h


VIDEO_IMPLEMENTATIONS = \
	OSDLFromGfx.cc                       \
	OSDLOpenGL.cc                        \
	OSDLOverlay.cc                       \
	OSDLPalette.cc                       \
	OSDLPixel.cc                         \
	OSDLSurface.cc                       \
	OSDLVideo.cc

