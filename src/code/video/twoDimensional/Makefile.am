## Makefile.am for the submodule 'Two Dimensional' of the 'Video' module 
## of the OSDL library.


# Convenience library for the 'Two Dimensional' submodule :
noinst_LTLIBRARIES = libOSDLTwoDimensional.la

libOSDLTwoDimensional_la_SOURCES = $(TWO_DIMENSIONAL_INTERFACES) $(TWO_DIMENSIONAL_IMPLEMENTATIONS)

libOSDLTwoDimensional_la_CPPFLAGS = -I@OSDL_BASIC@ -I@OSDL_VIDEO@ -I@OSDL_EVENTS@

libOSDLTwoDimensionalIncludedir      = $(includedir)/OSDL
libOSDLTwoDimensionalInclude_HEADERS = $(TWO_DIMENSIONAL_INTERFACES)


TWO_DIMENSIONAL_INTERFACES = \
	OSDLBackBufferedWidget.h              \
	OSDLBezier.h                          \
	OSDLConic.h                           \
	OSDLFixedFont.h                       \
	OSDLFont.h                            \
	OSDLGLTexture.h                       \
	OSDLGLUprightRectangle.h              \
	OSDLImage.h                           \
	OSDLLine.h                            \
	OSDLPoint2D.h                         \
	OSDLPoint3D.h                         \
	OSDLPoint.h                           \
	OSDLPolygon.h                         \
	OSDLTextWidget.h                      \
	OSDLTrueTypeFont.h                    \
	OSDLTwoDimensionalIncludes.h          \
	OSDLUprightRectangle.h                \
	OSDLWidget.h


TWO_DIMENSIONAL_IMPLEMENTATIONS = \
	OSDLBackBufferedWidget.cc             \
	OSDLBezier.cc                         \
	OSDLConic.cc                          \
	OSDLFixedFont.cc                      \
	OSDLFont.cc                           \
	OSDLGLTexture.cc                      \
	OSDLGLUprightRectangle.cc             \
	OSDLImage.cc                          \
	OSDLLine.cc                           \
	OSDLPoint2D.cc                        \
	OSDLPoint3D.cc                        \
	OSDLPoint.cc                          \
	OSDLPolygon.cc                        \
	OSDLTextWidget.cc                     \
	OSDLTrueTypeFont.cc                   \
	OSDLUprightRectangle.cc               \
	OSDLWidget.cc
	
