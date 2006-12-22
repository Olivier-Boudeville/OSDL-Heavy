## Makefile.am for the 'Engine' module of the OSDL library.



# Convenience library for the 'Engine' module :
noinst_LTLIBRARIES = libOSDLEngine.la

libOSDLEngine_la_SOURCES = $(ENGINE_INTERFACES) $(ENGINE_IMPLEMENTATIONS)

libOSDLEngine_la_CPPFLAGS = -I@OSDL_BASIC@ -I@OSDL_EVENTS@ -I@OSDL_VIDEO@ -I@OSDL_VIDEO_TWO_DIMENSIONAL@ -I@OSDL_VIDEO_RENDERER@ -I@OSDL_AUDIO_RENDERER@

libOSDLEngineIncludedir      = $(includedir)/OSDL
libOSDLEngineInclude_HEADERS = $(ENGINE_INTERFACES)


ENGINE_INTERFACES = \
	OSDLActiveObject.h                    \
	OSDLBoundingBox2D.h                   \
	OSDLBoundingBox.h                     \
	OSDLCircleBoundingBox.h               \
	OSDLEngineCommon.h                    \
	OSDLEngineIncludes.h                  \
	OSDLModel.h                           \
	OSDLMultimediaRenderer.h              \
	OSDLPeriodicSlot.h                    \
	OSDLRenderer.h                        \
	OSDLScheduler.h                       \
	OSDLWorldObject.h


ENGINE_IMPLEMENTATIONS = \
	OSDLActiveObject.cc                   \             
	OSDLBoundingBox2D.cc                  \
	OSDLBoundingBox.cc                    \
	OSDLCircleBoundingBox.cc              \
	OSDLEngineCommon.cc                   \
	OSDLModel.cc                          \
	OSDLMultimediaRenderer.cc             \
	OSDLPeriodicSlot.cc                   \
	OSDLRenderer.cc                       \
	OSDLScheduler.cc                      \
	OSDLWorldObject.cc

	
